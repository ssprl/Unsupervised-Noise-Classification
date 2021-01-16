//
//  audioProcessing.c
//  NoiseClustering
//
//  Created by Nasim Taghizadeh Alamdari on 8/15/17.
//  Copyright © 2017 Nasim Taghizadeh Alamdari. All rights reserved.
//
#define DECIMATION_FACTOR 3
#define EPS 1.0e-7
#define S2F 3.051757812500000e-05f
#define F2S 32768



#include "audioProcessing.h"

AudioProcessing* initAudioProcessing(Settings* settings, const char *pathHybridDir, FILE *file2){

    AudioProcessing* inParam = (AudioProcessing*)malloc(sizeof(AudioProcessing));
    
    inParam->stepSize           = settings->stepSize;
    inParam->decimatedStepSize  = settings->stepSize/DECIMATION_FACTOR;
    inParam->firstFrame         = 1;
    inParam->overlap        = settings->frameSize - settings->stepSize; // in clustering 50% overlap needed, so windowsize = 2*stepsize.
    inParam->input          = (float*)calloc(settings->stepSize, sizeof(float));
    inParam->downsampled    = (float*)calloc(settings->stepSize, sizeof(float));
    inParam->decimated      = (float*)calloc(inParam->decimatedStepSize, sizeof(float));
    inParam->inputBuffer    = (float*)calloc(2*inParam->decimatedStepSize, sizeof(float));
    inParam->processed      = (float*)calloc(inParam->decimatedStepSize, sizeof(float));
    inParam->interpolated   = (float*)calloc(settings->stepSize, sizeof(float));
    inParam->output         = (float*)calloc(settings->stepSize, sizeof(float));

    inParam->downsampleFilter       = initFIR(settings->stepSize);
    inParam->interpolationFilter    = initFIR(settings->stepSize);

    inParam->fft            = newTransform (2*inParam->decimatedStepSize, 1);
    inParam->fftPower       = (float*)calloc((int)(inParam->fft->points/2), sizeof(float));
    
    //subbandFeatures_initialize();
    inParam->bandFeatures   = (float*)calloc(8, sizeof(float)); // 8 is total number of features
    
    // initialize SubbandFeatures
    inParam->sbf    = initSubbandFeatures(inParam->fft->points, inParam->decimatedStepSize, settings->decisionbufferLength);
    // -------- Feature extraction modes ------------------------------------------------
    
    inParam->count = 0;
    inParam->SubbandFeatureNum    = (int)settings->NumFeatures;
    
    inParam->FeaturesMatrix       = (float **)malloc(settings->decisionbufferLength * sizeof(float *));
    for ( int i = 0; i < settings->decisionbufferLength ; i++)
        inParam->FeaturesMatrix[i] = (float *)malloc(settings->NumFeatures * sizeof(float));
    
    inParam->warmUp         = settings->decisionbufferLength;
   
    // For audio level
    // Default is 0.5 * 48000 / 600 = 40
    inParam->spl   = newSPLBuffer(settings->dbUpdateInterval * settings->fs / settings->stepSize, SPLBUFFER_MODE_TRIGGER);
    inParam->file2 = file2;
    
    //---------------------------------------------------------------
    //--------------- Nasim: unsupervised classification initialization ----------------------
    inParam->Clustering_ptr  = initNoiseClassifier( settings->decisionbufferLength,
                                                    settings->sigma,
                                                    settings->sizeL,
                                                    settings->NumFeatures,
                                                    settings->fractionRej,
                                                    settings->MAJORITY_BUF,
                                                    settings->ClusterSize,
                                                    settings->SegmentLength,
                                                    pathHybridDir,
                                                    settings->SavingFeatButt,
                                                    settings->LoadingButt,
                                                    settings->SavingButt,
                                                    file2);
    inParam-> ClusterLabel  = 0;
    inParam-> totalClusters = 0;
    inParam-> frameNumber   = 0;
    // --------------------------------------------------------------------------------------
    
    return inParam;
}



