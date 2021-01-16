//By Fatemeh Saki, University of Texas at Dallas, Spring 2017
// Modified by Nasim Alamdari, 2017
#pragma once


#ifdef __cplusplus
extern "C" {
#endif
// Declare the Classification function
// The definition is in Frameprocessing.cpp
void Classification(const SVCluster* ClusterSet, double** frame, int* ClassOut, const int sizeC, const int m, const int n);

// Declare the connectionCheck function
// The definition is in Frameprocessing.cpp
int connectionCheck(double** frame1, double** frame2, const int Rowsize1, const int Rowsize2, const int ColSize, const double eps1, const double eps2);

// Declare the Cluster Creation function
// The definition is in Frameprocessing.cpp
SVCluster* ClusterCreation(double** svData, const int svRow, const int svCol, const double fracRejection, const double sigma);

// Declare the Frame analysis function
// The definition is in Frameprocessing.cpp
double frameAnalysis(double** frame, const int Rowsize, const int ColSize);

#ifdef __cplusplus
}
#endif
