//
//  UnsupervisedClassification.c
//
//
//  Created by Nasim Taghizadeh Alamdari on 11/1/17.
//  Copyright © 2017 UT Dallas. All rights reserved.
//

#include "UnsupervisedClassification.h"

//
#define DECIMATION_FACTOR 3
#define EPS 1.0e-7
#define S2F 3.051757812500000e-05f
#define F2S 32768

#define Moving_average  3 // for clustering features (OFC part)
//#define mBands          4
#define frequency       48000



NoiseClassification* initNoiseClassifier(int decisionBufferLength, float sigmaValue, size_t SIZEL, size_t Nfeatures, double fracRej, int MAJORITY_BUF , size_t ClusterSize,int SegmentLength, const char *pathHybridDir, int SavingFeatButton, int LoadingButton, int SavingButton, FILE *file2){
    NoiseClassification* inParam = (NoiseClassification*)malloc(sizeof(NoiseClassification));
    
    inParam->decisionBufferLength = (int)decisionBufferLength;
    inParam-> Mavg = Moving_average;
    inParam-> m             = floor(decisionBufferLength/inParam->Mavg);
    inParam-> n             = (int)Nfeatures;
    inParam->DetectedClusterLabel = 0;
    inParam->totalDetectedClusters = 0;
    inParam->FlagC = 0;

    //inParam->mFrames = floor(SegmentLength / inParam->overlap)-1; // must be equal to decisionBufferLength
    inParam->frame = (double **)malloc(inParam-> m * sizeof(double *));
    // define the second dimension
    for ( int i = 0; i < inParam-> m; i++)
        inParam->frame[i] = (double *)malloc(inParam->n * sizeof(double));
    
    //
    //smoothingBufferLength = (int)(decisionRate*frequency)/(stepsize*decisionBufferLength);
    //stepsize = (int) ((getSamplingRate() * parseFloat(frameSize.getText().toString()))/(2*1000));
    //__android_log_print(ANDROID_LOG_DEBUG,"Initialize","%d", smoothingBufferLength);
    //smoothingBuffer = calloc(sizeof(int), smoothingBufferLength);
    //
    
    inParam-> sizeC = 0; // modified by Nasim
    inParam-> sizeChunk = (int)SIZEL;
    inParam-> SIZEL = (int)SIZEL;
    inParam-> sizeLSim = (int)SIZEL - 1;
    
    inParam-> maxLimit = 5 * SIZEL;
    inParam-> sigma = sigmaValue;
    inParam-> fractionalRej = fracRej;
    inParam-> inChunk = -1;
    inParam-> chunkSim [ inParam-> sizeLSim] = 0;//modified by Nasim
    inParam-> fadLabelV[SIZEL] =  0 ;
    inParam-> majorityBuffer = MAJORITY_BUF;
    inParam-> medFiltBuff[2] = 0;
    inParam-> smoothedLabel[inParam-> majorityBuffer] = 1;
    inParam-> SavingOption = SavingButton; // this needs to be based on setting input (boolian value :0 or 1)
    inParam-> LoadingOption = LoadingButton;
    inParam-> savingFeats = SavingFeatButton;
    inParam-> ClusterSize = (int)ClusterSize; // maximunm number of cluster detection.
    inParam-> pathHybridDir = pathHybridDir;
    inParam-> file2 = file2;
    
    inParam->myChunk_pre = (Chunk*)malloc(sizeof(Chunk) * (size_t)SIZEL);
    memset(inParam->myChunk_pre, 0, sizeof(*inParam->myChunk_pre));
    
    inParam->ClusterSet_pre = (SVCluster*)malloc(sizeof(SVCluster)*ClusterSize);
    memset(inParam->ClusterSet_pre, 0, sizeof(*inParam->ClusterSet_pre));
    
    //******* Loading the saved previously runned clustering resutls *********
    if (inParam->LoadingOption == 1) {
        
        //FILE *file = fopen("/Users/nasim/Desktop/ClusterResultsRF.dat", "rb"); // binary read mode
        FILE *file = fopen(pathHybridDir, "rb"); // binary read mode
        
        if (file != NULL){
            fread(&inParam->sizeC,sizeof(inParam->sizeC),1,file);
            //printf("sizeC is %d\n",inParam->sizeC);
            fread(&(inParam->ClusterSize),sizeof((inParam->ClusterSize)),1,file);
            for (int ri=0; ri< inParam->sizeC; ri++){
                fread(&inParam->ClusterSet_pre[ri].c,sizeof(inParam->ClusterSet_pre[ri].c),1,file);
                fread(&inParam->ClusterSet_pre[ri].r,sizeof(inParam->ClusterSet_pre[ri].r),1,file);
                inParam->ClusterSet_pre[ri].alpha  = (double*)malloc(sizeof(double)* inParam->ClusterSet_pre[ri].r);
                inParam->ClusterSet_pre[ri].SVvectors  = (double**)malloc(sizeof(double*)* inParam->ClusterSet_pre[ri].r);
                for(int  rj = 0; rj< inParam->ClusterSet_pre[ri].r; rj++){
                    inParam->ClusterSet_pre[ri].SVvectors[rj] = (double*)malloc(sizeof(double)* inParam->ClusterSet_pre[ri].c);
                    for(int rk = 0; rk< inParam->ClusterSet_pre[ri].c; rk++){
                        fread(&inParam->ClusterSet_pre[ri].SVvectors[rj][rk],sizeof(inParam->ClusterSet_pre[ri].SVvectors[0][0]),1,file);
                    }
                    fread(&inParam->ClusterSet_pre[ri].alpha[rj],sizeof(inParam->ClusterSet_pre[ri].alpha[0]),1,file);
                }
                fread(&inParam->ClusterSet_pre[ri].Kxx,sizeof(inParam->ClusterSet_pre[ri].Kxx),1,file);
                //printf("kxx for cluster %d is %d\n",ri,inParam->ClusterSet_pre[ri].Kxx);
                fread(&inParam->ClusterSet_pre[ri].R2,sizeof(inParam->ClusterSet_pre[ri].R2),1,file);
                fread(&inParam->ClusterSet_pre[ri].sigma,sizeof(inParam->ClusterSet_pre[ri].sigma),1,file);
                fread(&inParam->ClusterSet_pre[ri].offsets,sizeof(inParam->ClusterSet_pre[ri].offsets),1,file);
            }
            
            fclose(file);
        }
        else{
            printf("\n Error! Can not open the file for reading \n");
        }
        
    }
    
    //************************************************************************
    
    
    inParam->previousCluster = (preClass*)malloc(sizeof(preClass)* 3); //This is for smothing resutls of clusters, averaging over last 3 feature frames
    memset(inParam->previousCluster, 0, sizeof(*inParam->previousCluster));
    inParam->previousCluster->cLabel = (int *)malloc(3 * sizeof(int));
    
    
    inParam->ClassOut[0] = 0;
    inParam->ClassOut[1] = 0;
    inParam->ClassOut[2] = 1;
    
    inParam->frameNumber = 0;
    inParam->count       = 0;
    
    
    return inParam;
}



