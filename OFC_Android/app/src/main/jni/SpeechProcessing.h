//
//  SpeechProcessing.h
//  SPP RF+SB
//
//  Created by Sehgal, Abhishek on 5/4/15.
//  Copyright (c) 2015 UT Dallas. All rights reserved.
//

#ifndef __Speech_Processing_Pipeline__SpeechProcessing__
#define __Speech_Processing_Pipeline__SpeechProcessing__

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "FFTTransform/Transforms.h"
#include "FFTTransform/FIRFilter.h"
#include "SubbandFeatures/SubbandFeatures.h"
#include "Clustering/stdafx.h"

typedef struct Variables {
    Transform* fft;
    float* inputBuffer;
    float* downsampled;
    float* decimated;
    int decimatedStepSize;
    FIR* downsampleFilter;
    short* originalInput;
    int frequency;
    int stepSize;
    int windowSize;
    int overlap;
    int detectedClass;
    int firstFrame;
    int decisionBufferLength;

    // ------ unsupervised intialization ------
    NoiseClassification *Clustering_ptr;
    int ClusterLabel;
    int totalClusters;
    double frameNumber;
    //****Settings****
    double sigma;
    double fractionRej;
    int MAJORITY_BUF;
    int sizeL; //number of frames in chunk, ChunkSize
    int ClusterSize; // max number of cluster detection// it was size_t
    int SegmentLength;
    int LoadingButton;
    int SavingButton;
    int SavingFeatButton;
    //-------- Feature extraction -----------
    int count;
    float** FeaturesMatrix;
    int SubbandFeatureNum ;
    int  warmUp;
    SubbandFeatures* sbf;
    //-----------------------------------------
} Variables;

long* initialize(int frequency, int stepsize, int windowSize, int decisionBufferLength, float decisionRate, const char *pathHybridDir, FILE *file2,
                 double sigma,double fractionRej,  int LoadingButton, int SavingButton, int ChunkSize);
void compute(long* memoryPointer, float* input, float* output, int outputType, float quiet);
const char* getClassLabel(long* memoryPointer);
void copyArray(long* memoryPointer, float** array);
int getColElements(long* memoryPointer);
int getRowElements(long* memoryPointer);
float getdbPower(long* memoryPointer);
int getClusterLabel (long* memoryPointer); //added for clustering
int getTotalClusters(long* memoryPointer); //added for lcustering

#endif /* defined(__Speech_Processing_Pipeline__SpeechProcessing__) */
