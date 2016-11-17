#ifndef gradiantmethod_h_
#define gradiantmethod_h_
#define _CRT_SECURE_NO_WARNINGS
/*
This file implements the BCGD algorithm for dynamic ovelapping community detection algorithm

/*@author: linhong (linhong.seba.zhu@gmail.com)
*/
///----------------------------------------------------------------------//
#include"Updatecommunity.h"

//===================================================
//Compute the BCGD Local algorithm in algorithm 2 
//where step size is an input parameter          (START)
//or step size is automatically determined by 
//lipschitz constant
//the full graph and the full matrix can not be 
//loaded into the memory, instead, we sequential scan the graph
//and the matrix, load partial into the memory based on memorybound
//update them and save results back to disk
//===================================================
inline void BCGDLocalSparsefaster(int m, vector<char *> &filenames, double lambda, double membound, string &prefix, int printstep, bool isauto, int maxnodenum){
	int i = 0;
	int j = 0;
	int t = 0;
	Node *G;
	SparseMatrix Z;
	SparseMatrix Zprime;
	int nodenum = 0;
	int iter = 0;
	int gnodenum = 0;
	string s("Zmatrix");
	s = prefix + s;
	char *cfilename = new char[2048];
	char *cprefilename = new char[2048];
	char *tempstr = new char[2048];
	double error;
	double preerror;
	double **B = new double *[m];
	for (i = 0; i < m; i++){
		B[i] = new double[m];
	}
	double epsilo;
	FILE *rFile = NULL;
	//Initilize the memory usage (start)
	InitTempSubmemory(maxnodenum);
	double staticsize = sizeof(double)*maxnodenum;
	staticsize += (sizeof(int)*maxnodenum * 2);
	staticsize = staticsize / 1024;
	if (membound <= staticsize){
		cout << "Please increase the membound by " << staticsize << endl;
		exit(2);
	}
	membound -= staticsize;
	//Initilize the memory usage (end)

	//=============================================================
	//Random Initialize the Zmatrix for time 0 (	START)
	//==============================================================
	strcpy(cfilename, s.c_str());
	itostring(0, tempstr, 10);
	strcat(cfilename, tempstr);
	InitZ(m, true, filenames.at(0), cfilename, membound);
    releaseInitZmemory();
	cout << "finish initialization" << endl;
	//=============================================================
	//Random Initialize the Zmatrix for time 0 (end)
	//==============================================================
	FILE *rfile3 = NULL;
	FILE *rfile2 = NULL;
	FILE *wfile = NULL;
	FILE *wfiletemp = NULL;
	double a1;
	double a2;
	double a;
	for (t = 0; t < (int)filenames.size(); t++){
		//============================================
		//=====Open graph file to read (START)
		//===========================================
		rFile = fopen(filenames.at(t), "r");
		if (rFile == NULL){
			printf("Could not find the data file\n");
			exit(1);
		}
		int rv = fscanf(rFile, "%d\n", &nodenum);
		if (rv != 1){
			printf("the format of graph file %s is not correct", filenames.at(i));
			exit(2);
		}
		//============================================
		//=====Open graph file to read (END)
		//===========================================

		//=============================================
		//=========OPEN Z matrix pre file to read (START)
		//===============================================
		if (t > 0){
			strcpy(cprefilename, s.c_str());
			itostring(t - 1, tempstr, 10);
			strcat(cprefilename, tempstr);
		}
		else{
			strcpy(cprefilename, s.c_str());
			itostring(t, tempstr, 10);
			strcat(cprefilename, tempstr);
		}
		rfile3 = fopen(cprefilename, "r");
		rv = fscanf(rfile3, "%d\n", &nodenum);
		if (rv != 1){
			printf("the format of matrix file %s is not correct", cfilename);
			exit(2);
		}

		//=============================================
		//=========OPEN Z matrix pre file to read (END)
		//===============================================
		if (t > 0){
			//=============================================
			//=========OPEN Z matrix file to write (START)
			//===============================================
			strcpy(cfilename, s.c_str());
			itostring(t, tempstr, 10);
			strcat(cfilename, tempstr);
			wfile = fopen(cfilename, "w");
			if (wfile == NULL){
				printf("could not write result to disk\n");
				exit(1);
			}
			fprintf(wfile, "%d\n", nodenum);
			//=============================================
			//=========OPEN Z matrix file to write (END)
			//===============================================
		}
		else{
			//============================================
			//=======Open temp file to write (START)
			//============================================
			wfiletemp = fopen("temp.txt", "w");
			if (wfiletemp == NULL){
				printf("could not write result to disk\n");
				exit(1);
			}
			fprintf(wfiletemp, "%d\n", nodenum);
			//============================================
			//=======Open temp file to write (END)
			//============================================
		}
		totalfinish = -1;
		Vmin = nodenum;
		Vmax = 0;
		memset(node2subidx, -1, sizeof(int)*nodenum);
		while (1){
			//remember where we are in memory usage (START)
			char *precurMem2Start = curMemPos;
			char *precurMem2End = curMemEnd;
			int precurblk2 = curBlk;
			//remember where we are in memory usage (END)
			gnodenum = 0;
			memset(isseednode, 'v', sizeof(char)*nodenum);
			ReadSeeNode(gnodenum, nodenum, 10, membound, rFile);
			ReadSubGraph(G, gnodenum, nodenum, rFile);
			//GraphNorm(G, gnodenum);
			Readsubcommunity(Zprime, gnodenum, nodenum, rfile3);
			Z = Zprime;
			iter = 0;
			error = 0;
			a1 = 1;
			if (iter%printstep == 0)
				printf("in memory seed set is [%d, %d]\n", Vmin, Vmax);
			do{
				Z.Gettransposesqure(B);
				a2 = (1 + sqrt(4 * a1*a1 + 1)) / 2;
				a = (a2 + a1 - 1) / a2;
				if (isauto == true){
					alpha = 1.0 / ComputeL(nodenum,m);
					if (iter%printstep == 0)
						printf("stepsize value now is %.10lf\n", alpha);
				}
				for (i = 0; i < gnodenum; i++){
					if (isauto == false)
						updateZauto(Z, G, i, lambda, B, Zprime.matrix[i], 1.0);
					else
						updateZauto(Z, G, i, lambda, B, Zprime.matrix[i], a);
				}
				double navg = Getnonlink(G, Z, gnodenum);
				error = Fnormfast(G, Z, gnodenum);
				if (iter == 0)
					preerror = gnodenum;
				epsilo = (preerror - error) / gnodenum;
				if (iter%printstep == 0)
					printf("%d\t%f\t%.10lf\n", iter, error, epsilo);
				preerror = error;
				a1 = a2;
				iter++;
			} while (iter < maxiter&&epsilo>0.000000001);
			if (t > 0){
				Z.Writetofile(wfile, G);
			}
			else
				Z.Writetofile(wfiletemp, G);
			//==========================================================
			//==============Release Used Memory  (START)================
			//==========================================================
			Zprime.clear();
			releasegraphmemory(G, gnodenum);
			Z.clear();
			curMemPos = precurMem2Start;
			curMemEnd = precurMem2End;
			curBlk = precurblk2;

			if (G != NULL){
				free(G);
				G = NULL;
			}
			Zprime.deletemem();
			Z.deletemem();
			//_CrtDumpMemoryLeaks();
			//==========================================================
			//==============Release Used Memory  (END)================
			//==========================================================
			//update in-memory seed set
			Vmin = nodenum;
			totalfinish = Vmax;
			if (Vmax == nodenum - 1)
				break;
		}
		if (rFile != NULL) {
			fclose(rFile);
			rFile = NULL;
		}
		if (rfile3 != NULL) {
			fclose(rfile3);
			rfile3 = NULL;
		}
		if (t > 0){
			fclose(wfile);
			wfile = NULL;
		}
		else{
			fclose(wfiletemp);
			wfiletemp = NULL;
			//replace Zmatrixt with temp.txt
			Replacefile(cfilename, "temp.txt");
		}
	}
    releaseTempSubmemory();
	if (B != NULL) {
		for(i=0;i<m;i++){
			if (B[i] != NULL) {
				delete[]B[i];
				B[i] = NULL;
			}
		}
		delete[]B;
		B = NULL;
	}
	if (cfilename != NULL) {
		delete[]cfilename;
		cfilename = NULL;
	}
	if (cprefilename != NULL) {
		delete[]cprefilename;
		cprefilename = NULL;
	}
	if (tempstr != NULL) {
		delete[]tempstr;
		tempstr = NULL;
	}
}
inline void BCGDLocalSparse(int m, vector<char *> &filenames, double lambda, double membound, string &prefix, int printstep, bool isauto, int maxnodenum){
	int i=0;
	int j=0;
	int t=0;
	Node *G;
	SparseMatrix Z;
	SparseMatrix Zprime;
	int nodenum=0;
	int iter=0;
	int gnodenum=0;
	string s("Zmatrix");
	s = prefix + s;
	char *cfilename=new char[2048];
	char *cprefilename=new char[2048];
	char *tempstr=new char[2048];
	double error;
	double preerror;
	double epsilo;
	double **B = new double *[m];
	for (i = 0; i < m; i++){
		B[i] = new double[m];
	}
	FILE *rFile=NULL;
	//Initilize the memory usage (start)
    InitTempSubmemory(maxnodenum);
	double staticsize = sizeof(double)*maxnodenum;
	staticsize += (sizeof(int)*maxnodenum * 2);
	staticsize=staticsize / 1024;
	if (membound <= staticsize){
		cout << "Please increase the membound by " << staticsize << endl;
		exit(2);
	}
	membound -= staticsize;
	//Initilize the memory usage (end)

	//=============================================================
	//Random Initialize the Zmatrix for time 0 (	START)
	//==============================================================
	strcpy(cfilename, s.c_str());
	itostring(0, tempstr, 10);
	strcat(cfilename, tempstr);
	InitZ(m, true, filenames.at(0), cfilename, membound);
    releaseInitZmemory();
	cout << "finish initialization" << endl;
	//=============================================================
	//Random Initialize the Zmatrix for time 0 (end)
	//==============================================================
	FILE *rfile3=NULL;
	FILE *rfile2=NULL;
	FILE *wfile=NULL;
	double a1;
	double a2;
	double a;
	for (t = 0; t < (int)filenames.size(); t++){
		iter = 0;
		error = 0;
		a1 = 1;
		//============================================
		//=====Open graph file to read (START)
		//===========================================
		rFile = fopen(filenames.at(t), "r");
		if (rFile == NULL){
			printf("Could not find the data file\n");
			exit(1);
		}
		int rv = fscanf(rFile, "%d\n", &nodenum);
		if (rv != 1){
			printf("the format of graph file %s is not correct", filenames.at(i));
			exit(2);
		}
		//============================================
		//=====Open graph file to read (END)
		//===========================================

		//=============================================
		//=========OPEN Z matrix pre file to read (START)
		//===============================================
		if (t > 0){
			strcpy(cprefilename, s.c_str());
			itostring(t - 1, tempstr, 10);
			strcat(cprefilename, tempstr);
			rfile3 = fopen(cprefilename, "r");
			rv = fscanf(rfile3, "%d\n", &nodenum);
			if (rv != 1){
				printf("the format of matrix file %s is not correct", cfilename);
				exit(2);
			}
		}
		
		//=============================================
		//=========OPEN Z matrix pre file to read (END)
		//===============================================
		do{
			//============================================
			//=======Open temp file to write (START)
			//============================================
			wfile = fopen("temp.txt", "w");
			if (wfile == NULL){
				printf("could not write result to disk\n");
				exit(1);
			}
			fprintf(wfile, "%d\n", nodenum);
			//============================================
			//=======Open temp file to write (END)
			//============================================
			//=============================================
			//=========OPEN Z matrix file to read (START)
			//===============================================
			strcpy(cfilename, s.c_str());
			itostring(t, tempstr, 10);
			strcat(cfilename, tempstr);
			if (iter > 0 || t == 0){
				rfile2 = fopen(cfilename, "r");
				rv = fscanf(rfile2, "%d\n", &nodenum);
				if (rv != 1){
					printf("the format of matrix file %s is not correct", cfilename);
					exit(2);
				}
			}
			//=============================================
			//=========OPEN Z matrix file to read (END)
			//===============================================
			totalfinish = -1;
			Vmin = nodenum;
			Vmax = 0;
			memset(node2subidx, -1, sizeof(int)*nodenum);
			error = 0;
			while (1){
				//remember where we are in memory usage (START)
				char *precurMem2Start = curMemPos;
				char *precurMem2End = curMemEnd;
				int precurblk2 = curBlk;
				//remember where we are in memory usage (END)
				gnodenum = 0;
				memset(isseednode, 'v', sizeof(char)*nodenum);
				ReadSeeNode(gnodenum, nodenum, 30, membound, rFile);
				ReadSubGraph(G, gnodenum, nodenum, rFile);
				//GraphNorm(G, gnodenum);
				if (iter%printstep == 0)
					printf("in memory seed set is [%d, %d]\n", Vmin, Vmax);
				if (iter == 0){
					if (t > 0){
						Readsubcommunity(Zprime, gnodenum, nodenum, rfile3);
						Z = Zprime;
					}
					else{
						Readsubcommunity(Z, gnodenum, nodenum, rfile2);
						Zprime = Z;
					}

				}
				else{
					Readsubcommunity(Z, gnodenum, nodenum, rfile2);
					if (t > 0)
						Readsubcommunity(Zprime, gnodenum, nodenum, rfile3);
					else
						Zprime = Z;

				}
				Z.Gettransposesqure(B);
				//double sumZ = Z.Fnorm();
				//sumZ = pow(sumZ, 2);
				//if (iter%printstep == 0){
					//printf("sumZ value %lf and gnum%d\n", sumZ, gnodenum);
				//}
				a2 = (1 + sqrt(4 * a1*a1 + 1)) / 2;
				a = (a2 + a1 - 1) / a2;
				if (isauto == true){
					alpha = 1.0 / ComputeL(nodenum,m);
					if (iter%printstep == 0)
						printf("stepsize value now is %.10lf\n", alpha);
				}
				for (i = 0; i < gnodenum; i++){
					if (isauto == false)
						updateZauto(Z, G, i, lambda, B, Zprime.matrix[i], 1.0);
					else
						updateZauto(Z, G, i, lambda, B, Zprime.matrix[i], a);
				}
				////////////////////////////////////////////////////////////
				Zprime.clear();
				Z.Writetofile(wfile, G);
				error += Fnormfast(G, Z, gnodenum);
				//==========================================================
				//==============Release Used Memory  (START)================
				//==========================================================
				releasegraphmemory(G, gnodenum);
				Z.clear();
				curMemPos = precurMem2Start;
				curMemPos = precurMem2End;
				curBlk = precurblk2;

				if (G != NULL){
					free(G);
					G = NULL;
				}
				Zprime.deletemem();
				Z.deletemem();
				//_CrtDumpMemoryLeaks();
				//==========================================================
				//==============Release Used Memory  (END)================
				//==========================================================
				//update in-memory seed set
				Vmin = nodenum;
				totalfinish = Vmax;
				if (Vmax == nodenum - 1)
					break;
			}
			if (wfile != NULL) {
				fclose(wfile);
				wfile = NULL;
			}
			if (rfile2 != NULL) {
				fclose(rfile2);
				rfile2 = NULL;
			}
			//replace Zmatrixt with temp.txt
			Replacefile(cfilename, "temp.txt");
			if (iter == 0)
				preerror = nodenum;
			epsilo = (preerror - error) / nodenum;
			if (iter%printstep == 0)
				printf("%d\t%f\t%lf\n", iter, error, epsilo);
			preerror = error;
			a1 = a2;
			iter++;
		} while (iter < maxiter&&epsilo>0.000000001);
		if (rFile != NULL) {
			fclose(rFile);
			rFile = NULL;
		}
		if (rfile3 != NULL) {
			fclose(rfile3);
			rfile3 = NULL;
		}
	}
    releaseTempSubmemory();
	if (cfilename != NULL) {
		delete[]cfilename;
		cfilename = NULL;
	}
	if (cprefilename != NULL) {
		delete[]cprefilename;
		cprefilename = NULL;
	}
	if (tempstr != NULL) {
		delete[]tempstr;
		tempstr = NULL;
	}
	if (B != NULL) {
		for(i=0;i<m;i++){
			if (B[i] != NULL) {
				delete[]B[i];
				B[i] = NULL;
			}
		}
		delete[]B;
		B = NULL;
	}
}

