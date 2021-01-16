//
//  SPLBuffer.c
//  SPP Equalizer UI 1
//
//  Created by Akshay Chitale on 7/28/17.
//  Copyright © 2017 UT Dallas. All rights reserved.
//

#include "SPLBuffer.h"

int _SPLBuffer_buffer_trigger(SPLBuffer* spl, float db, Settings* settings);
int _SPLBuffer_buffer_range(SPLBuffer* spl, float db, Settings* settings);
int _SPLBuffer_buffer_none(SPLBuffer* spl, float db, Settings* settings);


SPLBuffer* newSPLBuffer(int len, int mode) {
    SPLBuffer* newSPLBuffer = (SPLBuffer*)malloc(sizeof(SPLBuffer));
    newSPLBuffer->bufflen = (len > 0 ? len : 1);
    newSPLBuffer->pos = 0;
    newSPLBuffer->buff = (float*)calloc(newSPLBuffer->bufflen, sizeof(float));
    switch(mode) {
        case SPLBUFFER_MODE_TRIGGER:
            newSPLBuffer->buffer = _SPLBuffer_buffer_trigger;
            break;
        case SPLBUFFER_MODE_RANGE:
            newSPLBuffer->buffer = _SPLBuffer_buffer_range;
            break;
        case SPLBUFFER_MODE_NONE:
        default:
            newSPLBuffer->buffer = _SPLBuffer_buffer_none;
    }
    return newSPLBuffer;
}

/// Return whether buffer is full (bool)
int _SPLBuffer_add_to_buffer(SPLBuffer* spl, float db) {
    if(spl->pos >= spl->bufflen) {
        spl->pos = 0;
        return 1;
    }
    spl->buff[spl->pos] = db;
    spl->pos++;
    if(spl->pos >= spl->bufflen) {
        spl->pos = 0;
        return 1;
    }
    return 0;
}

/// Buffers, returns using triggers or -1 if not ready
///
/// If settings->dbpower passes settings->lowDef, new gains are low
/// If settings->dbpower passes settings->mediumDef, new gains are medium
/// If settings->dbpower passes settings->highDef, new gains are high
int _SPLBuffer_buffer_trigger(SPLBuffer* spl, float db, Settings* settings) {
    if(_SPLBuffer_add_to_buffer(spl, db)) {
        // Buffer full, get classification and set dbpower
        int i;
        float dbavg = 0;
        for(i = 0; i < spl->bufflen; i++) {
            dbavg += spl->buff[i];
        }
        dbavg = dbavg / spl->bufflen;
        
        // Store maximum count and avg power, use highest count set (default to higher sets)
        settings->dbpower = dbavg;
        
    }
    return -1;
}

/// Buffers, returns based on ranges
///
/// Boundary between low and medium ranges is lowDef*alpha + mediumDef*(1-alpha)
/// Boundary between medium and high ranges is mediumDef*beta + highDef*(1-beta)
int _SPLBuffer_buffer_range(SPLBuffer* spl, float db, Settings* settings) {
    if(_SPLBuffer_add_to_buffer(spl, db)) {
        // Buffer full, get classification and set dbpower
        int  i;
        float dbavg = 0;
       
        for(i = 0; i < spl->bufflen; i++) {
            dbavg += spl->buff[i];
        }
        dbavg = dbavg / spl->bufflen;
        
        // Store maximum count and avg power, use highest count set (default to higher sets)
        settings->dbpower = dbavg;
       
    }
    return -1;
}

/// Never buffers anything, always returns false
int _SPLBuffer_buffer_none(SPLBuffer* spl, float db, Settings* settings) {
    return -1;
}

void destroySPLBuffer(SPLBuffer**);
