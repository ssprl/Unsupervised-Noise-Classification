 //
//  IosAudioController.m
//  Aruts
//
//  Created by Simon Epskamp on 10/11/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "IosAudioController.h"
#import <AudioToolbox/AudioToolbox.h>
#import <time.h>
#import "Settings.h"
#import "TPCircularBuffer.h"
#import "SpeechProcessing.h"
#import "audioProcessing.h" // Nasim added

NSDate *start, *stop;
NSTimeInterval executionTime = 0, overallExecutionTime = 0;
#define kOutputBus 0
#define kInputBus 1

AudioProcessing* memoryPointer2;

IosAudioController* iosAudio;
uint64_t duration = 0;
uint64_t startTime;
int count = 0;
AudioStreamBasicDescription audioFormat;
NSData *classifierParams;


//Added by Nasim ------------------
const char *pathHybridDir;
const char *pathFeatureSaveDir;
FILE     *file;
FILE     *file2;
NSString *str;
NSString *UserStr;
//-----------------------------------



long* memoryPointer;
ExtAudioFileRef fileRef;
TPCircularBuffer* inBuffer;
TPCircularBuffer* outBuffer;
MovingAverageBuffer* timeBuffer;

void checkStatus(OSStatus error){
    if (error) {
        char errorString[20];
        // See if it appears to be a 4-char-code
        *(UInt32 *)(errorString + 1) = CFSwapInt32HostToBig(error);
        if (isprint(errorString[1]) && isprint(errorString[2]) &&
            isprint(errorString[3]) && isprint(errorString[4])) {
            errorString[0] = errorString[5] = '\'';
            errorString[6] = '\0';
        } else
            // No, format it as an integer
            sprintf(errorString, "%d", (int)error);
        
        fprintf(stderr, "Error: (%s)\n", errorString);
    }
}

//Error Check Function
static void CheckError (OSStatus error, const char *operation) {
    if (error == noErr) return;
    
    char errorString[20];
    // See if it appears to be a 4-char-code
    *(UInt32 *)(errorString + 1) = CFSwapInt32HostToBig(error);
    if (isprint(errorString[1]) && isprint(errorString[2]) &&
        isprint(errorString[3]) && isprint(errorString[4])) {
        errorString[0] = errorString[5] = '\'';
        errorString[6] = '\0';
    } else
        // No, format it as an integer
        sprintf(errorString, "%d", (int)error);
    
    fprintf(stderr, "Error: %s (%s)\n", operation, errorString);
    
    exit(1);
}


 
/**
 This callback is called when new audio data from the microphone is
 available.
 */
static OSStatus recordingCallback(void *inRefCon,
                                  AudioUnitRenderActionFlags *ioActionFlags,
                                  const AudioTimeStamp *inTimeStamp,
                                  UInt32 inBusNumber,
                                  UInt32 inNumberFrames,
                                  AudioBufferList *ioData) {
    
    Settings* settings = (Settings*)(inRefCon); // For accessing settings
    
    // Because of the way our audio format (setup below) is chosen:
    // we only need 1 buffer, since it is mono
    // Samples are 16 bits = 2 bytes.
    // 1 frame includes only 1 sample
    
    AudioBuffer buffer;
    
    buffer.mNumberChannels = 1;
    buffer.mDataByteSize = inNumberFrames * 2;
    buffer.mData = malloc( inNumberFrames * 2 );
    
    // Put buffer in a AudioBufferList
    AudioBufferList bufferList;
    bufferList.mNumberBuffers = 1;
    bufferList.mBuffers[0] = buffer;
    
    // Then:
    // Obtain recorded samples
    
    if (settings->micStatus) {
        OSStatus status;
        
        status = AudioUnitRender([iosAudio audioUnit],
                                 ioActionFlags,
                                 inTimeStamp,
                                 inBusNumber,
                                 inNumberFrames,
                                 &bufferList);
        checkStatus(status);
        
        // Now, we have the samples we just read sitting in buffers in bufferList
        // Process the new data
        TPCircularBufferProduceBytes(inBuffer, (void*)bufferList.mBuffers[0].mData, bufferList.mBuffers[0].mDataByteSize);
        
        if(inBuffer->fillCount >= settings->frameSize*sizeof(short)){
            //Add timer here.
            start = [NSDate date];
            [iosAudio processStream];
          
            [timeBuffer addDatum:[NSNumber numberWithFloat:[[NSDate date] timeIntervalSinceDate:start]]];
            iosAudio.overallExecutionTime = 1000*[timeBuffer movingAverage];
            printf("\n overall Execution time: %f \n",iosAudio.overallExecutionTime);
        }
    }
    else {
        UInt32 frameCount = settings->frameSize;
        OSStatus err = ExtAudioFileRead(fileRef, &frameCount, &bufferList);
        CheckError(err,"File Read");
        if(frameCount > 0) {
            
            AudioBuffer audioBuffer = bufferList.mBuffers[0];
            
            TPCircularBufferProduceBytes(inBuffer, audioBuffer.mData, audioBuffer.mDataByteSize);
            
            if (inBuffer->fillCount >= settings->frameSize*sizeof(short)) {
                [iosAudio processStream];
            }
        }
        
        else{
            
            [iosAudio stop];
            //enableButtons();
        }
    }
    
    
    // release the malloc'ed data in the buffer we created earlier
    free(bufferList.mBuffers[0].mData);
    
    return noErr;
}

