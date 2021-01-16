//By Fatemeh Saki, University of Texas at Dallas, Spring 2017
// Modified by Nasim Alamdari, 2017
#include "stdafx.h"



// Define utility functions here

//--------------------------------------------------------------------------------------------
// elementwise exponential of a matrix 
void myExp(double** DistMtx, const double* alpha, const size_t r, const size_t n, const double sigma)
{
	size_t i, j;

	for (i = 0; i < r; i++){
		for (j = 0; j < n; j++){
			DistMtx[i][j] = exp(-DistMtx[i][j] * DistMtx[i][j] / (sigma * sigma)) * alpha[i]; //modified by Nasim-Jul27

		}
	}
	return;
}

//--------------------------------------------------------------------------------------------
// elementwise exponential of a matrix no alpha 
double myExp_alpha(double** DistMtx, const double* alpha, const size_t n, const double sigma)
{
	size_t i, j;
	double Kxx = 0;

	for (i = 0; i < n; i++){
		for (j = 0; j < n; j++){
			Kxx += alpha[i] * exp(-DistMtx[i][j] / (sigma * sigma)) * alpha[i]; //modified by Nasim
		}
	}
	return Kxx;
}

//--------------------------------------------------------------------------------------------
// vector to scalar comparison
int compareArrays(const double* a, const double b, const size_t n) {

	size_t i;

	for (i = 0; i < n; i++)
		if (a[i] < b) return 0;

	return 1;
}

//--------------------------------------------------------------------------------------------
// matrix min across rows // modified by Nasim
void minMatrixR(double** M, int* I, const size_t r, const size_t c) {

	size_t i, j;

	for (i = 0; i < c; i++){
		double min = M[0][i];
		I[i] = 0;
		for (j = 0; j < r; j++) 
			if (M[j][i] <= min){
				min = M[j][i];
				I[i] = j;
			}
	}
	return;
}

//--------------------------------------------------------------------------------------------
// matrix mean across rows// modified by Nasim
void meanMatrixR(double** M, double* I, const size_t m, const size_t n) {
	
	for (size_t i = 0; i < n; i++){ //modified by Nasim
		double sum = 0;
		for (size_t j = 0; j < m; j++) // modifed by Nasim
				sum += M[j][i];
		
		I[i] = sum / m; // modified by Nasim
	}
	return;
}

//--------------------------------------------------------------------------------------------
// statistical mode of an array a of length n
int mode(const int a[], const size_t n) {

	int maxValue = 0, maxCount = 0;
	size_t i, j;

	for (i = 0; i < n; ++i) {
		int count = 0;

		for (j = 0; j < n; ++j) {
			if (a[j] == a[i])
				++count;
		}

		if (count > maxCount) {
			maxCount = count;
			maxValue = a[i];
		}
	}

	return maxValue;
}

//--------------------------------------------------------------------------------------------
// mean of a matrix
double meanMatrix(double** M, const size_t m, const size_t n){

	double sum = 0;
	size_t i, j;

	for (i = 0; i < m; i++){
		for (j = 0; j < n; j++)
			sum += M[i][j];
			
	}
	return sum/(n*m);
}

//--------------------------------------------------------------------------------------------
// matrix max across all entries
double maxMatrix(double** M, const size_t m, const size_t n){

	double max = 0;
	size_t i, j;

	for (i = 0; i < m; i++){
		for (j = 0; j < n; j++){
			if (M[i][j] > max){
				max = M[i][j];
			}
		}
	}
	return max;
}

//--------------------------------------------------------------------------------------------
// conditional sum of matrix entries // modified by Nasim
double sumMatrixCond(double** M, const size_t m, const size_t n, const double target){

	double sum1 = 0; //modified by Nasim
	double sum2 = 0; //added by Nasim
	size_t i, j;

	for (i = 0; i < n; i++){
		for (j = 0; j < m; j++){
			if (M[j][i] < target)
				//sum += M[i][j];
				sum1 += 1; // //added by Nasim
		}
		sum2 += sum1; //added by Nasim
	}
	return sum2;
}