void computeAudioProc(AudioProcessing *_ptr, short* _in, short *_out, Settings* settings){
    AudioProcessing *inParam = _ptr;
    
    int i,j;
    
    // Convert from Short to Float
    for (i = 0; i < inParam->stepSize; i++) {
        inParam->input[i] = _in[i] * S2F;
    }
    
    // Downsample the audio
    processFIRFilter(inParam->downsampleFilter, inParam->input, inParam->downsampled);
    
    // Decimate the audio
    for (i = 0, j = 0; i < inParam->decimatedStepSize; i++, j+= 3) {
        inParam->decimated[i] = inParam->downsampled[j];
    }
    
    for(i = 0; i < inParam->decimatedStepSize; i++){
        inParam->inputBuffer[i] = inParam->inputBuffer[i+inParam->decimatedStepSize];
        inParam->inputBuffer[i+inParam->decimatedStepSize] = inParam->decimated[i];
    }
    
    // Process the Audio
    /**************************************/
    
    //ForwardFFTransform(inParam->fft, inParam->inputBuffer);
    ForwardFFT(inParam->fft, inParam->inputBuffer,settings->calibration);
    memcpy(inParam->fftPower, inParam->fft->power, (int)(inParam->fft->points/2) * sizeof(float));
    
    int cg;
    cg = inParam->spl->buffer(inParam->spl, inParam->fft->dbpower, settings);
    if (settings->SavingFeatButt == 1){
        fprintf(inParam->file2, "%.2f,", inParam->fft->dbpower);}
    
    //============= Feature Extraction and Unsupervised noise classification ================
    //subbandFeatures(inParam->fftPower, inParam->firstFrame, inParam->bandFeatures);
    computeSubbandFeatures(inParam->sbf, inParam->fft->power, inParam->firstFrame );
    inParam->firstFrame = 0;
        
    /*for (int jj = 0; jj < (inParam->SubbandFeatureNum ); jj++){
        inParam->FeaturesMatrix[inParam->count][jj] = inParam->bandFeatures[jj]; //first storing BandPeriodicity features, then band entropy features.
        //printf("%f\n", inParam->FeatureMatrix[inParam->count][jj]);
    }*/
    
    for (int jj = 0; jj < (inParam->SubbandFeatureNum)/2 ; jj++){
        inParam->FeaturesMatrix[inParam->count][jj] = inParam->sbf->framePeriodicityBuffer[jj][inParam->count]; //first storing BandPeriodicity features, then band entropy features.
        
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
    //============= End of unsupervised classification ==========================
    
    
    //Interpolate the Audio
    for (i = 0, j = 0; i < inParam->decimatedStepSize; i++, j+=3) {
        inParam->interpolated[j] = inParam->decimated[i];
    }
    
    // Low-Pass filter the Interpolated Audio
    processFIRFilter(inParam->interpolationFilter, inParam->interpolated, inParam->output);
    
    
    // If noise reduction is enabled, output processed audio
    // Otherwise output original audio
    
    for (i = 0; i < inParam->stepSize; i++) {
        _out[i] = (short)(2*F2S * inParam->output[i]);
    }
    
}

void destroyAudioProcessing(AudioProcessing** _ptr){
    if (*_ptr != NULL) {
        
        if ((*_ptr)->input != NULL){
            free((*_ptr)->input);
            (*_ptr)->input = NULL;
        }
        if ((*_ptr)->downsampled != NULL){
            free((*_ptr)->downsampled);
            (*_ptr)->downsampled = NULL;
        }
        if ((*_ptr)->decimated != NULL){
            free((*_ptr)->decimated);
            (*_ptr)->decimated = NULL;
        }
        if ((*_ptr)->processed != NULL){
            free((*_ptr)->processed);
            (*_ptr)->processed = NULL;
        }
        if ((*_ptr)->interpolated != NULL){
            free((*_ptr)->interpolated);
            (*_ptr)->interpolated = NULL;
        }
        if ((*_ptr)->output != NULL){
            free((*_ptr)->output);
            (*_ptr)->output = NULL;
        }
        destroyFIR(&(*_ptr)->downsampleFilter);
        destroyFIR(&(*_ptr)->interpolationFilter);
        destroyTransform(&(*_ptr)->fft);
        
        destroySubbandFeatures(&(*_ptr)->sbf);
        destroyNoiseClassifier(&(*_ptr)->Clustering_ptr);
        
        free(*_ptr);
        *_ptr = NULL;
    }
}
