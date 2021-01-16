#include "Synthesis.h"

void first(Synthesis* synthesis, float* input);
void second(Synthesis* synthesis, float* input);
void final(Synthesis* synthesis, float* input);

Synthesis*
newSynthesis(int stepSize, int windowSize, float* window)
{
	Synthesis* newSynthesis = (Synthesis*)malloc(sizeof(Synthesis));

	newSynthesis->stepSize = stepSize;
	newSynthesis->windowSize = windowSize;

	newSynthesis->window = (float*)malloc(stepSize*sizeof(float));
	newSynthesis->previous1 = (float*)malloc(stepSize*sizeof(float));
	newSynthesis->previous2 = (float*)malloc(stepSize*sizeof(float));
	newSynthesis->output = (float*)calloc(stepSize,sizeof(float));

	int repeat = windowSize-2*stepSize;
	int i;
	for(i=0;i<stepSize;i++){
		newSynthesis->window[i] = window[i+repeat] + window[i+(windowSize-stepSize)];
	}
	for(i=0;i<repeat;i++){
		newSynthesis->window[i+stepSize-repeat] += window[i];
	}
	for(i=0;i<stepSize;i++){
		newSynthesis->window[i] = 1.0f/newSynthesis->window[i];
	}

	newSynthesis->doSynthesis = first;

	return newSynthesis;
}

void
first(Synthesis* synthesis, float* input)
{
	int i;
	int overlap = synthesis->windowSize - synthesis->stepSize;
	for(i=0;i<synthesis->stepSize;i++) {
		synthesis->previous2[i] = input[i+overlap];
	}
	synthesis->doSynthesis = second;
}

void
second(Synthesis* synthesis, float* input)
{
	int i;
	int overlap = synthesis->windowSize - synthesis->stepSize;
	int repeat = overlap - synthesis->stepSize;
	for(i=0;i<synthesis->stepSize;i++) {
		synthesis->previous1[i] = input[i+overlap];
		synthesis->previous2[i] += input[i+repeat];
	}
	synthesis->doSynthesis = final;
}

void
final(Synthesis* synthesis, float* input)
{
	int i;
	int overlap = synthesis->windowSize - synthesis->stepSize;
	int repeat = overlap - synthesis->stepSize;

	for(i=0;i<synthesis->stepSize;i++) {
		synthesis->output[i] = synthesis->previous2[i];
		synthesis->previous2[i] = synthesis->previous1[i] + input[i+repeat];
		synthesis->previous1[i] = input[i+overlap];
	}

	for(i=0;i<repeat;i++) {
		synthesis->output[i+(synthesis->stepSize-repeat)] += input[i];
	}

	for(i=0;i<synthesis->stepSize;i++) {
		synthesis->output[i] *= synthesis->window[i];
	}
}

void
destroySynthesis(Synthesis** synthesis)
{
	if(*synthesis != NULL){
		if((*synthesis)->window != NULL){
			free((*synthesis)->window);
			(*synthesis)->window = NULL;
		}
		if((*synthesis)->previous1 != NULL){
			free((*synthesis)->previous1);
			(*synthesis)->previous1 = NULL;
		}
		if((*synthesis)->previous2 != NULL){
			free((*synthesis)->previous2);
			(*synthesis)->previous2 = NULL;
		}
		if((*synthesis)->output != NULL){
			free((*synthesis)->output);
			(*synthesis)->output = NULL;
		}
		free(*synthesis);
		*synthesis = NULL;
	}
}