//===================================================
//Compute the BCGD Local algorithm in algorithm 2 
//where step size is an input parameter          (END)
//or step size is automatically determined by 
//lipschitz constant
//the full graph and the full matrix can not be 
//loaded into the memory, instead, we sequential scan the graph
//and the matrix, load partial into the memory based on memorybound
//update them and save results back to disk
//===================================================

//===========================================================================================================================================
//============================================================================================================================================
//============================================================================================================================================
//= == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==
//============================================================================================================================================
//============================================================================================================================================

//===================================================
//Compute the BCGD Local algorithm in algorithm 2 
//where step size is an input parameter          (START)
//or step size is automatically determined by 
//lipschitz constant
//the full graph is store in memory
//===================================================

inline void BCGDLocalFull(int m, vector<char *> &filenames, double lambda, string &prefix, int printstep, bool isauto, int maxnodenum){
	int i = 0;
	int j = 0;
	int t = 0;
	Node *G;
	SparseMatrix Z;
	SparseMatrix Zprime;
	int nodenum = 0;
	int iter = 0;
	string s("Zmatrix");
	s = prefix + s;
	char *cfilename = new char[2048];
	char *cprefilename = new char[2048];
	char *tempstr = new char[2048];
	double error;
	double preerror;
	double epsilo;
	double **B = new double *[m];
	for (i = 0; i < m; i++){
		B[i] = new double[m];
	}
	//Initilize the memory usage (start)
    InitTempFullmemory(maxnodenum);
	//Initilize the memory usage (end)

	//=============================================================
	//Random Initialize the Zmatrix for time 0 (	START)
	//==============================================================
	strcpy(cfilename, s.c_str());
	itostring(0, tempstr, 10);
	strcat(cfilename, tempstr);
	InitZfull(m, true, filenames.at(0), cfilename);
    releaseInitZmemory();
	cout << "finish initialization" << endl;
	//=============================================================
	//Random Initialize the Zmatrix for time 0 (end)
	//==============================================================
	FILE *rfile3 = NULL;
	FILE *wfile = NULL;
	FILE *rFile = NULL;
	double a1;
	double a2;
	double a;
	double L;
	for (t = 0; t < (int)filenames.size(); t++){
		iter = 0;
		error = 0;
		a1 = 1;
		char *precurMemStart = curMemPos;
		char *precurMemEnd = curMemEnd;
		int precurblk = curBlk;
		//============================================
		//=====Open graph file to read (START)
		//===========================================
		rFile = fopen(filenames.at(t), "r");
		if (rFile == NULL){
			printf("Could not find the data file\n");
			exit(1);
		}
		int rv = fscanf(rFile, "%d\n", &nodenum);
		if (rv != 1){
			printf("the format of graph file %s is not correct", filenames.at(t));
			exit(2);
		}
		ReadGraph(G, nodenum, rFile);
		if (rFile != NULL) {
			fclose(rFile);
			rFile = NULL;
		}
		//============================================
		//=====Open graph file to read (END)
		//===========================================

		//=============================================
		//=========OPEN Z matrix pre file to read (START)
		//===============================================
		if (t > 0){
			strcpy(cprefilename, s.c_str());
			itostring(t - 1, tempstr, 10);
			strcat(cprefilename, tempstr);
		}
		else{
			strcpy(cprefilename, s.c_str());
			itostring(t, tempstr, 10);
			strcat(cprefilename, tempstr);
		}
		rfile3 = fopen(cprefilename, "r");
		if (rfile3 == NULL){
			printf("Could not find the community file\n");
			exit(1);
		}
		rv = fscanf(rfile3, "%d\n", &nodenum);
		if (rv != 1){
			printf("the format of matrix file %s is not correct", cprefilename);
			exit(2);
		}
		Readcommunity(Zprime, nodenum, rfile3);
		if (rfile3 != NULL) {
			fclose(rfile3);
			rfile3 = NULL;
		}
		//=============================================
		//=========OPEN Z matrix pre file to read (END)
		//===============================================

		//=============================================
		//=========OPEN Z matrix file to write (START)
		//===============================================
		strcpy(cfilename, s.c_str());
		itostring(t, tempstr, 10);
		strcat(cfilename, tempstr);
		wfile = fopen(cfilename, "w");
		if (wfile == NULL){
			printf("could not open matrix file %s to write", cfilename);
			exit(2);
		}
		fprintf(wfile, "%d\n", nodenum);
		//=============================================
		//=========OPEN Z matrix file to write (END)
		//===============================================
		Z = Zprime;
		do{
			Z.Gettransposesqure(B);
			a2 = (1 + sqrt(4 * a1*a1 + 1)) / 2;
			a = (a2 + a1 - 1) / a2;
			if (isauto == true){
				L = ComputeL(nodenum, m);
				alpha = 1.0 / L;
				if (iter%printstep == 0){
					//printf("L constant value is %.10lf\n", L);
					printf("stepsize value now is %.10lf\n", alpha*a);
				}
			}
			for (i = 0; i < nodenum; i++){
				if (isauto == false)
					updateZauto(Z, G, i, lambda, B, Zprime.matrix[i], 1.0);
				else
					updateZauto(Z, G, i, lambda, B, Zprime.matrix[i], a);
			}
			error = Fnormfast(G, Z, nodenum);
			if (iter == 0)
				preerror = nodenum;
			epsilo = (preerror - error) / nodenum;
			if (iter%printstep == 0)
				printf("%d\t%f\t%.10lf\n", iter, error, epsilo);
			preerror = error;
			a1 = a2;
			iter++;
		} while (iter < maxiter&&epsilo>0.000000001);
		Vmin = 0;
		Vmax = nodenum - 1;
		Z.Writetofile(wfile, G);
		if (wfile != NULL) {
			fclose(wfile);
			wfile = NULL;
		}
		//==========================================================
		//==============Release Used Memory  (START)================
		//==========================================================
		Z.clear();
		Zprime.clear();
		releasegraphmemory(G, nodenum);
		curMemPos = precurMemStart;
		curMemEnd = precurMemEnd;
		curBlk = precurblk;

		if (G != NULL){
			free(G);
			G = NULL;
		}
		Z.deletemem();
		Zprime.deletemem();
		//==========================================================
		//==============Release Used Memory  (END)================
		//==========================================================
		
	}
	if (tempvalue != NULL){
        delete []tempvalue;
		tempvalue = NULL;
	}
    
	if (B != NULL) {
		for(i = 0;i < m;i++){
			if (B[i] != NULL) {
				delete[]B[i];
				B[i] = NULL;
			}
		}
		delete[]B;
		B = NULL;
	}
	if (cfilename != NULL) {
		delete[]cfilename;
		cfilename = NULL;
	}
	if (cprefilename != NULL) {
		delete[]cprefilename;
		cprefilename = NULL;
	}
	if (tempstr != NULL) {
		delete[]tempstr;
		tempstr = NULL;
	}
	s.clear();
	//s.shrink_to_fit();
}