void computeNoiseClassifier(NoiseClassification *_ptr, float** FeaturesMatrix){
    NoiseClassification *inParam = _ptr;
    Chunk* myChunk = (Chunk*) inParam->myChunk_pre;
    SVCluster* ClusterSet = (SVCluster*) inParam->ClusterSet_pre;


    //****Gatther all feature frames**************
        // Put all subband features in inParam->frame

        for (int j = 0; j < inParam->n; j++){


            for (int i = 0; i < inParam->m; i++){
                float MavgSum = 0;

                for (int k = 0; k< inParam->Mavg; k++){
                    int idx = (i * inParam->Mavg) + k;
                    MavgSum += (FeaturesMatrix[idx][j]);

                }

                double mean_MavgSum = (double)MavgSum / inParam->Mavg;
                inParam->frame[i][j] = mean_MavgSum; // (inParam->FeatureMatrix[i][j])
                //printf("%f \n", inParam->frame[i][j]);
            }
        }


        // *********************** Saving the features *************
        if (inParam->savingFeats == 1){
            if (inParam->file2 != NULL) {
                
                for (int i = 0; i < inParam->m; i++){
                    for (int j = 0; j < inParam->n; j++){
                        fprintf(inParam->file2, "%.2f,", inParam->frame[i][j] );
                        
                        //fwrite(inParam->frame[i], sizeof(inParam->frame[i]), 1, file2);
                    }
                    fprintf(inParam->file2,";\n");
                }
            }
            else{
                printf("\n Error! Can not open the file for writing \n");
            }
        }
        //***************************************************
        
        //*************************************************
        //Adding OFC parts here
        
        // initialize a control variale
        
        bool crNovCL = 0;
        
        // run the frame analsis function first
        double radius = frameAnalysis(inParam->frame, inParam->m, inParam->n);
        
        // calculate the min fading parameter
        double minFading = exp(-log2(inParam->maxLimit));
        
        // check the size of the SVCluster array
        if (inParam->sizeC != 0) {
            //if (ClusterSet->alpha != 0) {
            // needs conversion
            Classification(ClusterSet, inParam->frame, inParam->ClassOut, inParam->sizeC,inParam-> m,inParam-> n);
            
            /*
             ClassOut[0] is equivalent of CloseCL    in MATLAB
             ClassOut[1] is equivalent of ClassType    in MATLAB
             ClassOut[2] is equivalent of NovOp        in MATLAB
             */
            inParam->smoothedLabel[4] = inParam->ClassOut[0]; // modified by Nasim
            // take the left circular shift of smoothedLabel
            
            circularShift(inParam->smoothedLabel, 5, -1); // Nasim: the shift is from down to up (inverse)
            inParam->ClassOut[0] = mode(inParam->smoothedLabel, 5);
            
            
            if ((inParam->ClassOut[0] != 0) & (inParam->SIZEL <16) ){
                circularShift(inParam->previousCluster->cLabel, 3, +1); // shift to left, I[3]-> I[2]
                inParam->previousCluster->cLabel[2] = inParam->ClassOut[0];
            }
            
            
            
            //********* 3rd order 1-D median filter , added by Nasim *************
            if ((inParam->ClassOut[0] != 0) ){
                inParam->medFiltBuff[2] = inParam->ClassOut[0];
                int medVal = median(inParam->medFiltBuff, 3);
                
                int maxFilt = 0;
                int maxFilt_idx = 0;
                for (int i = 0; i < 3; i++){
                    if (inParam->medFiltBuff[i] > maxFilt){
                        maxFilt     = inParam->medFiltBuff[i];
                        maxFilt_idx = i;
                    }
                }
                inParam->medFiltBuff [maxFilt_idx] = medVal;
                inParam->ClassOut[0] = mode(inParam->medFiltBuff , 3);
                circularShift(inParam->medFiltBuff, 3, +1); // shift to left, I[3]-> I[2]
                inParam->medFiltBuff[2] = inParam->ClassOut[0];
                circularShift(inParam->medFiltBuff, 3, +1);
            }
            //*********************************************************************
            
            if (inParam->ClassOut[0] != 0)
                inParam->ClassOut[2] = 0;
        } // end of if (sizeC != 0)
        
        // second condition
        if ((inParam->sizeC == 0) && (inParam->sizeChunk == 0)) {
            inParam->ClassOut[0] = 0;
            inParam->ClassOut[1] = 0;
            inParam->ClassOut[2] = 1;
            inParam->inChunk = -1;
        }
        else if (inParam->sizeC == 0) {
            inParam->ClassOut[0] = 0;
            inParam->ClassOut[1] = 0;
            inParam->ClassOut[2] = 1;
        } // end of if ((sizeC == 0) & (sizeL == 0))
        
        if (inParam->sizeC < inParam->ClusterSize){
            if ((inParam->ClassOut[0] == 0) && (inParam->ClassOut[1] == 0) && (inParam->ClassOut[2] == 1)) {
                // increase the inChunk
                inParam->inChunk++;
                myChunk[inParam->inChunk].m = inParam->m; // added by Nasim
                myChunk[inParam->inChunk].n = inParam->n;  // added by Nasim
                myChunk[inParam->inChunk].data = (double **)malloc(sizeof(double*) * myChunk[inParam->inChunk].m);  // modified by Nasim
                
                for (int i = 0; i < inParam->m; i++) {
                    myChunk[inParam->inChunk].data[i] = (double *)malloc(inParam->n * sizeof(double));
                }
                for (size_t j = 0; j < inParam->m; j++) {
                    for (size_t k = 0; k < inParam->n; k++) {
                        myChunk[inParam->inChunk].data[j][k] = inParam->frame[j][k];
                    }
                }
                
                //myChunk[inChunk].data = frame; // wrong approach, it causes all chunk.data become same thing!
                myChunk[inParam->inChunk].sigma = radius;
                myChunk[inParam->inChunk].centroid = (double *)malloc(inParam->n * sizeof(double)); //modified by Nasim
                //printFrame(myChunk[inChunk].data, FRAME_SIZE, N);
                
                // mean of frame across dim 1;
                meanMatrixR(inParam->frame, myChunk[inParam->inChunk].centroid, inParam->m, inParam->n);
                
                // sizeL is equivalent of NomWinInChunk in MATLAB
                if ((inParam->inChunk > 0) && (inParam->inChunk <= (int)inParam->sizeChunk)) {
                    
                    // create two dynamic arrays
                    double *currentArray = (double *)malloc(inParam->n * sizeof(double)); //modified by Nasim
                    double *previusArray = (double *)malloc(inParam->n * sizeof(double)); //modified by Nasim
                    
                    
                    // mean of the current frame data and previous frame dat across dim 1;
                    meanMatrixR(myChunk[inParam->inChunk].data, currentArray,inParam-> m, inParam->n);
                    meanMatrixR(myChunk[inParam->inChunk - 1].data, previusArray, inParam->m, inParam->n);
                    
                    double centDiff = 0;
                    size_t i;
                    for (i = 0; i < inParam->n; i++) // modified by Nasim
                        centDiff += (currentArray[i] - previusArray[i]) * (currentArray[i] - previusArray[i]);
                    
                    centDiff = sqrt(centDiff);
                    
                    // free the allocated memory
                    free(currentArray);
                    free(previusArray);
                    currentArray = NULL;
                    previusArray = NULL;
                    
                    // extract the sigmas from the structure
                    double curerntSigma = myChunk[inParam->inChunk].sigma;
                    double previouSigma = myChunk[inParam->inChunk - 1].sigma;
                    
                    if (centDiff < (curerntSigma + previouSigma)) {
                        inParam->chunkSim[inParam->inChunk - 1] = 1;
                    }
                    else {
                        // run the connection check function
                        inParam->chunkSim[inParam->inChunk - 1] = connectionCheck(myChunk[inParam->inChunk].data,
                                                                                  myChunk[inParam->inChunk - 1].data,
                                                                                  myChunk[inParam->inChunk].m,            // # of rows - frame 1
                                                                                  myChunk[inParam->inChunk - 1].m,        // # of rows - frame 2
                                                                                  myChunk[inParam->inChunk].n,            // # of columns - same for both frames
                                                                                  curerntSigma, previouSigma);
                    } // end of if (centDiff < (curerntSigma + previouSigma))
                } // end of if ((inChunk > 0) & (inChunk <= sizeL
                
            } // end of if ((ClassOut[0] == 0) & (ClassOut[1] == 0) & (ClassOut[2] == 1))
            
            
            // update the fadLabelV array
            for (int i = 0; i < inParam->inChunk; i++) {
                inParam->fadLabelV[i]++;
            }
            if (inParam->inChunk == inParam->sizeChunk - 1) {
                // allocate memory for fadValues
                size_t sizeL = inParam->sizeChunk;
                double* fadValues = (double*)malloc( (inParam->SIZEL) * sizeof(double));
                for (size_t i = 0; i < sizeL; i++) {
                    fadValues[i] = exp(-log2(inParam->fadLabelV[i] + 1)); //modified by Nasim
                    //out_data << fadValues[i] << ',';
                }
                
                //out_data << '\n';
                // find the elements of fadValues which are smaller than minfading
                findIdx* cIndex = find(fadValues, minFading, sizeL);
                //int kk = cIndex->length;
                int cnt = 0;
                if (cIndex->length > 0) {
                    for (int ii = 0; ii < cIndex->length; ii++) {
                        
                        myChunk[cIndex->Index[ii]].data = NULL; // Nasim: wrong
                        myChunk[cIndex->Index[ii]].centroid = NULL; //added by
                        myChunk[cIndex->Index[ii]].m = 0; //added by Nasim
                        //--------------------Added by Nasim ---------------//
                        if (cIndex->Index[ii] != inParam->inChunk) { //moving memory location
                            for (int i = cIndex->Index[ii]; i < inParam->inChunk; i++)
                            {
                                myChunk[i].data = myChunk[i + 1].data;
                                myChunk[i].centroid = myChunk[i + 1].centroid;
                                myChunk[i].m = myChunk[i + 1].m;
                                inParam->chunkSim[i] = inParam->chunkSim[i + 1];
                            }
                            
                            inParam->chunkSim[inParam->inChunk - cnt] = 0;
                            myChunk[inParam->inChunk - cnt].data = NULL; // Nasim: wrong
                            myChunk[inParam->inChunk - cnt].centroid = NULL; //added by
                            myChunk[inParam->inChunk - cnt].m = 0; //added by Nasim
                            cnt++;
                            
                        }
                        else {
                            inParam->chunkSim[inParam->inChunk - cnt] = 0; //which cnt stays 0;
                        }
                        //---------------------------------------------------
                        inParam->fadLabelV[cIndex->Index[ii]] = 0;
                        fadValues[cIndex->Index[ii]] = 0;
                        
                    }
                }//end if (cIndex->length>0)
                //out_data.close();
                
                //TODO fix this
                inParam->inChunk = inParam->inChunk - cIndex->length; // modified by Nasim-July22
                
                if (cIndex->length > 0) {
                    // circular shift fadvalues and fadLabelV by inChunk
                    circularShift_d(fadValues, inParam->SIZEL, -(inParam->inChunk + 1)); // modified by Nasim
                    circularShift_d(inParam->fadLabelV, inParam->SIZEL, -(inParam->inChunk + 1)); //modified by Nasim
                    
                }
                
                //free memory
                free(cIndex->Index); // added by Nasim
                cIndex->Index = NULL; // added by Nasim
                
                // free the allocated memory
                free(fadValues);
                fadValues = NULL;
                
                if (inParam->inChunk >= floor(INCHUNK_FLOOR_CONST * inParam->sizeChunk)) { // modified by Nasim
                    
                    
                    // run the massEvaluation function
                    SVData* mySVData;
                    mySVData = massEvaluation(myChunk, inParam->chunkSim, inParam->sizeChunk, inParam->inChunk);
                    
                    // extract the packed data
                    inParam->inChunk = mySVData->inChunk;
                    crNovCL = mySVData->crNovCL;
                    // the following may not be necessary
                    
                    int svRow = mySVData->svRow;
                    int svCol = mySVData->svCol;
                    //out_data << '\n' << '\n';
                    double** svData = (double **)malloc(svRow * sizeof(double*));  // modified by Nasim
                    for (int i = 0; i < svRow; i++) {
                        svData[i] = (double *)malloc(svCol * sizeof(double));
                    }
                    for (int j = 0; j < svRow; j++) {
                        for (int k = 0; k < svCol; k++) {
                            svData[j][k] = mySVData->data[j][k];
                        }
                        
                    }
                    
                    
                    if (inParam->inChunk == -1) { // modified by Nasim
                        // update the fadLabelV array
                        size_t i;
                        for (i = 0; i < sizeL; i++)
                            inParam->fadLabelV[i] = 0;
                    }
                    else {
                        // update the fadLabelV array
                        size_t i;
                        for (i = inParam->inChunk + 1; i < sizeL; i++)
                            inParam->fadLabelV[i] = 0;
                        
                    }
                    
                    if (crNovCL) {
                        
                        // this is equivalent of supVectCalculation() function in MATLAB
                        SVCluster* mySVclust = ClusterCreation(svData, svRow, svCol, inParam->fractionalRej, inParam->sigma);
                        
                        
                        
                        // pack the data in clusterSet
                        ClusterSet[inParam->sizeC].sigma = inParam-> sigma;
                        ClusterSet[inParam->sizeC].r = mySVclust->r; //modified by Nasim
                        ClusterSet[inParam->sizeC].c = mySVclust->c; //modified by Nasim
                        ClusterSet[inParam->sizeC].alpha = mySVclust->alpha;
                        ClusterSet[inParam->sizeC].offsets = mySVclust->offsets;
                        ClusterSet[inParam->sizeC].Kxx = mySVclust->Kxx;
                        ClusterSet[inParam->sizeC].R2 = mySVclust->R2;
                        ClusterSet[inParam->sizeC].SVvectors = mySVclust->SVvectors;
                        
                        inParam->sizeC++; //added by Nasim
                        
                        
                        //*********************** Saving the clustering resutls ***************************
                        if (inParam->SavingOption == 1) {
                            
                            
                            //FILE *file = fopen("/Users/nasim/Desktop/ClusterResultsRF.dat", "wb");
                            FILE *file = fopen(inParam->pathHybridDir, "wb");
                            //A DAT file is a generic data file created by a specific application.  DAT files are typically accessed only by the application that created them are not meant to be opened manually by the user.
                            
                            if (file != NULL) {
                                fwrite(&inParam->sizeC,sizeof(inParam->sizeC),1,file);
                                fwrite(&(inParam->ClusterSize),sizeof((inParam->ClusterSize)),1,file);
                                //int size = sizeof(ClusterSet)* inParam->ClusterSize;
                                for (int wi=0; wi< inParam->sizeC; wi++){
                                    fwrite(&ClusterSet[wi].c,sizeof(ClusterSet[wi].c),1,file);
                                    fwrite(&ClusterSet[wi].r,sizeof(ClusterSet[wi].r),1,file);
                                    for(int  wj = 0; wj< ClusterSet[wi].r; wj++){
                                        for(int wk = 0; wk<ClusterSet[wi].c; wk++){
                                            fwrite(&ClusterSet[wi].SVvectors[wj][wk],sizeof(ClusterSet[wi].SVvectors[0][0]),1,file);
                                        }
                                        fwrite(&ClusterSet[wi].alpha[wj],sizeof(ClusterSet[wi].alpha[0]),1,file);
                                    }
                                    fwrite(&ClusterSet[wi].Kxx,sizeof(ClusterSet[wi].Kxx),1,file);
                                    fwrite(&ClusterSet[wi].R2,sizeof(ClusterSet[wi].R2),1,file);
                                    fwrite(&ClusterSet[wi].sigma,sizeof(ClusterSet[wi].sigma),1,file);
                                    fwrite(&ClusterSet[wi].offsets,sizeof(ClusterSet[wi].offsets),1,file);
                                }
                                fclose(file);
                            }
                            else{
                                printf("\n Error! Can not open the file for writing \n");
                            }
                        }
                        //*********************************************************************************
                        
                        // set the chunkSim array to zero and myChunk to NULL
                        size_t i;
                        for (i = 0; i < sizeL - 1; i++) {
                            inParam-> chunkSim[i] = 0; //modified by Nasim
                            myChunk[i].data = NULL;
                            myChunk[i].centroid = NULL; //added by Nasi
                        }
                        
                        
                    } // end of if (crNovCL != 0)
                } // end of if (inChunk >= floor(0.5 * sizeL))
            } // end of if (inChunk == sizeL)
        } //end of if (inParam->sizeC < inParam->ClusterSize)
        
        inParam->myChunk_pre =  myChunk;
        inParam->ClusterSet_pre = ClusterSet;
        
        
        
        // free the dynamically allocated memory
        //for (int i = 0; i < inParam->decisionBufferLength; i++) free(inParam->frame[i]);
        //free(inParam-> frame);
        //inParam-> frame = NULL;
        
        
        
        // add functionality here to save/export ClusterSet or other
        inParam->DetectedClusterLabel = inParam->ClassOut[0];
        //inParam->totalDetectedClusters = sizeof(ClusterSet[inParam->sizeC]) / sizeof(ClusterSet[0]);
        inParam->FlagC = inParam->ClassOut[2];
        //(sizeof(ClusterSet[SIZEC]) -sizeof(ClusterSet[SIZEC - 1])) / sizeof(ClusterSet[0]);
        inParam->frameNumber++;
        
    
        
        // *************************************************
    //} // end warmUp condition
    // **************************************


    if ((inParam->ClassOut[0] < 0) | (inParam->ClassOut[0] >inParam->SIZEL+1) ){
        inParam->DetectedClusterLabel = 0;
    }
    else if ( (inParam->SIZEL <16) &  (inParam->FlagC == 1 | inParam->ClassOut[0] == 0 ) ){
        inParam->DetectedClusterLabel = inParam->previousCluster->cLabel[2];
    }
    else {
        inParam->DetectedClusterLabel =  inParam->DetectedClusterLabel;
    }
    
    inParam->totalDetectedClusters = inParam->sizeC;
    inParam->frameNumber = inParam->frameNumber;
}


void destroyNoiseClassifier(NoiseClassification** _ptr){
    if (*_ptr != NULL) {
        
        if ((*_ptr)->frame != NULL){
            free((*_ptr)->frame);
            (*_ptr)->frame = NULL;
        }
        if ((*_ptr)->FeatureMatrix != NULL){
            free((*_ptr)->FeatureMatrix);
            (*_ptr)->FeatureMatrix = NULL;
        }
        if ((*_ptr)->file2 != NULL){
            free((*_ptr)->file2);
            (*_ptr)->file2 = NULL;
        }

        if ((*_ptr)->myChunk_pre != NULL){
            free((*_ptr)->myChunk_pre);
            (*_ptr)->myChunk_pre = NULL;
        }
        if ((*_ptr)->ClusterSet_pre != NULL){
            free((*_ptr)->ClusterSet_pre);
            (*_ptr)->ClusterSet_pre = NULL;
        }
  
        
        free(*_ptr);
        *_ptr = NULL;
    }
}
