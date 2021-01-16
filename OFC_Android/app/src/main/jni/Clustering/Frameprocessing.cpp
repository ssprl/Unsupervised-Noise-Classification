//By Fatemeh Saki, University of Texas at Dallas, Spring 2017
//Changes made by Nasim Alamdari , 2017
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <dlib/svm.h>


// Macros for this module
#define R2_MULTIPLIER_CONST		1.1
#define THRESHOLD_CONST			0.3

// SVM Macros
#define SVM_TYPE				SVDD
#define KERNEL_TYPE				RBF
#define TOL						1e-8 
#define PI						3.14159265358979323846
#define CACHE_SIZE				100

// -----------------------------------------------------------------------------------------------
// Definition of the Classification function
void Classification(const SVCluster* ClusterSet, double** frame, int ClassOut[3], const int sizeC, const int m, const int n)
{
	/* 
	initialize the output vector
	ClassOut[0] is equivalent of CloseCL	in MATLAB
	ClassOut[1] is equivalent of ClassType	in MATLAB
	ClassOut[2] is equivalent of NovOp		in MATLAB
	*/
	for (int i = 0; i < 2; i++){
		ClassOut[i] = 0;}

	// allocating memory for the Dist results
	double **Dist = (double **)malloc(sizeC * sizeof(double *));
	for (int i = 0; i < sizeC; i++)
		Dist[i] = (double *)malloc(m* sizeof(double)); // modified by Nasim

	// allocating memory for the closeness index results
	double **CloseIdx = (double **)malloc(sizeC * sizeof(double *));
	for (int i = 0; i < sizeC; i++)
		CloseIdx[i] = (double *)malloc(m* sizeof(double)); // modified by Nasim


	// loop over the cluster set and perform the calculation
	for (int i = 0; i < sizeC; i++){

		// extract the structure information for the ith element
		int c = (ClusterSet + i)->c;
		int r = (ClusterSet + i)->r;
		double Kxx = (ClusterSet + i)->Kxx;
		double R2 = (ClusterSet + i)->R2;
		double sigma = (ClusterSet + i)->sigma;
		double offsets = (ClusterSet + i)->offsets;
		double* alpha = (ClusterSet + i)->alpha;
		double** SVvect = (ClusterSet + i)->SVvectors;


		// declare the kernelDist vector
		double *kernelDist = (double *)malloc(m * sizeof(double)); //modified by Nasim

		// allocating memory for the result pauerwise euclidian distance 
		double **DistMtx = (double **)malloc(r * sizeof(double *));
		for (int j = 0; j < r; j++)
			DistMtx[j] = (double *)malloc(m* sizeof(double));

		// calculate the distance 
		MyDistm(SVvect, frame, DistMtx, r, m, c);// corrected by Nasim

		// claculate alphaK (to save memory we reuse DistMtx) 
		myExp(DistMtx, alpha, r, m, sigma); // corrected by Nasim


		//ofstream out_data;
		//out_data.open("OFC Results.txt", ios::trunc); //modified by Nasim
		// loop over j to calculate the kernelDist
		for (int j = 0; j < m; j++){ // modified by Nasim
			double sum = 0;
			for (int k = 0; k < r; k++){
				sum += DistMtx[k][j]; //modified by Nasim (previously was DistMtx[j][k];)
			}


			//kernelDist[j] = (1 - 2 * sum + Kxx) / (R2_MULTIPLIER_CONST * R2);
			kernelDist[j] = (1 - 2 * sum + Kxx); // Modified by Nasim
			Dist[i][j] = kernelDist[j] / (R2_MULTIPLIER_CONST * R2);

			// Perform a comparision for the calculation of CloseIdx
			if (kernelDist[j] <= (R2_MULTIPLIER_CONST * R2))
				CloseIdx[i][j] = 1; //modified by Nasim
			else
				CloseIdx[i][j] = 0;

			//out_data << CloseIdx[i][j] << '\n';

		}// end of the for loop
		//out_data.close();
		// free the memory for the current SVCluster
		for (int j = 0; j < r; j++) free(DistMtx[j]);
		free(DistMtx);

		free(kernelDist);

		DistMtx = NULL;
		kernelDist = NULL;
	}// end of the for loop
	
	// declare the sumCloseIdx vector
	double *sumCloseIdx = (double *)malloc(sizeC * sizeof(double));

	// calculate the sum of closeIdx matrix across the first dimension
	for (int i = 0; i < sizeC; i++){
		double sum = 0;
		for (int j = 0; j < m; j++) //modified by Nasim
			sum += CloseIdx[i][j];

		sumCloseIdx[i] = sum;
	}
	
	// define a threshold
	double threshold = floor(THRESHOLD_CONST * m);

	// perform the required comparisions
	if (sizeC == 1){
		if (compareArrays(sumCloseIdx, threshold, sizeC)){
			ClassOut[0] = 1;
			ClassOut[1] = 1;
			ClassOut[2] = 0; // modified by Nasim
		}
		else
			ClassOut[0] = 0; // modified by Nasim
		    ClassOut[1] = 0; // modified by Nasim
			ClassOut[2] = 1;

	}
	else if (sizeC > 1){
		// declare the Clust vector
		int *Clust = (int *)malloc(m * sizeof(int)); // modified by Nasim
		// find the min of matrix dist across the first dimension
		minMatrixR(Dist, Clust, sizeC, m); 

		//ofstream out_data;
		//out_data.open("OFC Results.txt", ios::trunc); //modified by Nasim
		int *Clust2 = (int *)malloc(m * sizeof(int)); //added by Nasim
		for (int i = 0; i < m; i++){ //added by Nasim
			Clust2[i] = Clust[i] + 1;
			//out_data << Clust2[i] << ',';
		}
		//out_data.close();
		// take the statistical mode of array Clust
		int CloseCL2 = mode(Clust2, m); // modified by Nasim
		// now compute the sum
		double sum = 0;
		for (int i = 0; i < m; i++) //modified by Nasim
			sum += CloseIdx[CloseCL2-1][i]; // modified by Nasim

		if (sum>=threshold){
			ClassOut[0] = (int)CloseCL2;
			ClassOut[1] = 1;
		}
		else if (sum==0)
			ClassOut[2] = 1;

	}
	

	//free the allocated memory
	for (int i = 0; i < sizeC; i++) free(Dist[i]);
	free(Dist);

	for (int i = 0; i < sizeC; i++) free(CloseIdx[i]);
	free(CloseIdx);

	free(sumCloseIdx);

	Dist		= NULL;
	CloseIdx	= NULL;
	sumCloseIdx = NULL;

	return;
}



