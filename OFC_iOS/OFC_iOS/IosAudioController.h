//
//  IosAudioController.h
//  Aruts
//
//  Created by Simon Epskamp on 10/11/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <AudioUnit/AudioUnit.h>
#import "Settings.h"
#import "MovingAverageBuffer.h"

#ifndef max
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif


@interface IosAudioController : NSObject {
	AudioComponentInstance 	audioUnit;
	AudioBuffer tempBuffer; // this will hold the latest data from the microphone
    Settings* settings; // holds previously global settings
    int clusterLabel;
    double frameNumber;
    float overallExecutionTime;
    int totalDetectedClusters;

}

@property (readonly) AudioComponentInstance audioUnit;
@property (readonly) AudioBuffer tempBuffer;
@property (readwrite) Settings* settings;
@property int clusterLabel;
@property double frameNumber;
@property float overallExecutionTime;
@property int totalDetectedClusters;

- (void) start;
- (void) stop;
- (void) destroySettings;
- (void) processStream;

@end

// setup a global iosAudio variable, accessible everywhere (Only for use in AudioSettingsController.swift)
extern IosAudioController* iosAudio;
