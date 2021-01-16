//By Fatemeh Saki, University of Texas at Dallas, Spring 2017
#include "stdafx.h"


void MyDistm(double **A, double **B, double **DistMtx, const int rA, const int rB, const int c){

	// initialize local variables
	double sum;
	// run through and calculate the Euclidean distance
	for (int row1 = 0; row1 < rA; row1++)
	{
		for (int row2 = 0; row2 < rB; row2++)
		{
			sum = 0;

			for (int col = 0; col < c; col++)

				sum += (A[row1][col] - B[row2][col])*(A[row1][col] - B[row2][col]);
			DistMtx[row1][row2] = sqrt(sum);
		}
	}
	return;
}

