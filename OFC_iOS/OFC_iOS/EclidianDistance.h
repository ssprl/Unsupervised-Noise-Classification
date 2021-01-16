// EclidianDistance.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//By Fatemeh Saki, University of Texas at Dallas, Spring 2017
// Modified by Nasim Alamdari, 2017
#pragma once
#include "math.h"

#ifdef __cplusplus
extern "C" {
#endif


// Declear the Euclidean distance function here.
// The definition is in EclideanDistance.cpp
void MyDistm(double **A, double **B, double **DistMtx, const int rA, const int rB, const int c);


#ifdef __cplusplus
}
#endif
