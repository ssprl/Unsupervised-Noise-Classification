//By Fatemeh Saki, University of Texas at Dallas, Spring 2017
// Modified by Nasim Alamdari, 2017
#include "stdafx.h"

#include <iostream>
#include <fstream>
using namespace std;

// Macros for this module
#define CHUNK_SIZE_THRESHOLD	0.5
#define SPORADIC_THRESH			2

// define the mass struct
typedef struct{
	double** centroid;
	double* radius;
} mass;


// ------------------------------------------------------------------------------
// Declare the mass Evaluation function
SVData* massEvaluation(Chunk* myChunk, int* chunkSim, const int sizeL, const int inChunk){ //modified by Nasim

	// initialize mySVData by allocating memory dynamically
	SVData* mySVData = (SVData *)malloc(sizeof(SVData));

	// initialize the structure to zero
	memset(mySVData, 0, sizeof(*mySVData));

	// find the indecies of chunksim enteries which are false
	//findIdx* disIdx = find(chunkSim, false, sizeL-1); //modified by Nasim
	findIdx* disIdx = find2(chunkSim, -1, 0, inChunk); //modified by Nasim-July22
	
	ofstream out_data;
	out_data.open("OFC Results.txt", ios::trunc);
	
	// allocating memory for the Dist results
	int **MassIdx = (int **)malloc((disIdx->length -1) * sizeof(int *));
	for (int i = 0; i < (disIdx->length -1); i++)
		MassIdx[i] = (int *)malloc(3 * sizeof(int));
	
	if (disIdx->length > 2){
		int alert = 1;
		out_data << disIdx->Index[0] << ',' << disIdx->Index[1] << ',' << disIdx->Index[2] << ',';
		out_data << disIdx->Index[3] << ',' << disIdx->Index[4] << ',';
	}

	out_data << '\n';

	// initialize a length counter
	int LmIdx = 0;
	for (int iDis = 0; iDis < (disIdx->length  - 1); iDis++){
		// don't add the elements with length smaller than or equal to sporadic threshold
		if ((disIdx->Index[iDis + 1] - disIdx->Index[iDis]) > SPORADIC_THRESH){
			MassIdx[LmIdx][0] = disIdx->Index[iDis]; //modified by Nasim
			MassIdx[LmIdx][1] = disIdx->Index[iDis + 1] - 1; //modified by Nasim
			MassIdx[LmIdx][2] = disIdx->Index[iDis + 1] - disIdx->Index[iDis]; //modified by Nasim
			out_data << MassIdx[LmIdx][0] << ',' << MassIdx[LmIdx][1] << ',' << MassIdx[LmIdx][2] << '\n';
			LmIdx++;
			
		}
	}
	out_data.close();
	free(disIdx->Index); // Modified by Nasim
	disIdx->Index = NULL; // Modified by Nasim
	
	// reallocating memory to adjuct the matrix length
	MassIdx = (int **)realloc(MassIdx, LmIdx * sizeof(int *));

	if (LmIdx > 0){
		// allocate memory for the array of mass structures
		mass* myMass = (mass *)malloc(LmIdx * sizeof(mass));

		for (int iMass = 0; iMass < LmIdx; iMass++){
			// memory allocation for radius and cent 
			myMass[iMass].radius = (double *)malloc((MassIdx[iMass][1] - MassIdx[iMass][0] + 1) * sizeof(double)); //modified by Nasim-Jul20
			myMass[iMass].centroid = (double **)malloc((MassIdx[iMass][1] - MassIdx[iMass][0] + 1) * sizeof(double*)); //modified by Nasim-Jul20
			for (int i = 0; i < (MassIdx[iMass][1] - MassIdx[iMass][0] + 1); i++) //modified by Nasim-Jul20
				myMass[iMass].centroid[i] = (double *)malloc(myChunk[0].n * sizeof(double));

			for (int imicro = MassIdx[iMass][0]; imicro <= MassIdx[iMass][1]; imicro++){
				int cnt = imicro - MassIdx[iMass][0];
				for (int i = 0; i < myChunk[0].n; i++){
					myMass[iMass].centroid[cnt][i] = myChunk[imicro ].centroid[i];
				}
				myMass[iMass].radius[cnt] = myChunk[imicro].sigma;
			}
		}//end of for loof for iMass

		// allocating memory for CloseMasses
		bool **CloseMasses = (bool **)malloc(LmIdx * sizeof(bool *));
		for (int j = 0; j < LmIdx; j++)
			CloseMasses[j] = (bool *)malloc(LmIdx * sizeof(bool));

		// initialize a flag
		bool ConnectedMass = false;

		for (int nMass = 0; nMass < LmIdx; nMass++){
			for (int jMass = 0; jMass < LmIdx; jMass++){

				// allocating memory for the result pauerwise euclidian distance 
				double **DistMtx = (double **)malloc( (MassIdx[nMass][1] - MassIdx[nMass][0] + 1) * sizeof(double *)); // modified by Nasim-Jul20
				for (int j = 0; j < (MassIdx[nMass][1] - MassIdx[nMass][0] + 1); j++) //modified by Nasim-Jul20
					DistMtx[j] = (double *)malloc((MassIdx[jMass][1] - MassIdx[jMass][0] + 1) * sizeof(double)); //modified by Nasim-Jul20

				// calculate the distance 
				MyDistm(myMass[nMass].centroid, myMass[jMass].centroid, DistMtx, (MassIdx[nMass][1] - MassIdx[nMass][0] + 1), (MassIdx[jMass][1] - MassIdx[jMass][0] + 1), myChunk[0].n); //modified by Nasim-Jul20
				//out_data << DistMtx << '\n';

				// find the required elements of DistMtx
				for (int i = 0; i < (MassIdx[nMass][1] - MassIdx[nMass][0] + 1); i++){ //Nasim-Jul20
					for (int j = 0; j < (MassIdx[jMass][1] - MassIdx[jMass][0] + 1); j++){ //Nasim-Jul20
						if (DistMtx[i][j] < myMass[nMass].radius[i] + myMass[jMass].radius[j]){
							CloseMasses[nMass][jMass] = true;
							if (nMass != jMass)
								ConnectedMass = true;
						}
						else
							CloseMasses[nMass][jMass] = false;
					}
				}

				// free the memory for DistMtx
				for (int j = 0; j < MassIdx[nMass][2]; j++) free(DistMtx[j]);
				free(DistMtx);
			}
		}

		// if no connections between masses exist
		if (!ConnectedMass){

			//for (int i = 0; i < MassIdx[LmIdx - 1][2]; i++){
			//	//TODO: free memory for data and centroid when setting them to NULL
			//	// here and everywhere else
			//	myChunk[i].centroid = NULL;
			//	myChunk[i].data = NULL;
			//}

			//TODO: claculate inChunk dynamically; the following might fail for different conditions
			//int newChunkSize = sizeL - MassIdx[LmIdx - 1][2] - 1;


			//------------------Added by Nasim --------------------------
			int removeIdx = 0;
			int idxx = 0;
			if (MassIdx[LmIdx - 1][0] > 0){ //added by Nasim
				for (int j = 0; j < MassIdx[LmIdx - 1][0]; j++) {
					myChunk[j].centroid = NULL;
					myChunk[j].data = NULL;
					if (j != inChunk) {
						chunkSim[j] = 0;
					}
					//--------------------Added by Nasim ---------------//
					if (j != inChunk) { //moving memory location
						for (int i = j; i < inChunk; i++)
						{
							myChunk[i].data = myChunk[i + 1].data;
							myChunk[i].centroid = myChunk[i + 1].centroid;
							myChunk[i].m = myChunk[i + 1].m;
							chunkSim[i] = chunkSim[i + 1];
						}
						
						chunkSim[inChunk - idxx] = 0;
						myChunk[inChunk - idxx].data = NULL; // Nasim: wrong 
						myChunk[inChunk - idxx].centroid = NULL; //added by 
						myChunk[inChunk - idxx].m = 0; //added by Nasim
						idxx++;

					}
					else {
						chunkSim[inChunk - idxx-1] = 0; //which cnt stays 0;
						myChunk[inChunk - idxx].data = NULL; // Nasim: wrong 
						myChunk[inChunk - idxx].centroid = NULL; //added by 
					}
					//---------------------------------------------------

					removeIdx++;
				}
			}


			int idxx2 = 0;
			if (MassIdx[LmIdx - 1][2]+1 < inChunk){ //added by Nasim
				for (int j = MassIdx[LmIdx - 1][2] + 1; j <= inChunk; j++){

					myChunk[j].centroid = NULL;
					myChunk[j].data = NULL;
					if (j != inChunk) {
						chunkSim[j] = 0;
					}
					//--------------------Added by Nasim ---------------//
					if (j != inChunk) { //moving memory location
						for (int i = j; i < inChunk; i++)
						{
							myChunk[i].data = myChunk[i + 1].data;
							myChunk[i].centroid = myChunk[i + 1].centroid;
							myChunk[i].m = myChunk[i + 1].m;
							chunkSim[i] = chunkSim[i + 1];
						}
						
						chunkSim[inChunk - idxx2] = 0;
						myChunk[inChunk - idxx2].data = NULL; // Nasim: wrong 
						myChunk[inChunk - idxx2].centroid = NULL; //added by 
						myChunk[inChunk - idxx2].m = 0; //added by Nasim
						idxx2++;

					}
					else {
						chunkSim[inChunk - idxx-1] = 0; //which cnt stays 0;
						myChunk[inChunk - idxx].data = NULL; // Nasim: wrong 
						myChunk[inChunk - idxx].centroid = NULL; //added by 
					}
					//---------------------------------------------------
					removeIdx++;
				}
			}

			int newChunkSize = (inChunk+1) - removeIdx; // Modified by Nasim
			//---------------------------------------------------------

			mySVData->inChunk = newChunkSize-1; // modified by Nasim

			// find the right size for data matrix
			int svRow = myChunk->m * newChunkSize;
			mySVData->svRow = svRow; // Modified by Nasim
			mySVData->svCol = myChunk->n; // Modified by Nasim

			// allocate momory for data matrix
			mySVData->data = (double **)malloc(svRow * sizeof(double *));
			for (int i = 0; i < svRow; i++)
				mySVData->data[i] = (double *)malloc(myChunk->n * sizeof(double));

			for (int i = 0; i < sizeL-1; i++){ //modified by Nasim
				if (myChunk[i].centroid != NULL){
					chunkSim[i] = -1;//modified by Nasim
				}
			}

			if (mySVData->inChunk >= CHUNK_SIZE_THRESHOLD * sizeL){

				mySVData->inChunk = -1; //modified by Nasim
				mySVData->crNovCL = 1;

				// initialize a counter
				int SVcnt = 0;
				for (int iChunk = 0; iChunk < newChunkSize; iChunk++){ //modified by Nasim
					if (myChunk[iChunk].centroid != NULL){

						for (int i = 0; i < myChunk->m; i++) {
							for (int j = 0; j < myChunk->n; j++) {
								mySVData->data[i + SVcnt * myChunk->m][j] = myChunk[iChunk].data[i][j];
								//out_data << mySVData->data[i + SVcnt * myChunk->m][j] << ',';
							}
							//out_data << '\n';
						}
						// increase the counter
						SVcnt++;
					} // end of if (myChunk[iChunk].centroid != NULL)

				} // end of for (int iChunk = 0; iChunk < sizeL; iChunk++)
				//out_data.close();
			}
			else{
				mySVData->crNovCL = 0;

				for (int i = 0; i < sizeL-1; i++) {//TODO: Nasim might need modification, modified by Nasim
					if (myChunk[i].centroid != NULL){
						chunkSim[i] = +1; //modified by Nasim
					}
			}

			} // end of if (mySVData->inChunk >= CHUNK_SIZE_THRESHOLD * sizeL)

			//for (int i = 0; i< sizeL; i++) { // Modified by Nasim
				//TODO: free memory for data and centroid when setting them to NULL
				// here and everywhere else
			//	myChunk[i].centroid = NULL;
			//	myChunk[i].data = NULL;
			//}

		}
		else{  // connection exists, check the connected mass and find the bigest one

			// allocated memory for array ConnectedMassIdx
			int* ConnectedMassIdx = (int *)malloc(LmIdx * LmIdx * sizeof(int));

			// find columns CloseMasses which are true
			int cnt = 0;										// keeps track of the conncted masses lables 
			for (int nMass = 0; nMass < LmIdx; nMass++){
				for (int jMass = 0; jMass < LmIdx; jMass++){
					if (CloseMasses[nMass][jMass] == true){
						ConnectedMassIdx[cnt] = jMass;			// has all the mass's indexes which are connected 
						cnt++;
					}
				}
			}

			int* uniqueArray = (int *)malloc(cnt * sizeof(int));
			int uniqueCount = 0;

			Unique(ConnectedMassIdx, uniqueArray, cnt, &uniqueCount); // uniqueCount+1 has all the valid points we need 

			// free the allocated memory
			free(ConnectedMassIdx);

			// calculating how many frames we will have to alloacte memory
			int numFramesInMicros = 0;

			for (int iM = 0; iM <= uniqueCount; iM++)
				numFramesInMicros += MassIdx[uniqueArray[iM]][2];

			// allocate memory for the frames to be kept 
			int* MergIdx = (int*)malloc(numFramesInMicros * sizeof(int));
			for (int iM = 0; iM <= uniqueCount; iM++){
				for (int j = 0; j < MassIdx[uniqueArray[iM]][2]; j++)
					MergIdx[iM] = (MassIdx[uniqueArray[iM]][0] + j); // put the indexesnof each microcluster, MassIdx[uniqueArray[iM]][0] has the the index of the first frame,   

				// TODO:Put the indexes 

			}

			// free the allocated memory 
			free(uniqueArray);



			if (numFramesInMicros >= CHUNK_SIZE_THRESHOLD * sizeL){

				mySVData->inChunk = -1; // modified by Nasim
				mySVData->crNovCL = 1;

				Chunk NovelClassData = myChunk[numFramesInMicros];

				// initialize a counter
				int SVcnt = 0;
				//===========================================================
				// for (int iChunk = 0; iChunk < length(MergeIdx); iChunk++)
				//mySVData->data[i + SVcnt * myChunk->m][j] = myChunk[MergIdx[iChunk]].data[i][j];
				for (int iChunk = 0; iChunk < numFramesInMicros; iChunk++){
					if (myChunk[iChunk].centroid != NULL){

						for (int i = 0; i < myChunk->m; i++)
							for (int j = 0; j < myChunk->n; j++)
								mySVData->data[i + SVcnt * myChunk->m][j] = myChunk[MergIdx[iChunk]].data[i][j];

						// increase the counter
						SVcnt++;
					} // end of if (myChunk[iChunk].centroid != NULL)
				} // end of for (int iChunk = 0; iChunk < sizeL; iChunk++)
				//=============================================================
				free(MergIdx); // free it : TO DO check it 
			}

			else{
				//TODO:
				for (int i = 0; i < sizeL; i++){

					myChunk[i].centroid = NULL;
					myChunk[i].data = NULL;

				}

				mySVData->inChunk = -1; // modified by Nasim
				mySVData->crNovCL = 0;
				mySVData->data = NULL;
				for (int i = 0; i < sizeL - 1; i++)
					chunkSim[i] = -1; // modified by Nasim



				for (int i = 0; i < sizeL - 1; i++)
					chunkSim[i] = -1; //modifed by Nasim



			}

		} // end of "else" when connection exists.


		//free the allocated memory for CloseMasses
		for (int i = 0; i < LmIdx; i++) free(CloseMasses[i]);
		free(CloseMasses);
		CloseMasses = NULL;

		// free the allocated memory for mass array
		for (int iMass = 0; iMass < LmIdx; iMass++){
			free(myMass[iMass].radius);
			myMass->radius = NULL;

			//free the allocated memory
			for (int i = 0; i < MassIdx[iMass][2]; i++) free(myMass[iMass].centroid[i]);
			free(myMass[iMass].centroid);
			myMass->centroid = NULL;
		}
		free(myMass);
		myMass = NULL;

	} // end of if (LmIdx > 0)
	else{
		// pack the data in the structure
		mySVData->inChunk = -1; // modified by Nasim
		mySVData->crNovCL = 0;
		mySVData->data	  = NULL;
		for (int i = 0; i < sizeL-1; i++)
			chunkSim[i] = -1; //modified by Nasim
	}

	//free the allocated memory
	for (int i = 0; i < LmIdx; i++) free(MassIdx[i]);
	free(MassIdx);
	
	MassIdx = NULL;

	return mySVData;
}
