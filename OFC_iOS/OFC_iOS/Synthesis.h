#ifndef SYNTHESIS_H
#define SYNTHESIS_H

#include <stdlib.h>

typedef struct Synthesis {
		int stepSize;
		int windowSize;
		float* window;
		float* previous1;
		float* previous2;
		float* output;
		void (*doSynthesis)(struct Synthesis* synthesis, float* input);
} Synthesis;

Synthesis* newSynthesis(int stepSize, int windowSize, float* window);
void destroySynthesis(Synthesis** synthesis);

#endif
