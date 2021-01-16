//
//  Settings.c
//  SPP Equalizer UI 1
//
//  Created by Akshay Chitale on 6/25/17.
//  Copyright © 2017 UT Dallas. All rights reserved.
//

#import "Settings.h"

Settings* newSettings() {
    
    Settings* newSettings = (Settings*)malloc(sizeof(Settings));
    
    // Set defaults
    newSettings->fs =  48000;
    newSettings->frameSize = 1200; //;
    newSettings->stepSize = 600; //;
    
    ///******************* unsupervised Noise Classification ***********
    newSettings->windowSize           = 2 * newSettings->frameSize; //600;
    newSettings->decisionbufferLength = 39;
    newSettings->sigma                = 2.0;
    newSettings->fractionRej          = 0.01;
    newSettings->NumFeatures          = 8;
    newSettings->MAJORITY_BUF         = 5;
    newSettings->sizeL                = 10;
    newSettings->ClusterSize          = 10;
    newSettings->SegmentLength        = 4 * newSettings->stepSize;
    newSettings->LoadingButt          = 0; // this is for hybrid option, default is for loading previously detected clusters
    newSettings->SavingButt           = 0; //  default is for saving
    newSettings->SavingFeatButt       = 0; //saving feature extractions
    newSettings->DisplayUpdateRate    = 1.0;
    newSettings->ClassifficUpdateRate = 39 * newSettings->stepSize;
    newSettings->UserDefinedNoiseType = "UnknowNoiseType"; // user will define the noise type
    //*************************************************************
    newSettings->micStatus = 1;
    newSettings->playAudio = 0;
    newSettings->dbpower = 0; // needed for main view
    newSettings->calibration = -93.9794; // needed for main view
    newSettings->dbUpdateInterval = 1.0; // needed for dBpower update

    
    return newSettings;
}

void destroySettings(Settings* settings) {
    if(settings != NULL){
        free(settings);
        settings = NULL;
    }
}