//===================================================
//Compute the BCGD Local algorithm in algorithm 2 
//where step size is an input parameter          (END)
//or step size is automatically determined by 
//lipschitz constant
//the full graph is store in memory
//===================================================
//===========================================================================================================================================
//============================================================================================================================================
//============================================================================================================================================
//===========================================================================================================================================


//===================================================
//Compute the BCGD Global algorithm in algorithm 1 
//where step size is an input parameter       (START)
//or the step size is automatically determined by 
//lipschitz constant
//the full graph and the full matrix can not be 
//loaded into the memory, instead, we sequential scan the graph
//and the matrix, load partial into the memory based on memorybound
//update them and save results back to disk
//===================================================

inline void BCGDGlobalSparse(int m, vector<char *> &filenames, double lambda, double membound, string &prefix, int printstep, bool isauto, int maxnodenum){
	int i = 0;
	int j = 0;
	int t = 0;
	Node *G;
	SparseMatrix Z; //current temporal latent space embedding
	SparseMatrix Zprime; //previous latent space embedding
	SparseMatrix Znext; //next time stamp latent space embedding
	int nodenum = 0;
	int iter = 0;
	int gnodenum = 0;
	string s("Zmatrix");
	s = prefix + s;
	char *cfilename = new char[2048];
	char *cprefilename = new char[2048];
	char *cnextfilename = new char[2048];
	char *tempstr = new char[2048];
	double **B = new double*[m];
	for (i = 0; i < m; i++){
		B[i] = new double[m];
	}
	double error;
	double preerror;
	double epsilo;
	FILE *rFile;
	//======Init memory(start)=======//
    InitTempSubmemory(maxnodenum);
	double staticsize = sizeof(double)*maxnodenum;
	staticsize += (sizeof(int)*maxnodenum * 2);
	staticsize=staticsize / 1024;
	if (membound <= staticsize){
		cout << "Please increase the membound by " << staticsize << endl;
		exit(2);
	}
	membound -= staticsize;
    //======Init memory(end)========//
	//=============================================================
	//Random Initialize the Zmatrix for each time stamp (START)
	//==============================================================
	for (t = 0; t<(int)filenames.size(); t++){
		strcpy(cfilename, s.c_str());
		itostring(t, tempstr, 10);
		strcat(cfilename, tempstr);
		InitZ(m, true, filenames.at(t), cfilename, membound);
	}
    releaseInitZmemory();
	cout << "finish initialization" << endl;
	//=============================================================
	//Random Initialize the Zmatrix for each time stamp (end)
	//==============================================================
	FILE *rfile3 = NULL;
	FILE *rfile2 = NULL;
	FILE *rfile4 = NULL;
	FILE *wfile = NULL;
	double a1 = 1;
	double a2;
	double a;
	do{
		error = 0;
		a2 = (1 + sqrt(4 * a1*a1 + 1)) / 2;
		a = (a2 + a1 - 1) / a2;
		for (t = 0; t<(int)filenames.size(); t++){
			///////////////////////////////////////////////////////////////////
			//load partial graph into memory based on 
			//the bounded memory size  (START)
			//////////////////////////////////////////////////////////////////
			rFile = fopen(filenames.at(t), "r");
			if (rFile == NULL){
				printf("Could not find the data file\n");
				exit(1);
			}
			int rv = fscanf(rFile, "%d\n", &nodenum);
			if (rv != 1){
				printf("the format of graph file %s is not correct", filenames.at(t));
				exit(2);
			}
			wfile = fopen("temp.txt", "w");
			if (wfile == NULL){
				printf("could not write result to disk\n");
				exit(1);
			}
			fprintf(wfile, "%d\n", nodenum);
			strcpy(cfilename, s.c_str());
			itostring(t, tempstr, 10);
			strcat(cfilename, tempstr);
			rfile2 = fopen(cfilename, "r");
			if (rfile2 == NULL){
				printf("could not open file %s to read\n", cfilename);
				exit(2);
			}
			rv = fscanf(rfile2, "%d\n", &nodenum);
			if (rv != 1){
				printf("the format of matrix file %s is not correct", cfilename);
				exit(2);
			}
			totalfinish = -1;
			Vmin = nodenum;
			Vmax = 0;
			memset(node2subidx, -1, sizeof(int)*nodenum);
			//===================================================================
			//Load graph and Z matrix block by block into memory
			//Update Z matrix                                (START)
			//=======================================================================
			while (1){
                //remember the start position of memory block (start)/////
				char *precurMem2Start = curMemPos;
				char *precurMem2End = curMemEnd;
				int precurblk2 = curBlk;
                //remember the start position of memory block (end)/////
				gnodenum = 0;
				memset(isseednode, 'v', sizeof(char)*nodenum);
				ReadSeeNode(gnodenum, nodenum, 10, membound, rFile);
				ReadSubGraph(G, gnodenum, nodenum, rFile);
				//GraphNorm(G, gnodenum);
				if (iter % printstep == 0)
					printf("in memory seed set is [%d, %d]\n", Vmin, Vmax);
				///////////////////////////////////////////////////////////////////
				//load partial graph into memory based on 
				//the bounded memory size  (END)
				//////////////////////////////////////////////////////////////////
				//load corresponding Zt into memory (start)	
				Readsubcommunity(Z, gnodenum, nodenum, rfile2);
				//load corresponding Zt into memory (end)
				if (t == 0){
					Zprime = Z;
				}
				else{
					strcpy(cprefilename, s.c_str());
					itostring(t - 1, tempstr, 10);
					strcat(cprefilename, tempstr);
					rfile3 = fopen(cprefilename, "r");
					if (rfile3 == NULL){
						printf("could not open %s file to read\n", cprefilename);
						exit(2);
					}
					rv = fscanf(rfile3, "%d\n", &nodenum);
					if (rv != 1){
						printf("the format of matrix file %s is not correct", cprefilename);
						exit(2);
					}
					Readsubcommunity(Zprime, gnodenum, nodenum, rfile3);
					if (rfile3 != NULL) {
						fclose(rfile3);
						rfile3 = NULL;
					}
					
				}
				if (t == filenames.size() - 1){
					Znext = Z;
				}
				else{
					strcpy(cnextfilename, s.c_str());
					itostring(t + 1, tempstr, 10);
					strcat(cnextfilename, tempstr);
					rfile4 = fopen(cnextfilename, "r");
					if (rfile4 == NULL){
						printf("could not open %s file to read\n", cnextfilename);
						exit(2);
					}
					rv = fscanf(rfile4, "%d\n", &nodenum);
					if (rv != 1){
						printf("the format of matrix file %s is not correct", cnextfilename);
						exit(2);
					}
					Readsubcommunity(Znext, gnodenum, nodenum, rfile4);
					if (rfile4 != NULL) {
						fclose(rfile4);
						rfile4 = NULL;
					}
					
				}
				Z.Gettransposesqure(B);
				if (isauto==true)
					alpha = 1.0 / ComputeL(nodenum,m);
				if (iter % printstep == 0)
					printf("step size now is %.10lf\n", alpha*a);
				for (i = 0; i<gnodenum; i++){
					if (isauto == true)
						updateZautofull(Z, G, i, lambda, B, Zprime.matrix[i], Znext.matrix[i],a);
					else
						updateZautofull(Z, G, i, lambda, B, Zprime.matrix[i], Znext.matrix[i],1.0);
				}
				Zprime.clear();
				////////////////////////////////////////////////////////////
				Z.Writetofile(wfile, G);
				error += Fnormfast(G, Z, gnodenum);
				//==========================================================
				//==============Release Used Memory  (START)================
				//==========================================================
				releasegraphmemory(G, gnodenum);
				Z.clear();
                Znext.clear();
                //reset memory position (start)///////
				curMemPos = precurMem2Start;
				curMemEnd = precurMem2End;
				curBlk = precurblk2;

                //reset memory position (end)////////
				if (G != NULL){
					free(G);
					G = NULL;
				}
				Z.deletemem();
				Zprime.deletemem();
                Znext.deletemem();
				Vmin = nodenum;
				totalfinish = Vmax;
				if (Vmax == nodenum - 1)
					break;
				//==========================================================
				//==============Release Used Memory  (END)================
				//==========================================================
			}//end of load partial data into memory and process	
			//===================================================================
			//Load graph and Z matrix block by block into memory
			//Update Z matrix                                (START)
			//=======================================================================
			if (wfile != NULL) {
				fclose(wfile);
				wfile = NULL;
			}
			if (rFile != NULL) {
				fclose(rFile);
				rFile = NULL;
			}
			if (rfile2 != NULL) {
				fclose(rfile2);
				rfile2 = NULL;
			}
			//replace Zmatrixt with temp.txt
			Replacefile(cfilename, "temp.txt");
		}
		if (iter == 0)
			preerror = nodenum*filenames.size();
		epsilo = (preerror - error) / nodenum;
		if (iter%printstep==0)
			printf("%d\t%lf\t%.10lf\n", iter, error, epsilo);
		preerror = error;
		iter++;
		a1 = a2;
	} while (iter<maxiter&&epsilo>0.000000001);
    releaseTempSubmemory();
	if (cfilename != NULL) {
		delete[]cfilename;
		cfilename = NULL;
	}
	if (cprefilename != NULL) {
		delete []cprefilename;
		cprefilename = NULL;
	}
	if (cnextfilename != NULL) {
		delete[]cnextfilename;
		cnextfilename = NULL;
	}
	if (B != NULL) {
		for(i=0;i<m;i++){
			if (B[i] != NULL) {
				delete[]B[i];
				B[i]=NULL;
			}
		}
		delete[]B;
		B = NULL;
	}
	if (tempstr != NULL) {
		delete[]tempstr;
		tempstr = NULL;
	}
}

