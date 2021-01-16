//
//  SpeechProcessing.c
//  SPP RF+SB
//
//  Created by Nasim Alamdari on 10/4/17.
//  Copyright (c) 2017 UT Dallas. All rights reserved.
//

#include "SpeechProcessing.h"
#include <unistd.h>
#include <android/log.h>
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define DECIMATION_FACTOR 3


static int nBands = 4;
//
static int* smoothingBuffer;
static int smoothingBufferLength;
//

inline int resolution8k(int samplingFrequency, int frameSize){
    int pow2Size = 0x01;
    while (pow2Size < frameSize)
    {
        pow2Size = pow2Size << 1;
    }
    float binResolution = (float) samplingFrequency/pow2Size;
    return (int)lround(16000.0/binResolution);
}


long* initialize(int frequency, int stepsize, int windowsize, int decisionBufferLength, float decisionRate, const char *pathHybridDir, FILE *file2,
                 double sigma,double fractionRej,  int LoadingButton, int SavingButton, int ChunkSize)
{
    Variables* inParam = (Variables*) malloc(sizeof(Variables));
    inParam->overlap = windowsize - stepsize;
    inParam->stepSize = stepsize;
    inParam->windowSize = windowsize;
    inParam->originalInput = (short*)malloc(stepsize*sizeof(short));
    inParam->frequency = frequency/DECIMATION_FACTOR;
    inParam->inputBuffer = (float*)calloc(stepsize,sizeof(float));
    inParam->decimatedStepSize = stepsize/DECIMATION_FACTOR;
    inParam->downsampled = (float*)calloc(stepsize, sizeof(float));
    inParam->decimated   = (float*)calloc(2*inParam->decimatedStepSize, sizeof(float));
    inParam->downsampleFilter = initFIR(stepsize);
    inParam->fft         = newTransform(2*inParam->decimatedStepSize, (int)(frequency/stepsize));

    inParam->detectedClass = -1;
    inParam->firstFrame = 1;
    inParam->decisionBufferLength = decisionBufferLength;
    //
    smoothingBufferLength = (int)(decisionRate*inParam->frequency)/(stepsize*decisionBufferLength);
    __android_log_print(ANDROID_LOG_DEBUG,"Initialize","%d", smoothingBufferLength);
    smoothingBuffer = calloc(sizeof(int), smoothingBufferLength);
    //
    // -------- Feature extraction ------------------------------------------------

    inParam->count = 0;
    inParam->SubbandFeatureNum    = (int)2* nBands;

    inParam->FeaturesMatrix       = (float **)malloc(decisionBufferLength * sizeof(float *));
    for ( int i = 0; i < decisionBufferLength ; i++)
        inParam->FeaturesMatrix[i] = (float *)malloc(inParam->SubbandFeatureNum * sizeof(float));

    inParam->warmUp         = decisionBufferLength;

    // initialize SubbandFeatures
    inParam->sbf    = initSubbandFeatures(inParam->fft->points, inParam->decimatedStepSize, decisionBufferLength);

    //--------------- Nasim: unsupervised classification initialization ----------------------
    inParam->sigma          = sigma;
    inParam->sizeL          = ChunkSize; //10; // number of featureFrames in Chunk
    inParam-> fractionRej   = fractionRej;
    inParam->MAJORITY_BUF   = 5;
    inParam->ClusterSize    = 20; //Maximum number of clusters that can be created
    inParam->SegmentLength  = 4 * inParam->stepSize;;
    inParam->SavingFeatButton = 0;  // 0 for disabling saving features
    inParam->LoadingButton    = LoadingButton;     // for activating or disactivating hybrid classification
    inParam->SavingButton     = SavingButton;   //for saving detected clusters for future use of hybrid classification

    //--------------- Nasim: unsupervised classification initialization ----------------------
    inParam->Clustering_ptr  = initNoiseClassifier( inParam->decisionBufferLength,
                                                    inParam->sigma,
                                                    inParam->sizeL,
                                                    (size_t)inParam->SubbandFeatureNum,
                                                    inParam->fractionRej,
                                                    inParam->MAJORITY_BUF,
                                                    inParam->ClusterSize,
                                                    inParam->SegmentLength,
                                                    pathHybridDir,
                                                    inParam->SavingFeatButton,
                                                    inParam->LoadingButton,
                                                    inParam->SavingButton,
                                                    file2);
    inParam-> ClusterLabel  = 0;
    inParam-> totalClusters = 0;
    inParam-> frameNumber   = 0;
    // --------------------------------------------------------------------------------------

    return (long*)inParam;
}

int getClass(long* memoryPointer)
{
    Variables* inParam = (Variables*)memoryPointer;
    
    return inParam->detectedClass;
}



int getClusterLabel (long* memoryPointer){
     Variables* inParam = (Variables*)memoryPointer;
    return inParam->ClusterLabel;
}

 int getTotalClusters(long* memoryPointer){
      Variables* inParam = (Variables*)memoryPointer;
     return inParam->totalClusters;
 }

