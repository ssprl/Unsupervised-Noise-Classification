//
//  FIRFilter.c
//  NC-2Ch
//
//  Created by Abhishek Sehgal on 5/18/17.
//  Copyright © 2017 SIPLab. All rights reserved.
//

#include "FIRFilter.h"
//#include "filterCoefficients.h"

#define NCOEFFS 81


const float filterCoefficients[] = {-0.000159,-0.000250,-0.000188,0.000124,0.000525,0.000641,0.000187,-0.000668,-0.001240,-0.000812,0.000627,0.002062,0.002071,0.000097,-0.002663,-0.003814,-0.001708,0.002704,0.005952,0.004603,-0.001444,-0.007877,-0.008781,-0.001761,0.008842,0.014153,0.007787,-0.007572,-0.020182,-0.017558,0.002323,0.026206,0.033056,0.010520,-0.031329,-0.060927,-0.043355,0.034802,0.151141,0.255497,0.297319,0.255497,0.151141,0.034802,-0.043355,-0.060927,-0.031329,0.010520,0.033056,0.026206,0.002323,-0.017558,-0.020182,-0.007572,0.007787,0.014153,0.008842,-0.001761,-0.008781,-0.007877,-0.001444,0.004603,0.005952,0.002704,-0.001708,-0.003814,-0.002663,0.000097,0.002071,0.002062,0.000627,-0.000812,-0.001240,-0.000668,0.000187,0.000641,0.000525,0.000124,-0.000188,-0.000250,-0.000159};


float checkRange(float input){
    float output;
    if (input > 1.0) {
        output = 1.0;
    }
    else if(input < -1.0){
        output = -1.0;
    }
    else {
        output = input;
    }
    
    return output;
}

FIR* initFIR(int stepSize) {
    
    FIR* fir = (FIR*)malloc(sizeof(FIR));
    
    fir->N = stepSize;
    
    fir->inputBuffer = (float*)calloc(2*stepSize, sizeof(float));
    
    return fir;
    
}

void processFIRFilter(FIR* fir, float* input, float* output) {
    
    int i,j, idx;
    float temp;
    
    for (i = 0; i < fir->N; i++) {
        fir->inputBuffer[i] = fir->inputBuffer[fir->N + i];
        fir->inputBuffer[fir->N + i] = input[i];
    }
    
    for (i = 0; i < fir->N; i++) {
        temp = 0;
        
        for (j = 0; j < NCOEFFS; j++) {
            idx = fir->N + (i - j);
            temp += (fir->inputBuffer[idx]*filterCoefficients[j]);
        }
        output[i] = checkRange(temp);
    }
}

void destroyFIR(FIR **fir) {
    
    if ((*fir) != NULL) {
        
        if ((*fir)->inputBuffer != NULL) {
            free((*fir)->inputBuffer);
            (*fir)->inputBuffer = NULL;
        }
        
        free((*fir));
        (*fir) = NULL;        
    }
    
}