// -----------------------------------------------------------------------------------------------
// Definition of the connection Check function
int connectionCheck(double** frame1, double** frame2, const int Rowsize1, const int Rowsize2, const int ColSize, const double eps1, const double eps2){

	// allocating memory for the euclidian distance matrix
	double **DistMtx = (double **)malloc(Rowsize1 * sizeof(double *));
	for (int i = 0; i < Rowsize1; i++)
		DistMtx[i] = (double *)malloc(Rowsize2 * sizeof(double)); //modified byNasim

	// run the Eclidean distance function
	MyDistm(frame1, frame2, DistMtx, Rowsize1, Rowsize2, ColSize);

	// calculate the conditional sum of the matrix entries
	double clustDist = sumMatrixCond(DistMtx, Rowsize1, Rowsize2, eps1 + eps2); // modified by Nasim


	// free the allocated memory 
	for (int i = 0; i < Rowsize1; i++) free(DistMtx[i]); //modified by Nasim
	free(DistMtx);

	// avoid danggling pointer
	DistMtx = NULL;

	if (clustDist > (THRESHOLD_CONST * Rowsize1 * THRESHOLD_CONST * Rowsize2))
		return +1;

	return -1;
}



// -----------------------------------------------------------------------------------------------
// Definition of the Frame analysis function
//Confirmed by Nasim
double frameAnalysis(double** frame, const int RowSize, const int ColSize)
{
	// allocating memory for the euclidian distance matrix
	double **DistMtx = (double **)malloc(RowSize * sizeof(double *));
	for (int i = 0; i < RowSize; i++)
		DistMtx[i] = (double *)malloc(RowSize * sizeof(double));

	// run the Eclidean distance function
	MyDistm(frame, frame, DistMtx, RowSize, RowSize, ColSize);

	// find the mean of the matrix entries
	double mDist = meanMatrix(DistMtx, RowSize, ColSize)/2;

	// find the maximum element
	double radius = maxMatrix(DistMtx, RowSize, ColSize);
	
	// for debug purpose only
	//printFrame(DistMtx, Rowsize, ColSize);

	// free the allocated memory 
	for (int i = 0; i < RowSize; i++) free(DistMtx[i]);
	free(DistMtx);

	// avoid danggling pointer
	DistMtx = NULL;

	return radius;
}