//--------------------------------------------------------------------------------------------
// circularshift of an array. left shift: amount > 0, right shift: amount < 0
void circularShift(int* I, const size_t n, const int amount){ // checked by Nasim
	
	size_t i;

	// create a duplicate of I
	int *temp = (int *) malloc(n * sizeof(int));
	for (i = 0; i < n; i++)
		temp[i] = I[i];

	// now change the indexing based on the circular shift amount
	
	for (i = 0; i < n; i++){ 
		int shift_i = (i + amount + n) % n; // modified by Nasim
		I[i] = temp[shift_i];
	}

	// free the temp array and set the pointer to NULL
	free(temp);
	temp = NULL;
	return;
}

//--------------------------------------------------------------------------------------------
// circularshift of a double array. Overloading is not allowed in c, that's why we have two 
// versions of this function: one for integer arrays and one for double arrays. 
// left shift: amount > 0, right shift: amount < 0
void circularShift_d(double* I, const size_t n, const int amount){ //checked by Nasim

	size_t i;

	// create a duplicate of I
	double *temp = (double *)malloc(n * sizeof(double));
	for (i = 0; i < n; i++)
		temp[i] = I[i];

	// now change the indexing based on the circular shift amount
	for (i = 0; i < n; i++){
		int shift_i = (i + amount + n) % n;
		I[i] = temp[shift_i];
	}

	// free the temp array and set the pointer to NULL
	free(temp);
	temp = NULL;
	return;
}

//--------------------------------------------------------------------------------------------
// find indices smaller than a scalar
findIdx* find(const double* a, const double b, const size_t n) {

	// allocated memory for the output data
	findIdx* fIndex = (findIdx*)malloc(sizeof(findIdx));

	//allocate memory for the Index vector
	int* I = (int*)malloc(n*sizeof(int));
	// initialize a length counter
	int m = 0;
	size_t i;

	for (i = 0; i < n; i++)
		if (a[i] < b){
			I[m] = i;
			m++;
		}
	// reallocate memory by cutting the last part of I
	I = (int*)realloc(I, m*sizeof(int));

	int k = 0;
	for (i = 0; i < n; i++)
		if (a[i] < b) {
			I[k] = i;
			k++;
		}


	fIndex->Index  = I;
	fIndex->length = k;

	// free the allocated memory
	//free(I); // Modified by Nasim
	//I = NULL; // Modified by Nasim

	return fIndex;
}



//------------------------Added by Nasim -----------------------------------------
// find indices smaller than a scalar : specifically for chunkSim
findIdx* find2(const int* a, const int b, const int c, const int n) {

	// allocated memory for the output data
	findIdx* fIndex = (findIdx*)malloc(sizeof(findIdx));

	//allocate memory for the Index vector
	int* I = (int*)malloc((n + 2) * sizeof(int));


	int* I2 = (int*)malloc(n*sizeof(int));
	// initialize a length counter
	int m = 0;
	int p = 0;
	size_t i;
	size_t j;
	size_t z;

	for (j = 0; j < n; j++){
		if (a[j] == c){ // Nasim: find index that chunksim == 0 (equal to NULL)
			I2[p] = j;
			p++;
		}
	}
	
	I[0] = 0;
		for (i = 0; i < n; i++){
			if (a[i] == b){
				I[m] = i;
				//------------------------------------
				for (z = 0; z < p; z++){
					if (i > I2[z]){ // Nasim: find index that chunksim == 0 (equal to NULL)
						I[m] = i - p;
					}
				}
				//------------------------------------
				m++;
			}
		}
		// reallocate memory by cutting the last part of I
		I = (int*)realloc(I, (m + 2) * sizeof(int));

		int k = 0;
		for (i = 0; i < n; i++){ //added by Nasim
			if (a[i] == b){
				I[k+1] = i;
				//------------------------------------
				for (z = 0; z < p; z++){
					if (i > I2[z]){ // Nasim: find index that chunksim == 0 (equal to NULL)
						I[k+1] = i - p;
					}
				}
				//------------------------------------
				k++;
			}
		}

		// set the last element of the array
		//I[m + 1] = ((n - 1) - p) + 1; // modified by Nasim
		I[k + 1] = n+1; // modified by Nasim

		fIndex->Index = I;
		fIndex->length = m+2; // modified by Nasim

	// free the allocated memory
	//free(I); // Modified by Nasim
	//I = NULL; // Modified by Nasim

	return fIndex;
}


