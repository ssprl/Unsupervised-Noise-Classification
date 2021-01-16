//
//  Settings.h
//  SPP Equalizer UI 1
//
//  Created by Akshay Chitale on 6/25/17.
//  Copyright © 2017 UT Dallas. All rights reserved.
//

#import <stdlib.h>

#ifndef Settings_h
#define Settings_h

typedef struct Settings {
    // Core values
    int fs;
    int frameSize;
    int stepSize;
    //******************* unsupervised Noise Classification ***********
    int windowSize;
    int decisionbufferLength;
    double sigma;
    double fractionRej;
    size_t NumFeatures;
    int MAJORITY_BUF;
    int sizeL; //number of frames in chunk, ChunkSize
    int ClusterSize; // max number of cluster detection// it was size_t
    int SegmentLength;
    int LoadingButt;
    int SavingButt;
    int SavingFeatButt;
    float DisplayUpdateRate;
    float ClassifficUpdateRate;
    const char* UserDefinedNoiseType; // user will define the noise type
    //***************************************************************
    float dbpower;
    float calibration;
    float dbUpdateInterval; // How long to wait (in seconds) before updating dbpower
    // Audio status flags
    int micStatus;
    int playAudio;
    

} Settings;

Settings* newSettings();
void destroySettings(Settings* settings);

#endif /* Settings_h */