// -----------------------------------------------------------------------------------------------
// Definition of the Cluster Creation function //modified by Nasim
SVCluster* ClusterCreation(double** svData, const int svRow, const int svCol, const double fracRejection, const double sigma){

	//******************************* Nasim added: dlib one-class SVM****************
	// We will use column vectors to store our points.  Here we make a convenient typedef
	// for the kind of vector we will use.
	typedef dlib::matrix<double,0, 1> sample_type; //Nasim: This typedef declares a matrix with unknow(0) rows and 1 column. This is column vector.

	// Then we select the kernel we want to use.  For our present problem the radial basis
  // kernel is quite effective.
	typedef dlib::radial_basis_kernel<sample_type> kernel_type; //Nasim: use same

	std::vector<sample_type> samples;
	sample_type samp(svCol); //Nasim: my data should be m(8), since 8 features every vector

	for (int i = 0; i< svRow; i++)
	{
			samp(0) = svData[i][0];
			samp(1) = svData[i][1];
			samp(2) = svData[i][2];
			samp(3) = svData[i][3];
			samp(4) = svData[i][4];
			samp(5) = svData[i][5];
			samp(6) = svData[i][6];
			samp(7) = svData[i][7];
			samples.push_back(samp);
	}

	//vector_normalizer<sample_type> normalizer;
	// let the normalizer learn the mean and standard deviation of the samples
	//normalizer.train(samples);
	// now normalize each sample
	//for (unsigned long i = 0; i < samples.size(); ++i)
	//	samples[i] = normalizer(samples[i]);

	// Now make the object responsible for training one-class SVMs.
	dlib::svm_one_class_trainer<kernel_type> trainer;

	// Here we set the width of the radial basis kernel
	trainer.set_kernel(kernel_type(sigma)); // Nasim 4.0 is width of rbf kernel (sigma)
	double NU = svRow*fracRejection;
	trainer.set_nu((NU < 1) ? 1 : 1 / NU);
	trainer.set_epsilon(TOL);
	//trainer.set_nu(1/svRow*fracRejection);
	//trainer.set_lambda(0.00001);
	//const double C = get_option(parser, "c", 1.0);
	//trainer.set_c(C);

	// Now train a one-class SVM.  The result is a function, df(),
	dlib::decision_function<kernel_type> df = trainer.train(samples);

	//cout << "\n number of support vectors in our learned_function is "
	//	<< df.basis_vectors.size() << endl;

	//df.alpha;
	//df.b;
	//df.basis_vectors;
	//df.kernel_function;
	//df.kernel_function.gamma;

	//***********************************************************************************
	// initialize mySVClust by allocating memory dynamically
	SVCluster* mySVClust = (SVCluster *)malloc(sizeof(SVCluster));

	// allocating memory for the SVvectors matrix //modified by Nasim
	double **SVvectors = (double **)malloc(df.basis_vectors.size() * sizeof(double *));
	for (int j = 0; j < df.basis_vectors.size(); j++)
	SVvectors[j] = (double *)malloc(svCol * sizeof(double));

	// populate the SVvectors matrix from the model
	for (int i = 0; i < df.basis_vectors.size(); i++) {
		for (int j = 0; j < svCol; j++) {
			SVvectors[i][j] = df.basis_vectors(i)(j);
        
		}
    
	}
	
	double sumAlpha = 0;
	for (int i = 0; i < df.basis_vectors.size(); i++) {
		sumAlpha += df.alpha(i);
	}

	double *alpha = (double *)malloc(df.basis_vectors.size() * sizeof(double));
	for (int i = 0; i < df.basis_vectors.size(); i++) {
		alpha[i] = df.alpha(i) / sumAlpha; //Nasim: not sure about devision by sumAlpha
		//alpha[i] = df.alpha(i); //Nasim
		
	}
	

	// allocating memory for the result pauerwise euclidian distance 
	double **DistMtx = (double **)malloc(df.basis_vectors.size() * sizeof(double *));
	for (int j = 0; j < df.basis_vectors.size(); j++)
	DistMtx[j] = (double *)malloc(df.basis_vectors.size() * sizeof(double));

	// calculate the distance 
	MyDistm(SVvectors, SVvectors, DistMtx, df.basis_vectors.size(), df.basis_vectors.size(), svCol);

	// claculate Kxx (to save memory we reuse DistMtx) 
	double Kxx = myExp_alpha(DistMtx, alpha, df.basis_vectors.size(), sigma);

	//------added by Nasim----------------------------------------
	double *Dx = (double *)malloc(df.basis_vectors.size() * sizeof(double));
	for (int i = 0; i < df.basis_vectors.size(); i++) {
		double K = 0;
		for (int j = 0; j < df.basis_vectors.size(); j++) {
			K += alpha[i]*exp(-DistMtx[j][i] / (sigma * sigma)); 
		}
		Dx[i] = -2 * K;
	}

	double offs = 1 + Kxx;
	double sum = 0;
	for (int i = 0; i < df.basis_vectors.size(); i++) {
		sum += Dx[i];
	}
	double R2 = sum / df.basis_vectors.size(); //
	double threshold = R2 + offs;
	//------------------------------------------------------------------
	// free the memory for DistMtx
	for (int j = 0; j < df.basis_vectors.size(); j++) free(DistMtx[j]);
	free(DistMtx);

	DistMtx = NULL;


	// pack the data in the structure
	mySVClust->SVvectors    = SVvectors;
	mySVClust->alpha        = alpha;
	mySVClust->r			= df.basis_vectors.size();
	mySVClust->c            = svCol;
	mySVClust->Kxx			= Kxx;
	mySVClust->R2           = threshold;//model->r_square;	// had to make changes to the svm library to get access to this
	mySVClust->sigma        = df.kernel_function.gamma;			// redundant
	mySVClust->offsets      = offs;				// // Modified by Nasim

										// free the svm-related dynamic memory
										//free(prob.x);
										//free(prob.y);
										//free(x_space);
										//svm_free_and_destroy_model(&model);
										//svm_destroy_param(&param);

	return mySVClust;
}
