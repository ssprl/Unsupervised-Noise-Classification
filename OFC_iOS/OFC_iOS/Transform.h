#ifndef TRANSFORM_H
#define TRANSFORM_H
#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct Transforming {
		int points;
		int windowSize;
		float* real;
		float* imaginary;
		float* power;
		float* sine;
		float* cosine;
        float* window;
        float totalPower;
        float dBSPL;
        float dbpower;
        float* dbpowerBuffer;
        int framesPerSecond;
} Transforming;

Transforming* newTransforms(int windowSize, int framesPerSecond);
void ForwardFFTransform(Transforming* fft, float* real);
void InverseFFTransform(Transforming* fft);
void destroyTransforms(Transforming** transform);

#endif
