// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//#include "targetver.h"
#include <stdio.h>
//#include <tchar.h> // tchar.h is just a set of macros to select different strings/functions for Unicode/8 bit characters, and is MicroSoft specific.
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#define INCHUNK_FLOOR_CONST		0.5

#ifdef __cplusplus
extern "C" {
#endif



// define the blueprints of the required structures here
typedef struct Chunk{
	int m, n;
    double** data;
	double* centroid ;
	double sigma;
} Chunk;

// define SVCluster
typedef struct SVCluster{
	int c, r;
	double** SVvectors ;
	double* alpha ;
	double Kxx, R2, sigma, offsets;
} SVCluster;

// define SVData
typedef struct SVData{
	double** data;
	int svRow, svCol, inChunk;
	bool crNovCL;
} SVData;

// define findIdx
typedef struct findIdx{
	int* Index;
	int length;
} findIdx;

// reference additional headers the program requires here
//#include "svm.h"
#include "utilities.h"
#include "EclidianDistance.h"
#include "Frameprocessing.h"
#include "massEvaluation.h"
#include "UnsupervisedClassification.h"

#ifdef __cplusplus
}
#endif
