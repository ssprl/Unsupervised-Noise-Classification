//
//  SPLBuffer.h
//  SPP Equalizer UI 1
//
//  Created by Akshay Chitale on 7/28/17.
//  Copyright © 2017 UT Dallas. All rights reserved.
//

#ifndef SPLBuffer_h
#define SPLBuffer_h

#include "Settings.h"

// Use low, medium, and high definitions as triggers to switch to that gain
#define SPLBUFFER_MODE_TRIGGER 0

// Use low, medium and high gains such that boundaries are between the definitions, as specified by alpha and beta
#define SPLBUFFER_MODE_RANGE 1

// Do not buffer
#define SPLBUFFER_MODE_NONE -1

typedef struct SPLBuffer {
    
    int bufflen;
    int pos;
    float* buff;
    
    // Adds db value to buffer, returns whether settings has been updated with new current gains
    int(*buffer)(struct SPLBuffer* spl, float db, Settings* settings);
    
} SPLBuffer;

SPLBuffer* newSPLBuffer(int len, int mode);
void destroySPLBuffer(SPLBuffer**);

#endif /* SPLBuffer_h */
