//By Fatemeh Saki, University of Texas at Dallas, Spring 2017
// Modified by Nasim Alamdari, 2017
#pragma once

#ifdef __cplusplus
extern "C" {
#endif



// Declare utility functions here
void myExp(double** DistMtx, const double* alpha, const size_t r, const size_t n, const double sigma);
void minMatrixR(double** M, int* I, const size_t r, const size_t c);
void meanMatrixR(double** M, double* I, const size_t m, const size_t n);
void circularShift(int* I, const size_t n, const int amount);
void circularShift_d(double* I, const size_t n, const int amount);
void printFrame(double** frame, const size_t m, const size_t n);
void Unique(int *I, int* uniqueI, int arraySize, int* uniqueCount);

int compareArrays(const double* a, const double b, const size_t n);
int mode(const int* I, const size_t n);

double meanMatrix(double** M, const size_t m, const size_t n);
double maxMatrix(double** M, const size_t m, const size_t n);
double sumMatrixCond(double** M, const size_t m, const size_t n, const double target);
double myExp_alpha(double** DistMtx, const double* alpha, const size_t n, const double sigma);
    
void swap(int *p,int *q); // added by Nasim , for median filter
void sort(int a[],const size_t  n); // added by Nasim
int median(int* x, const size_t n); //added by Nasim
    
findIdx* find(const double* a, const double b, const size_t n);
//findIdx* find(const bool* a, const bool b, const size_t n); // Nasim :did not used in new version
findIdx* find2(const int* a, const int b, const int c, const int n); //added by Nasim
    



#ifdef __cplusplus
}
#endif
