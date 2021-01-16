//
//  audioProcessing.h
//  NoiseClustering
//
//  Created by Nasim Taghizadeh Alamdari on 8/15/17.
//  Copyright © 2017 Nasim Taghizadeh Alamdari. All rights reserved.
//

#ifndef audioProcessing_h
#define audioProcessing_h

#include <stdio.h>
#include "FIRFilter.h"
#include "Transforms.h"
#include "stdafx.h"
#include <unistd.h>
#include "Settings.h"
#include "SPLBuffer.h" //for updating dBpower
#include "SubbandFeatures.h"



typedef struct AudioProcessing{
    int stepSize;
    int decimatedStepSize;
    int firstFrame;
    float* input;
    float* downsampled;
    float* decimated;
    float* inputBuffer;
    float* processed;
    float* interpolated;
    float* output;
    float* fftPower;
    FIR* downsampleFilter;
    FIR* interpolationFilter;
    Transform* fft;
    // ------ unsupervised intialization ------
    NoiseClassification *Clustering_ptr;
    int ClusterLabel;
    int totalClusters;
    double frameNumber;
    //-------- For Feature Extraction Mode -----
    int count;
    float** FeaturesMatrix;
    int SubbandFeatureNum ;
    int  warmUp;
    float* bandFeatures;
    //-----------------------------------------
    int windowSize;
    int overlap;
    // For audio level
    SPLBuffer* spl;
    FILE *file2;
    SubbandFeatures* sbf;

} AudioProcessing;

AudioProcessing* initAudioProcessing(Settings* settings, const char *pathHybridDir, FILE *file2);
void computeAudioProc(AudioProcessing *_ptr, short* _in, short *_out, Settings* settings);
void destroyAudioProcessing(AudioProcessing** _ptr);



#endif /* audioProcessing_h */
