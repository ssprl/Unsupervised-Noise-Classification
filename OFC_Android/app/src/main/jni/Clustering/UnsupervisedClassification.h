//
//  UnsupervisedClassification.h
//
//
//  Created by Nasim Taghizadeh Alamdari on 11/1/17.
//  Copyright © 2017 UT Dallas. All rights reserved.
//

#ifndef UnsupervisedClassification_h
#define UnsupervisedClassification_h


#include <stdio.h>
#include "stdafx.h"
#include <unistd.h>
#include <string.h>

typedef struct preClass{
    int* cLabel ;
} preClass;



typedef struct NoiseClassification{

    int NFeatures;
    int detectedClass;
    double sigma;
    double maxLimit;
    int sizeC;
    int sizeChunk; //sizeL
    int SIZEL;
    int ClusterSize;
    int majorityBuff;
    int DetectedClusterLabel;
    int totalDetectedClusters;
    int decisionBufferLength;
    int warmUp;
    double** frame;
    int FlagC;
    int sizeLSim;
    int m;
    int  n;
    double fractionalRej;
    int inChunk;
    int chunkSim[9]; // needs to be variable
    double fadLabelV[10]; // needs to be variable
    int majorityBuffer;
    int smoothedLabel[5]; // nedds to be variable
    int medFiltBuff[3];
    int ClassOut[3];
    Chunk* myChunk_pre;
    SVCluster* ClusterSet_pre;
    preClass* previousCluster;
    double frameNumber;
    int count;
    double Mavg;
    float** FeatureMatrix;
    bool SavingOption;
    bool LoadingOption;
    bool savingFeats;
    const char* pathHybridDir;
    FILE *file2;

} NoiseClassification;


NoiseClassification* initNoiseClassifier(int decisionBufferLength, float sigmaValue, size_t SIZEL, size_t Nfeatures, double fracRej, int MAJORITY_BUF , size_t ClusterSize, int SegmentLength, const char *pathHybridDir, int SavingFeatButt, int LoadingButt, int SavingButt, FILE *file2);

void computeNoiseClassifier(NoiseClassification *_ptr, float** FeaturesMatrix);
void destroyNoiseClassifier(NoiseClassification** _ptr);

#endif /* UnsupervisedClassification_h */