/**
 This callback is called when the audioUnit needs new data to play through the
 speakers. If you don't have any, just don't write anything in the buffers
 */
static OSStatus playbackCallback(void *inRefCon,
                                 AudioUnitRenderActionFlags *ioActionFlags,
                                 const AudioTimeStamp *inTimeStamp,
                                 UInt32 inBusNumber,
                                 UInt32 inNumberFrames,
                                 AudioBufferList *ioData) {
    
    Settings* settings = (Settings*)(inRefCon); // For accessing settings
    
    // Notes: ioData contains buffers (may be more than one!)
    // Fill them up as much as you can. Remember to set the size value in each buffer to match how
    // much data is in the buffer.
    if (!settings->playAudio) {
        void* emptyBuffer = calloc(inNumberFrames,sizeof(short));
        memcpy(ioData->mBuffers[0].mData, emptyBuffer, ioData->mBuffers[0].mDataByteSize);
        free(emptyBuffer);
        return noErr;
    }
    
    
    for (int i=0; i < ioData->mNumberBuffers; i++) { // in practice we will only ever have 1 buffer, since audio format is mono
        AudioBuffer buffer = ioData->mBuffers[i];
        
        if (outBuffer->fillCount > ioData->mBuffers[0].mDataByteSize) {
            
            int32_t availableBytes;
            short *tail = TPCircularBufferTail(outBuffer, &availableBytes);
            
            memcpy(buffer.mData, tail, buffer.mDataByteSize);
            TPCircularBufferConsume(outBuffer, buffer.mDataByteSize);
            
        }
    }
    
    return noErr;
}

@implementation IosAudioController

@synthesize audioUnit, tempBuffer, settings, clusterLabel, frameNumber, overallExecutionTime, totalDetectedClusters;

/**
 Initialize the audioUnit and allocate our own temporary buffer.
 The temporary buffer will hold the latest data coming in from the microphone,
 and will be copied to the output when this is requested.
 */
- (id) init {
    self = [super init];
    
    self.settings = newSettings();
    
    [self initAudioUnit:true];
    
    return self;
}

/**
 Destroy settings struct
 */
- (void) destroySettings {
    destroySettings(self.settings);
}

/**
 Initializes audio, if BOOL constructor is true, does it for the initializer
 */
- (void) initAudioUnit: (BOOL)constructor {
    OSStatus status;
    
    if(!constructor) {
        CheckError(AudioUnitUninitialize(audioUnit), "AudioUnit Uninitialization");
    }
    
    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord
                                           error:NULL];
    [[AVAudioSession sharedInstance] setMode:AVAudioSessionModeMeasurement
                                       error:NULL];
    [[AVAudioSession sharedInstance] setPreferredSampleRate:self.settings->fs
                                                      error:NULL];
    [[AVAudioSession sharedInstance] setPreferredIOBufferDuration:64/(float)self.settings->fs
                                                            error:NULL]; // Nasim: frame samples was 64, but due to skipping frames during recording, it replaced with 128
    
    // Describe audio component
    AudioComponentDescription desc;
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_RemoteIO;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    
    // Get component
    AudioComponent inputComponent = AudioComponentFindNext(NULL, &desc);
    
    // Get audio units
    status = AudioComponentInstanceNew(inputComponent, &audioUnit);
    checkStatus(status);
    
    // Enable IO for recording
    UInt32 flag = 1;
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioOutputUnitProperty_EnableIO,
                                  kAudioUnitScope_Input,
                                  kInputBus,
                                  &flag,
                                  sizeof(flag));
    checkStatus(status);
    
    // Enable IO for playback
