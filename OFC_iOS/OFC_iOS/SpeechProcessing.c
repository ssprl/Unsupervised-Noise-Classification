//
//  SpeechProcessing.c
//  SPP Equalizer
//
//  Created by Sehgal, Abhishek on 5/4/15.
//  Copyright (c) 2015 UT Dallas. All rights reserved.
//

#include "SpeechProcessing.h"


#define DECIMATION_FACTOR 3
#define EPS 1.0e-7
#define S2F 3.051757812500000e-05f
#define F2S 32768


long* initialize(Settings* settings, FILE *file2)
{
    Variables* inParam = (Variables*) malloc(sizeof(Variables));
    
    inParam->timer = newTimer();
    inParam->stepSize = settings->stepSize;
    inParam->decimatedStepSize = (int)(settings->stepSize/DECIMATION_FACTOR);
    inParam->frequency = settings->fs; // deprecated
    inParam->windowSize = settings->frameSize; // deprecated

    //inParam->inputBuffer = (float*)calloc(settings->frameSize,sizeof(float)); // deprecated
    //inParam->outputBuffer = (float*)malloc(settings->stepSize*sizeof(float)); // deprecated
    //inParam->originalInput = (short*)malloc(settings->stepSize*sizeof(short)); // deprecated
    
    inParam->input   = (float*)calloc(settings->stepSize, sizeof(float));
    inParam->inputDS = (float*)calloc(settings->stepSize, sizeof(float));
    inParam->inputDec = (float*)calloc(2*inParam->decimatedStepSize, sizeof(float));
    inParam->output   = (float*)calloc(settings->stepSize, sizeof(float));
    inParam->firFilter = initFIR(settings->stepSize);
    
    // Second filter
    inParam->interpOutput = (float*)calloc(settings->stepSize, sizeof(float));
    inParam->interpFilter = initFIR(settings->stepSize);
    
    // For audio level
    // Default is 0.5 * 48000 / 600 = 40
    inParam->spl = newSPLBuffer(settings->dbUpdateInterval * settings->fs / settings->stepSize, SPLBUFFER_MODE_TRIGGER);
    
    inParam->fft  = newTransform(2*inParam->decimatedStepSize, 1);
    inParam->synthesis = newSynthesis(inParam->decimatedStepSize, 2*inParam->decimatedStepSize, inParam->fft->window);
    inParam->overlap = settings->frameSize - settings->stepSize; // deprecated
    inParam->file2 = file2;
    
    return (long*)inParam;
}

void compute(long* memoryPointer, short* input, short* output, Settings* settings)
{
    Variables* inParam = (Variables*)memoryPointer;
    startTimer(inParam->timer);
    
    int i, j, cg;
    
    //for (i = 0; i < inParam->overlap; i++) {
    //    inParam->inputBuffer[i] = inParam->inputBuffer[inParam->stepSize + i]; // deprecated
    //}
    
    for (i=0; i<inParam->stepSize; i++) {
        //inParam->inputBuffer[inParam->overlap + i] = input[i] * S2F; // deprecated
        inParam->input[i] = input[i]*S2F;
    }
    
    // Downsample
    processFIRFilter(inParam->firFilter, inParam->input, inParam->inputDS);
    
    //Decimate
    for (i = 0, j = 0; i < inParam->decimatedStepSize; i++, j += 3){
        inParam->inputDec[i] = inParam->inputDec[inParam->decimatedStepSize + i];
        inParam->inputDec[inParam->decimatedStepSize + i] = inParam->inputDS[j];
    }
    
   ForwardFFT(inParam->fft, inParam->inputDec, settings->calibration);
    
    //ForwardFFT(inParam->fft, inParam->inputBuffer); // deprecated
    
    // buffer function updates dbpower if needed, but does not update currentGains
    cg = inParam->spl->buffer(inParam->spl, inParam->fft->dbpower, settings);
    if (settings->SavingFeatButt == 1){
        fprintf(inParam->file2, "%.2f,", inParam->fft->dbpower);}
    /* 
    // Detect audio level if needed
    if(settings->autoGains && cg != -1) {
        settings->currentGains = cg;
        switch(cg) {
            case 0: // Low
                settings->gains = settings->lowGains;
                break;
            case 1: // Medium
                settings->gains = settings->mediumGains;
                break;
            case 2: // High
                settings->gains = settings->highGains;
                break;
        }
    }
    
    equalizer(inParam->fft, settings->gains, settings->numGains);
    
    InverseFFT(inParam->fft);
    
    inParam->synthesis->doSynthesis(inParam->synthesis, inParam->fft->real);
    
    // Interpolate
    for (i = 0, j = 0; i < inParam->decimatedStepSize; i++, j += 3){
        inParam->output[j] = inParam->synthesis->output[i];
    }
    
    //Interpolate Filter
    processFIRFilter(inParam->interpFilter, inParam->output, inParam->interpOutput);
    
    if (settings->outputType) {
        for(i=0;i<inParam->stepSize;i++)
        {
            output[i] = (settings->amplification*inParam->interpOutput[i]*F2S); //  (short)(inParam->synthesis->output[i]*F2S);
        }
    } else {
        for(i=0;i<inParam->stepSize;i++)
        {
            output[i] = input[i];
        }
    }
    */
    
    stopTimer(inParam->timer);
}