//--------------------------------------------------------------------------------------------
// find indices of entries which are equal to true/flase. Allocate extra memory for 
findIdx* find(const bool* a, const bool b, const size_t n) {
	// allocated memory for the output data
	findIdx* fIndex = (findIdx*)malloc(sizeof(findIdx));

	//allocate memory for the Index vector
	int* I = (int*)malloc((n + 2) * sizeof(int));
	
	// initialize a length counter
	int m = 0;
	size_t i; // modified by Nasim 

	// set the first element of the array
	I[0] = 0;

	for (i = 0; i < n; i++)
		if (a[i] == b){
			I[m+1] = i+1; // modified by Nasim
			m++;
		}
	// reallocate memory by cutting the last part of I
	I = (int*)realloc(I, (m + 2) * sizeof(int));

	// set the last element of the array
	I[m + 1] = n;
	
	fIndex->Index  = I;
	fIndex->length = m + 2; // modified by Nasim

	// free the allocated memory
	//free(I); // Modified by Nasim
	//I = NULL; // Modified by Nasim

	return fIndex;
}

//--------------------------------------------------------------------------------------------
// print a frame
void printFrame(double** frame, const size_t m, const size_t n){

	printf("Frame data:\n");
	for (size_t j = 0; j < m; j++){
		for (size_t k = 0; k < n; k++)
			printf("%.6f\t", frame[j][k]);

		printf("\n");
	}
	printf("\n");
	return;
}

//--------------------------------------------------------------------------------------------
// find the unique elements of an array
void Unique(int *Array, int* uniqueArray, int ArraySize, int* uniquCnt)
{
	int CoutTemp = *uniquCnt;
	int numValues = ArraySize;
	int * index = (int *)malloc(numValues * sizeof(int));

	for (int i = 0; i < numValues; i++)
		index[i] = i;

	for (int i = 0; i < numValues - 1; i++) {
		for (int j = i + 1; j < numValues; j++) {
			if (Array[index[i]] > Array[index[j]])
			{
				int temp = index[i];
				index[i] = index[j];
				index[j] = temp;
			}
		}
	}
	int * sortedArry = (int *)malloc(numValues * sizeof(int));

	for (int i = 0; i < numValues; i++)
	{
		sortedArry[i] = Array[index[i]];
		printf("%d\n", sortedArry[i]);

	}
	free(index);

	printf("======================\n");


	uniqueArray[0] = sortedArry[0];

	for (int i = 0; i < numValues - 1; i++)
	{

		if (uniqueArray[CoutTemp] != sortedArry[i + 1])
		{
			CoutTemp++;
			uniqueArray[CoutTemp] = sortedArry[i + 1];
			//uniquCnt++;
			//printf("%d\n", uniqueArray[uniquCnt]);
		}
	}

	free(sortedArry);
	for (int i = 0; i < numValues; i++)
		//printf("%d\n", values[index[i]]);
		printf("%d\n", uniqueArray[i]);

	uniqueArray = (int*)realloc(uniqueArray, (CoutTemp + 1) * sizeof(int));
	printf("======================\n");
	printf("%d\n", CoutTemp);
	printf("======================\n");

	*uniquCnt = CoutTemp;


}

//********************************************************************
// Added by Nasim for median filter
void swap(int *p,int *q) {
    int t;
    
    t=*p;
    *p=*q;
    *q=t;
}

//*********************************************
// Sort the data, added by Nasim for median filter
void sort(int a[],const size_t  n) {
    int i,j;
    
    for(i=0; i< n-1 ;i++) {
        for(j=0; j < n-i-1 ;j++) {
            if(a[j] > a[j+1]){
                swap(&a[j],&a[j+1]);
            }
        }
    }
}


//***********************************************
// 1-D Median filter , added by Nasim
// statistical mode of an array a of length n


int median(int* x, const size_t n)

{
    
    int *temp = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
        temp[i] = x[i];
    
    int median;
    float tmp;
    int i, j;
    // the following two loops sort the array x in ascending order
    for(i=0; i< n-1; i++) {
        for(j= i+1; j<n; j++) {
            if(temp[j] < temp[i]) {
                // swap elements
                tmp     = temp[i];
                temp[i] = temp[j];
                temp[j] = tmp;
            }
        }
    }
    
    if(n % 2==0) {
        // if there is an even number of elements, return mean of the two elements in the middle
        median = ((temp[n/2] + temp[n/2 - 1]) / 2.0);
    } else {
        // else return the element in the middle
        median =  temp[n/2];
    }
    median = temp[ (n+1/ 2)-1];// -1 as array indexing in C starts from 0
   
    
    return median;
}