//    if(!constructor) {
//        flag = [self settings]->playAudio;
//    }
    flag = 1;
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioOutputUnitProperty_EnableIO,
                                  kAudioUnitScope_Output,
                                  kOutputBus,
                                  &flag,
                                  sizeof(flag));
    checkStatus(status);
    
    // Describe format
    //AudioStreamBasicDescription audioFormat;
    audioFormat.mSampleRate			= [self settings]->fs;
    if(constructor) {
        audioFormat.mFormatID			= kAudioFormatLinearPCM;
        audioFormat.mFormatFlags		= kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
        audioFormat.mFramesPerPacket	= 1;
        audioFormat.mChannelsPerFrame	= 1;
        audioFormat.mBitsPerChannel		= 16;
        audioFormat.mBytesPerPacket		= 2;
        audioFormat.mBytesPerFrame		= 2;
    }
    
    // Apply format
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Output,
                                  kInputBus,
                                  &audioFormat,
                                  sizeof(audioFormat));
    checkStatus(status);
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Input,
                                  kOutputBus,
                                  &audioFormat,
                                  sizeof(audioFormat));
    checkStatus(status);
    
    
    // Set input callback
    AURenderCallbackStruct callbackStruct;
    callbackStruct.inputProc = recordingCallback;
    callbackStruct.inputProcRefCon = (void *)([self settings]); // Only need settings, not all of self
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioOutputUnitProperty_SetInputCallback,
                                  kAudioUnitScope_Global,
                                  kInputBus,
                                  &callbackStruct,
                                  sizeof(callbackStruct));
    checkStatus(status);
    
    // Set output callback
    callbackStruct.inputProc = playbackCallback;
    callbackStruct.inputProcRefCon = (void *)([self settings]); // Only need settings, not all of self
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioUnitProperty_SetRenderCallback,
                                  kAudioUnitScope_Global,
                                  kOutputBus,
                                  &callbackStruct,
                                  sizeof(callbackStruct));
    checkStatus(status);
    
    // Disable buffer allocation for the recorder (optional - do this if we want to pass in our own)
    flag = 0;
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioUnitProperty_ShouldAllocateBuffer,
                                  kAudioUnitScope_Output,
                                  kInputBus,
                                  &flag,
                                  sizeof(flag));
    
    
    // Allocate our own buffers (1 channel, 16 bits per sample, thus 16 bits per frame, thus 2 bytes per frame).
    // Practice learns the buffers used contain 512 frames, if this changes it will be fixed in processAudio.
    tempBuffer.mNumberChannels = 1;
    tempBuffer.mDataByteSize = [self settings]->frameSize * sizeof(short);
    tempBuffer.mData = malloc( [self settings]->frameSize * sizeof(short) );
    
    NSLog(@"Sampling rate: %.0f", [AVAudioSession sharedInstance].sampleRate);
    NSLog(@"Input Buffer: %.0f", [AVAudioSession sharedInstance].IOBufferDuration*[AVAudioSession sharedInstance].sampleRate);
    timeBuffer = [[MovingAverageBuffer alloc] initWithPeriod:round(settings->fs/settings->frameSize)];
    // Initialise
    status = AudioUnitInitialize(audioUnit);
    
    checkStatus(status);
}

/**
 Start the audioUnit. This means data will be provided from
 the microphone, and requested for feeding to the speakers, by
 use of the provided callbacks.
 */
