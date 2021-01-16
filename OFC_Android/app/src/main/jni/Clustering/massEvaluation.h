
#pragma once
//By Fatemeh Saki, University of Texas at Dallas, Spring 2017
// Modified by Nasim Alamdari, 2017
// Declare the mass evaluation function
// The definition is in massEvaluation.cpp

#ifdef __cplusplus
extern "C" {
#endif


//modified by Nasim , chunkSim added
SVData* massEvaluation(Chunk* myChunk, int* chunkSim, const int sizeL, const int inchunk);


#ifdef __cplusplus
}
#endif