//===================================================
//Compute the BCGD Global algorithm in algorithm 1 
//where step size is an input parameter       (END)
//or the step size is automatically determined by 
//lipschitz constant
//the full graph and the full matrix can not be 
//loaded into the memory, instead, we sequential scan the graph
//and the matrix, load partial into the memory based on memorybound
//update them and save results back to disk
//===================================================

//===========================================================================================================================================
//============================================================================================================================================
//============================================================================================================================================
//===========================================================================================================================================
//============================================================================================================================================
//============================================================================================================================================
//===========================================================================================================================================
//============================================================================================================================================
//============================================================================================================================================

//===================================================
//Compute the BCGD Global algorithm in algorithm 1 
//where step size is an input parameter       (START)
//or the step size is automatically determined by 
//lipschitz constant
//the full graph is stored in the memory
//===================================================

inline void BCGDGlobalFull (int m, vector<char *> &filenames, double lambda,string &prefix, int printstep, bool isauto, int maxnodenum){
	int i = 0;
	int j = 0;
	int t = 0;
	Node *G;
	SparseMatrix Z;
	SparseMatrix Zprime;
	SparseMatrix Znext;
	int nodenum = 0;
	int iter = 0;
	string s("Zmatrix");
	s = prefix + s;
	char *cfilename = new char[2048];
	char *cprefilename = new char[2048];
	char *tempstr = new char[2048];
	char *cnextfilename = new char[2048];
	double **B = new double*[m];
	for (i = 0; i < m; i++){
		B[i] = new double[m];
	}
	double error;
	double preerror;
	double epsilo;
	FILE *rFile = NULL;
    InitTempFullmemory(maxnodenum);
	//=============================================================
	//Random Initialize the Zmatrix for each time stamp (START)
	//==============================================================
	for (t = 0; t<(int)filenames.size(); t++){
		strcpy(cfilename, s.c_str());
		itostring(t, tempstr, 10);
		strcat(cfilename, tempstr);
		InitZfull(m, true, filenames.at(t), cfilename);
	}
    releaseInitZmemory();
	cout << "finish initialization" << endl;
	//=============================================================
	//Random Initialize the Zmatrix for each time stamp (end)
	//==============================================================
	FILE *rfile3 = NULL;
	FILE *rfile2 = NULL;
	FILE *rfile4 = NULL;
	FILE *wfile = NULL;
	double a1 = 1;
	double a2;
	double a;
	do{
		error = 0;
		a2 = (1 + sqrt(4 * a1*a1 + 1)) / 2;
		a = (a2 + a1 - 1) / a2;
		for (t = 0; t < (int)filenames.size(); t++){
			char *precurMemStart = curMemPos;
			char *precurMemEnd = curMemEnd;
			int precurblk = curBlk;
			//==============================================
			//load graph at time t into memory (start)
			//=============================================
			rFile = fopen(filenames.at(t), "r");
			if (rFile == NULL){
				printf("Could not find the data file\n");
				exit(1);
			}
			int rv = fscanf(rFile, "%d\n", &nodenum);
			if (rv != 1){
				printf("the format of graph file %s is not correct", filenames.at(t));
				exit(2);
			}
			ReadGraph(G, nodenum, rFile);
			if (rFile != NULL) {
				fclose(rFile);
				rFile = NULL;
			}
			//==============================================
			//load graph at time t into memory (end)
			//==============================================

			//==============================================
			//load community matrix Zprime into memory (start)
			//===============================================
			if (t > 0){
				strcpy(cprefilename, s.c_str());
				itostring(t - 1, tempstr, 10);
				strcat(cprefilename, tempstr);
			}
			else{
				strcpy(cprefilename, s.c_str());
				itostring(t, tempstr, 10);
				strcat(cprefilename, tempstr);
			}
			if (t == filenames.size() - 1){
				strcpy(cnextfilename, s.c_str());
				itostring(t, tempstr, 10);
				strcat(cnextfilename, tempstr);
			}
			else{
				strcpy(cnextfilename, s.c_str());
				itostring(t+1, tempstr, 10);
				strcat(cnextfilename, tempstr);
			}
			rfile3 = fopen(cprefilename, "r");
			rv = fscanf(rfile3, "%d\n", &nodenum);
			if (rv != 1){
				printf("the format of matrix file %s is not correct", cprefilename);
				exit(2);
			}
			Readcommunity(Zprime, nodenum, rfile3);
			if (rfile3 != NULL) {
				fclose(rfile3);
				rfile3 = NULL;
			}
			rfile4 = fopen(cnextfilename, "r");
			rv = fscanf(rfile4, "%d\n", &nodenum);
			if (rv != 1){
				printf("the format of matrix file %s is not correct", cnextfilename);
				exit(2);
			}
			Readcommunity(Znext, nodenum, rfile4);
			if (rfile4 != NULL) {
				fclose(rfile4);
				rfile4 = NULL;
			}
			//==============================================
			//load community matrix Zprime into memory (start)
			//===============================================

			//==============================================
			//load community matrix Z into memory (start)
			//===============================================
			strcpy(cfilename, s.c_str());
			itostring(t, tempstr, 10);
			strcat(cfilename, tempstr);
			rfile2 = fopen(cfilename, "r");
			rv = fscanf(rfile2, "%d\n", &nodenum);
			if (rv != 1){
				printf("the format of matrix file %s is not correct", cfilename);
				exit(2);
			}
			Readcommunity(Z, nodenum, rfile2);
			if (rfile2 != NULL) {
				fclose(rfile2);
				rfile2 = NULL;
			}
			//==============================================
			//load community matrix Z into memory (end)
			//===============================================

			//==============================================
			//write community matrix Z back into disk (start)
			//===============================================
			wfile = fopen(cfilename, "w");
			if (wfile == NULL){
				printf("could not write result to disk\n");
				exit(1);
			}
			fprintf(wfile, "%d\n", nodenum);
			//==============================================
			//write community matrix Z back into disk (end)
			//===============================================
			Z.Gettransposesqure(B);
			if (isauto == true)
				alpha = 1.0 / ComputeL(nodenum,m);
			if (iter % printstep == 0)
				printf("step size now is %.10lf\n", alpha);
			for (i = 0; i < nodenum; i++){
				if (isauto == true)
					updateZautofull(Z, G, i, lambda, B, Zprime.matrix[i], Znext.matrix[i], a);
				else
					updateZautofull(Z, G, i, lambda, B, Zprime.matrix[i], Znext.matrix[i],1.0);
			}
			Zprime.clear();
			////////////////////////////////////////////////////////////
			Vmin = 0;
			Vmax = nodenum - 1;
			Z.Writetofile(wfile, G);
			error += Fnormfast(G, Z, nodenum);
			//==========================================================
			//==============Release Used Memory  (START)================
			//==========================================================
			releasegraphmemory(G, nodenum);
			Z.clear();
            Znext.clear();
			//reset memory (start)/////
			curMemPos = precurMemStart;
			curMemEnd = precurMemEnd;
			curBlk = precurblk;

            //reset memory (end)/////
			if (G != NULL){
				free(G);
				G = NULL;
			}
			Z.deletemem();
			Zprime.deletemem();
			Znext.deletemem();
			//==========================================================
			//==============Release Used Memory  (END)================
			//==========================================================
			if (wfile != NULL) {
				fclose(wfile);
				wfile = NULL;
			}
		}
		if (iter == 0)
			preerror = nodenum*filenames.size();
		epsilo = (preerror - error) / nodenum;
		if (iter%printstep == 0)
			printf("%d\t%f\t%lf\n", iter, error, epsilo);
		preerror = error;
		iter++;
		a1 = a2;
	} while (iter<maxiter&&epsilo>0.000000001);
    releaseTempSubmemory();
	if (cfilename != NULL) {
		delete[]cfilename;
		cfilename = NULL;
	}
	if (cprefilename != NULL) {
		delete[]cprefilename;
		cprefilename = NULL;
	}
	if (cnextfilename != NULL) {
		delete[]cnextfilename;
		cnextfilename = NULL;
	}
	if (B != NULL) {
		for(i=0;i<m;i++){
			if (B[i] != NULL) {
				delete []B[i];
				B[i] = NULL;
			}
		}
		delete[]B;
		B = NULL;
	}
	if (tempstr != NULL) {
		delete[]tempstr;
		tempstr = NULL;
	}
}

//===================================================
//Compute the BCGD Global algorithm in algorithm 1 
//where step size is an input parameter       (END)
//or the step size is automatically determined by 
//lipschitz constant
//the full graph is store in the memory
//===================================================
#endif