- (void) start {
    startTime = clock();
    
    openFile();
    
  
    
    
    //------------------- Getting paths fot saving and loading files ----------------------------------------------
    if (settings->SavingButt){
        
        str = @"NoiseClasses";
        NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
        [formatter setDateFormat:@"MM_dd_yyyy_HH_mm_ss"];
        NSString* dateString = [formatter stringFromDate:[NSDate date]]; // string of file
        //NSArray  *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES); //path of file
        //NSString *documentsDirectory = [paths objectAtIndex:0];
        NSArray *paths      = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);//path for file
        NSString *documentsDirectory = [paths objectAtIndex:0];
        NSString *documents_dir = [documentsDirectory stringByAppendingString:[NSString stringWithFormat:@"/%@_%@.txt",str, dateString]];
        NSLog(@"%@" ,documents_dir); // only for cheking file name format, NSLog is for display
        const char *pathDir = [documents_dir UTF8String];
        file = fopen(pathDir, "w");
    }
    if (settings->LoadingButt){
        str = @"Hybrid_clusterParameters";
        NSArray *paths      = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);//path for file
        NSString *documentsDirectory = [paths objectAtIndex:0];
        NSString *Hybrid_dir = [documentsDirectory stringByAppendingString:[NSString stringWithFormat:@"/%@.dat",str]];
        NSLog(@"%@" ,Hybrid_dir); // only for cheking file name format, NSLog is for display
        pathHybridDir = [Hybrid_dir UTF8String];
    }
    if (settings->SavingFeatButt){
        str     = @"ExtractedFeatures_";
        UserStr = [NSString stringWithUTF8String:settings->UserDefinedNoiseType];
        NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
        [formatter setDateFormat:@"MM_dd_yyyy_HH_mm_ss"];
        NSString* dateString = [formatter stringFromDate:[NSDate date]]; // string of file
        NSArray *paths      = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);//path for file
        NSString *documentsDirectory = [paths objectAtIndex:0];
        NSString *featuresSave_dir = [documentsDirectory stringByAppendingString:[NSString stringWithFormat:@"/%@_%@.txt",str, dateString]];
        NSLog(@"%@" ,featuresSave_dir); // only for cheking file name format, NSLog is for display
        pathFeatureSaveDir = [featuresSave_dir UTF8String];
        file2 = fopen(pathFeatureSaveDir, "wb");
    }
    //--------------------------------------------------------------------------------------
    //memoryPointer = initialize([self settings], file2);
    memoryPointer2 = initAudioProcessing([self settings],pathHybridDir,file2);

    
    inBuffer = malloc(sizeof(TPCircularBuffer));
    outBuffer = malloc(sizeof(TPCircularBuffer));
    TPCircularBufferInit(inBuffer, 4096);
    TPCircularBufferInit(outBuffer, 4096);
    

    [self initAudioUnit:false];
    //initAudio([NSString stringWithUTF8String:[self settings]->fileName]); // deprecated
    
    audioFormat.mSampleRate = [self settings]->fs;
    OSStatus status = AudioUnitSetProperty(audioUnit,
                                           kAudioUnitProperty_StreamFormat,
                                           kAudioUnitScope_Output,
                                           kInputBus,
                                           &audioFormat,
                                           sizeof(audioFormat));
    checkStatus(status);
    
    
    
    status = AudioOutputUnitStart(audioUnit);
    checkStatus(status);
    printf("Audio Started (IosAudioController.m)\n");
}

/**
 Stop the audioUnit
 */
- (void) stop {
    OSStatus status = AudioOutputUnitStop(audioUnit);
    checkStatus(status);
    fclose(file);
    printf("Audio Stopped (IosAudioController.m)\n");
}

- (void) processStream {
    
    //Frame Size
    UInt32 frameSize = settings->stepSize * sizeof(short);
    int32_t availableBytes;
    
    //Initialize Temporary buffers for processing
    short *tail = TPCircularBufferTail(inBuffer, &availableBytes);
    
    if (availableBytes > frameSize){
        short *buffer = malloc(frameSize), *output = malloc(frameSize);
        
        memcpy(buffer, tail, frameSize);
        
        TPCircularBufferConsume(inBuffer, frameSize);
        
        //compute(memoryPointer, buffer, output, settings);
        
        //********************** Nasim: Unsupervised Classification ******************************************
        short* head = TPCircularBufferHead(outBuffer, &availableBytes);
        computeAudioProc(memoryPointer2, tail, head, settings);
        
        iosAudio.clusterLabel = memoryPointer2->ClusterLabel;
        iosAudio.frameNumber = memoryPointer2->frameNumber;
        iosAudio.totalDetectedClusters = memoryPointer2->totalClusters;
        
        if (settings->SavingButt){
                if (file != NULL) {
                    fprintf(file, "%d, %d, %.1f; ", clusterLabel, totalDetectedClusters, frameNumber);
   
                    }
                else{
                    printf("\n Error! Can not open the file for writing \n");
                }
        }
        //**********************************************************************
        
        TPCircularBufferProduceBytes(outBuffer, head, frameSize);
        
        free(output);
        free(buffer);
        
        duration+=clock() - startTime;
        count++;
        startTime = clock();
        
    }
    
}


void openFile()
{
    classifierParams = [NSData dataWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"GMMParam"
                                                                                      ofType:@"txt"]];    
    
}



@end
