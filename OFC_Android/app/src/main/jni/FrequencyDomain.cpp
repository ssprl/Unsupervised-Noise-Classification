#include <jni.h>
#include <stdlib.h>
#include <SuperpoweredFrequencyDomain.h>
#include <AndroidIO/SuperpoweredAndroidAudioIO.h>
#include <SuperpoweredAdvancedAudioPlayer.h>
#include <SuperpoweredSimple.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>
#include <fstream>
#include <sstream>
#include "dlib/svm.h"
#include "Clustering/stdafx.h"
extern "C" {
#include "SpeechProcessing.h"
#include "Timer.h"
#include <string.h>
#include <stdbool.h> // for conversion from boolean to int
}

//Changes made by Nasim Alamdari , 2017

static SuperpoweredFrequencyDomain *frequencyDomain;
static float  *inputBufferFloat;
static float *left, *right, **features;
SuperpoweredAndroidAudioIO *audioIO;
SuperpoweredAdvancedAudioPlayer *audioPlayer;
long* memoryPointer;
Timer *timer;
static float quiet;
const char* filePath;
const char *pathHybridDir; // for clustering
FILE *file2; // for clustering

bool featureStore = false;
bool outputEnabled;
FILE* featureStoreFile;

std::string Convert (float number){
    std::ostringstream buff;
    buff<<number;
    return buff.str();
}

static void playerEventCallback(void * __unused clientData, SuperpoweredAdvancedAudioPlayerEvent event, void *value) {
    switch (event) {
        case SuperpoweredAdvancedAudioPlayerEvent_LoadSuccess: audioPlayer->play(false); break;
        case SuperpoweredAdvancedAudioPlayerEvent_LoadError: __android_log_print(ANDROID_LOG_DEBUG,
                                                                                 "NCAndroid",
                                                                                 "Open error: %s",
                                                                                 (char *)value); break;
        case SuperpoweredAdvancedAudioPlayerEvent_EOF: audioPlayer->togglePlayback(); break;
        default:;
    };
}

// This is called periodically by the media server.
static bool audioProcessing(void * __unused clientdata, short int *audioInputOutput, int numberOfSamples, int __unused samplerate) {

    //Running the algorithm
    start(timer);
    SuperpoweredShortIntToFloat(audioInputOutput, inputBufferFloat, (unsigned int)numberOfSamples); // Converting the 16-bit integer samples to 32-bit floating point.
    SuperpoweredDeInterleave(inputBufferFloat, left, right, (unsigned int)numberOfSamples);
    compute(memoryPointer, left, right, 0, quiet);
    stop(timer);

    //Storing Features
    if(featureStore){
        std::ofstream out;
        out.open(filePath,std::ios::app);
        copyArray(memoryPointer,features);
        for (int i = 0; i < getRowElements(memoryPointer); i++) {
            for (int j = 0; j < getColElements(memoryPointer); j++) {
                out << Convert(features[i][j]);
                out << ",";
            }
            out << ";\n";
        }
        out << "\n";
        out.close();
    }

    return true;
}



extern "C" JNIEXPORT void Java_com_superpowered_NCAndroid_MainActivity_FrequencyDomain(JNIEnv *javaEnvironment,
                                                                                             jobject __unused obj,
                                                                                             jint samplerate,
                                                                                             jint buffersize,
                                                                                             jfloat decisionRate,
                                                                                             jfloat quietThreshold,
                                                                                             jboolean playAudio,
                                                                                             jboolean storeFeatures,
                                                                                             jstring fileName,
                                                                                            jdouble sigma,
                                                                                            jdouble fractionRej,
                                                                                            jboolean HybridButton,
                                                                                            jboolean SavingClassifDataButton,
                                                                                            jfloat decisionBufferLength,
                                                                                            jint ChunkSize,
                                                                                            jstring HybridFileName) {

    if ( ((int) HybridButton == 1) |  ((int) SavingClassifDataButton == 1) ) {
    pathHybridDir = javaEnvironment->GetStringUTFChars(HybridFileName, NULL); // convert jstring to char*
    }
    //pathHybridDir = .c_str(); // convert string to const char*
    memoryPointer = initialize(samplerate, buffersize,2*buffersize,floor(decisionBufferLength),decisionRate, pathHybridDir, file2,sigma, fractionRej,(int) HybridButton, (int) SavingClassifDataButton, ChunkSize);// it was 80
    timer = newTimer();
    inputBufferFloat = (float *)malloc(buffersize * sizeof(float) * 2 + 128);
    left = (float *)malloc(buffersize * sizeof(float) + 128);
    right = (float *)malloc(buffersize * sizeof(float) + 128);
    quiet = quietThreshold;
    featureStore = storeFeatures;
    if (storeFeatures) {
        filePath = javaEnvironment->GetStringUTFChars(fileName,JNI_FALSE);
        features = (float **)calloc((size_t) getRowElements(memoryPointer), sizeof(float*));
        for ( size_t i = 0; i < getRowElements(memoryPointer) ; i++){
            features[i] = (float *)malloc( getColElements(memoryPointer) * sizeof(float));}
    }
    audioIO = new SuperpoweredAndroidAudioIO(samplerate, buffersize, true, playAudio, audioProcessing, javaEnvironment, -1, SL_ANDROID_STREAM_MEDIA, buffersize * 2); // Start audio input/output.

}


extern "C" JNIEXPORT void Java_com_superpowered_NCAndroid_MainActivity_StopAudio(JNIEnv * javaEnvironment, jobject __unused obj, jstring fileName){

    if(inputBufferFloat != nullptr){
        delete audioIO;
        free(inputBufferFloat);
        free(left);
        free(right);
        inputBufferFloat = nullptr;
        if(featureStore) {
            javaEnvironment->ReleaseStringUTFChars(fileName, filePath); // javaEnvironment->ReleaseStringUTFChars(JavaString,nativeString );
        }
    }
}

extern "C" JNIEXPORT float Java_com_superpowered_NCAndroid_MainActivity_getExecutionTime(JNIEnv * __unused javaEnvironment, jobject __unused obj) {
    return getMS(timer);
}

extern "C" JNIEXPORT float Java_com_superpowered_NCAndroid_MainActivity_getdbPower(JNIEnv * __unused javaEnvironment, jobject __unused obj) {
    return getdbPower(memoryPointer);
}

extern "C" JNIEXPORT int Java_com_superpowered_NCAndroid_MainActivity_getClusterLabel(JNIEnv * __unused javaEnvironment, jobject __unused obj) {
    return getClusterLabel(memoryPointer);
}

extern "C" JNIEXPORT int Java_com_superpowered_NCAndroid_MainActivity_getTotalClusters(JNIEnv * __unused javaEnvironment, jobject __unused obj) {
    return getTotalClusters(memoryPointer);
}