//
//  SpeechProcessing.h
//  SPP Equalizer
//
//  Created by Sehgal, Abhishek on 5/4/15.
//  Copyright (c) 2015 UT Dallas. All rights reserved.
//

#ifndef __Speech_Processing_Pipeline__SpeechProcessing__
#define __Speech_Processing_Pipeline__SpeechProcessing__

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "Timer.h"
#include "Transforms.h"
#include "FIRFilter.h"
#include "Synthesis.h"
#include "Settings.h"
#include "SPLBuffer.h"

typedef struct Variables{
    
    int stepSize;
    int decimatedStepSize;
    int windowSize;
    int overlap;
    int frequency;
    
    float* input;
    float* inputDS;
    float* inputDec;
    float* output;
    float* inputBuffer;
    float* outputBuffer;
    short* originalInput;
    
    Timer *timer;
    FIR *firFilter;
    Transform *fft;
    Synthesis * synthesis;
    
    // Second filter
    float* interpOutput;
    FIR *interpFilter;
    
    // For audio level
    SPLBuffer* spl;
    FILE *file2;
    
} Variables;

long* initialize(Settings* settings, FILE *file2);
void compute(long* memoryPointer, short* input, short* output, Settings* settings);


#endif /* defined(__Speech_Processing_Pipeline__SpeechProcessing__) */