void compute(long* memoryPointer, float* input, float* output, int outputType, float quiet)
{
    Variables* inParam = (Variables*)memoryPointer;
    
    int i;
    
    for (i = 0; i < inParam->overlap; i++) {
        inParam->inputBuffer[i] = inParam->inputBuffer[inParam->stepSize + i];
    }
    
    for (i=0; i<inParam->stepSize; i++) {
        inParam->originalInput[i] = (short)(input[i] * 32768);
        inParam->inputBuffer[inParam->overlap + i] = input[i];
        //__android_log_print(ANDROID_LOG_DEBUG,"AudioInput","%f",inParam->inputBuffer[i]);
    }

    int j;

    // Downsample the audio
    processFIRFilter(inParam->downsampleFilter, input, inParam->downsampled);

    // Decimate the audio
    for (i = 0, j = 0; i < inParam->decimatedStepSize; i++, j+= 3) {
        inParam->decimated[i] = inParam->decimated[i+inParam->decimatedStepSize];
        inParam->decimated[i+inParam->decimatedStepSize] = inParam->downsampled[j];
    }

    ForwardFFT(inParam->fft, inParam->decimated);
    //__android_log_print(ANDROID_LOG_ERROR,"AudioIO","DB Log Power %f", inParam->fft->dbpower);
    //if (inParam->fft->dbpower > (quiet)) {

    computeSubbandFeatures(inParam->sbf, inParam->fft->power, inParam->firstFrame );
    inParam->firstFrame = 0;

    //--------- Unsupervised noise classification -------------------
    for (int jj = 0; jj < (inParam->SubbandFeatureNum)/2 ; jj++){
        inParam->FeaturesMatrix[inParam->count][jj] = inParam->sbf->framePeriodicityBuffer[jj][inParam->count]; //first storing BandPeriodicity features, then band entropy features.
        //printf("%f\n", inParam->FeatureMatrix[inParam->count][jj]);
    }
    int jIdx = 0;
    for (int jj = (inParam->SubbandFeatureNum)/2; jj < (inParam->SubbandFeatureNum) ; jj++){
        inParam->FeaturesMatrix[inParam->count][jj] = inParam->sbf->frameEntropyBuffer[jIdx][inParam->count]; //first storing BandPeriodicity features, then band entropy features.
        jIdx++;
        //printf("%f\n", inParam->FeatureMatrix[inParam->count][jj]);
    }

        inParam->count++;

        //When the count is more than the decision buffer length
        //start warmUp condition
        if (inParam->count > inParam->warmUp -1 ) {
            inParam->count = 0;
            computeNoiseClassifier(inParam->Clustering_ptr, inParam->FeaturesMatrix);
        }

    inParam-> ClusterLabel = inParam->Clustering_ptr->DetectedClusterLabel;
    inParam-> totalClusters = inParam->Clustering_ptr->totalDetectedClusters;
    inParam-> frameNumber = inParam->Clustering_ptr->frameNumber;

    //-------------------------------------------------------------

}


void copyArray(long* memoryPointer, float** array) {
    Variables* inParam = (Variables*)memoryPointer;

    int MovAvg  = 3;
    array       = (float **)malloc(floor(inParam->decisionBufferLength/MovAvg) * sizeof(float *));
    for ( int i = 0; i < inParam->decisionBufferLength ; i++)
        array[i] = (float *)malloc( (inParam->SubbandFeatureNum + 4) * sizeof(float));

    int i,j;
    for (j = 0; j < inParam->SubbandFeatureNum; j++) {
        for (i = 0; i < floor(inParam->decisionBufferLength / MovAvg); i++) {

            float MavgSum = 0;
            for (int k = 0; k < MovAvg; k++) {
                int idx = (i * MovAvg) + k;
                MavgSum += (inParam->FeaturesMatrix[idx][j]);
            }
            double mean_MavgSum = (double) MavgSum / MovAvg;
            array[i][j] = mean_MavgSum;
        }
    }

    for (i = 0; i < floor(inParam->decisionBufferLength / MovAvg); i++) {
        array[i][inParam->SubbandFeatureNum] = inParam->fft->dbpower;
        array[i][inParam->SubbandFeatureNum + 1] = inParam->ClusterLabel;  //added for clustering results
        array[i][inParam->SubbandFeatureNum + 2] = inParam->totalClusters;  //added for clustering results
        array[i][inParam->SubbandFeatureNum + 3] = inParam->frameNumber;   //added for clustering results
    }

}

int getRowElements(long* memoryPointer) {
    Variables* inParam = (Variables*)memoryPointer;
    return inParam->SubbandFeatureNum + 4; //in original was inParam->nFeatures + 3
}

int getColElements(long* memoryPointer) {
    Variables* inParam = (Variables*)memoryPointer;
    return floor(inParam->decisionBufferLength/3); //in original was inParam->nFeatures + 3
}

float getdbPower(long* memoryPointer) {
    Variables* inParam = (Variables*)memoryPointer;
    return inParam->fft->dbpower;
}