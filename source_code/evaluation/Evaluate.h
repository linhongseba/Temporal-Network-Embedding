#ifndef Evaluate_h_
#define Evaluate_h_
#include"../main/function.h"
#include"../main/IOfunction.h"
#include"../main/SparseMatrix.h"
#include"../main/Updatecommunity.h"

/*
This file implements the link prediction evaluation algorithm

/*@author: linhong (linhong.seba.zhu@gmail.com)
*/
inline void AA(Node *G, int u, int *&AAtarget, double *&AAscore, int &pos, int N){
	pos = 0;
	for (int i = 0; i < N; i++){
		AAscore[i] = 0;
	}
	int w;
	int v;
	int cur;
	double value;
	for (int j = 0; j < G[u].deg; j++){
		w = G[u].nbv[j];
		for (int k = 0; k < G[w].deg; k++){
			v = G[w].nbv[k];
			if (v != u){
				cur = binarysearch(AAtarget, 0, pos, v);
				if (G[w].deg <= 1)
					value = 0;
				else
					value = 1 / (log(G[w].deg));
				if (cur < 0){
					insert(AAtarget, -cur - 1, v, pos);
					insert(AAscore, -cur - 1, value, pos);
					pos++;
				}
				else{
					AAscore[cur] += value;
				}
			}
		}
	}
}
inline double Linkfunction(SparseMatrix &Z, int i, int j, int type,double avg1,double avg2){
	double res=0;
	if (type == 1){
		//i j was linked in previous snapshot
		res = Z.Rowdotproduct2(i, j);
		if (res > avg1){
			res = res;
		}
		else{
			res = min(avg2/2,res - avg2); //reconstruction error large which might indicates deleted links
		}
	}
	else{
		//i j was not linked in previous snapshot
		res = Z.Rowdotproduct2(i, j);
		if (res > avg2&&res<avg1){ //reconstruction error large which might indicates new links
			res = min(1.0,res+avg1-avg2);
		}
		else{
			res = res;
		}
	}
	return res;
}
inline double Evaluateerror(char *graphfile, char *Zfile){
	char *precurMem2 = curMemPos;
	int precurblk2 = curBlk;
	char* preendMem2 = curMemEnd;
	FILE *rfile = fopen(graphfile, "r");
	FILE *rfile2 = fopen(Zfile, "r");
	if (rfile == NULL){
		printf("could not open graph file to read\n");
		exit(1);
	}
	if (rfile2 == NULL){
		printf("could not open community file to read\n");
		exit(1);
	}
	Node *G;
	SparseMatrix Z;
	int nodenum;
	int rv = fscanf(rfile, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of graph file is not correct!");
		exit(2);
	}
	rv = fscanf(rfile2, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of graph file is not correct!");
		exit(2);
	}
	ReadGraph(G, nodenum, rfile);
	fclose(rfile);
	Readcommunity(Z, nodenum, rfile2);
	fclose(rfile2);
	/*for (int i = 0; i < 10; i++){
		Z.Printrow(i);
	}*/
	tempvalue = new double[nodenum];
	double avg = Getaverage(G, Z, nodenum);
	double navg;
	double nmin;
	double nmax;
	Getnonlink(G, Z, nodenum, nmin, nmax, navg);
	double error;
	if (nodenum > 10000)
		error = Fnormestimatefull(G,Z,nodenum,navg);
	else
		error = Fnormexactfull(G, Z, nodenum);
	cout << "mean square error is " << error << endl;
	cout << " the averge link probability is " << avg << endl;
	cout << "the average non-link probability is " << navg << endl;
	//cout << " the minimum non-link probability is " << nmin << endl;
	//cout << "the maximum non-link probability is " << nmax << endl; 
	delete[]tempvalue;
	tempvalue = NULL;
	Z.clear();
	Z.deletemem();
	releasegraphmemory(G, nodenum);
	if (G != NULL){
		free(G);
		G = NULL;
	}
	curMemPos = precurMem2;
	curBlk = precurblk2;
	curMemEnd = preendMem2;
	return error;
}
/*
Prediction:
using Z(u).Z(v)T to predict the probability of node pair u, v is linked
In addition, we compare the reconstruction error for train graph to tune the link
probability 
*/
inline void Predictionlinks(char *Zfile, char *pairfile, char *traingraphfile, int ltype){
	char *precurMem2 = curMemPos;
	int precurblk2 = curBlk;
	char* preendMem2 = curMemEnd;
	FILE *rfile1 = fopen(traingraphfile, "r");
	FILE *rfile2 = fopen(Zfile, "r");
	FILE *rfile3 = fopen(pairfile, "r");
	if (rfile1 == NULL){
		printf("could not open the graph file to read\n");
		exit(2);
	}
	if (rfile2 == NULL){
		printf("could not open latent space file to read\n");
		exit(2);
	}
	if (rfile3 == NULL){
		printf("could not open test pair file to read\n");
		exit(2);
	}
	////////////////////////////////////////////////
	//read latent space file (start)
	///////////////////////////////////////////////
	SparseMatrix Z;
	int nodenum;
	int rv = fscanf(rfile2, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of latent space file is not correct\n");
		exit(2);
	}
	Readcommunity(Z, nodenum, rfile2);
	fclose(rfile2);
	////////////////////////////////////////////////
	//read latent space file (end)
	///////////////////////////////////////////////

	////////////////////////////////////////////////
	//read train graph file (start)
	///////////////////////////////////////////////
	Node *Gpre;
	rv = fscanf(rfile1, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of latent space file is not correct\n");
		exit(2);
	}
	ReadGraph(Gpre, nodenum, rfile1);
	fclose(rfile1);
	////////////////////////////////////////////////
	//read train graph file (end)
	///////////////////////////////////////////////
	FILE *wfile = fopen("link.txt", "w");
	if (wfile == NULL){
		printf("could not open file to write\n");
		exit(1);
	}
	int i;
	int j;
	int gt;
	int pregt;
	double value;
	/*
	avg1: the average value of linked pairs
	avg2: the average value of non-linked pairs
	*/
	double avg1 = Getaverage(Gpre, Z, nodenum);
	double minv;
	double maxv;
	double avg2;
	Getnonlink(Gpre, Z, nodenum, minv, maxv, avg2);
	cout << "average of linked and non-linked pairs" << avg1 << "\t" << avg2 << endl;
	while (feof(rfile3) == false){
		int c = fscanf(rfile3, "%d\t%d\t%d\t%d\n", &i, &j, &pregt, &gt);
		if (c != 4){
			printf("the format of test pair file is not correct!\n");
			exit(2);
		}
		if (ltype == 1)
			value = Linkfunction(Z, i, j, pregt, avg1, avg2);
		else
			value = Z.Rowdotproduct2(i, j);
		fprintf(wfile, "%lf\t%d\n", value, gt);
	}
	fclose(rfile3);
	if (wfile != NULL)
		fclose(wfile);
	Z.clear();
	Z.deletemem();
	releasegraphmemory(Gpre,nodenum);
	if (Gpre != NULL){
		free(Gpre);
		Gpre = NULL;
	}
	curMemPos = precurMem2;
	curBlk = precurblk2;
	curMemEnd = preendMem2;
}
void Writeresults(vector<double>*& results, int timepos, char *pairfile){
	FILE *rfile3 = fopen(pairfile, "r");
	if (rfile3 == NULL){
		printf("could not open test pair file to read\n");
		exit(2);
	}
	FILE *wfile = fopen("link.txt", "w");
	if (wfile == NULL){
		printf("could not open file to write\n");
		exit(1);
	}
	int i;
	int j;
	int gt;
	int pregt;
	int linenum = 0;
	double avg = 0;
	double minsim = 0;
	while (feof(rfile3) == false){
		int c = fscanf(rfile3, "%d\t%d\t%d\t%d\n", &i, &j, &pregt,&gt);
		if (c != 4){
			printf("the format of test pair file is not correct!\n");
			exit(2);
		}
		avg = 0;
		minsim = 1.0;
		for (int t = 0; t < timepos; t++){
			//fprintf(wfile, "%lf\t", results[t].at(linenum));
			if (results[t].at(linenum) < minsim)
				minsim = results[t].at(linenum);
			avg += results[t].at(linenum);
		}
		avg /= timepos;
		fprintf(wfile, "%lf\t", avg);
		fprintf(wfile, "%d\n", gt);
		linenum++;
	}
	fclose(rfile3);
	fclose(wfile);
}
inline void Predictionlinks(char *Zfile, char *pairfile, vector<double> &res, char *traingraphfile, int ltype){
	char *precurMem2 = curMemPos;
	int precurblk2 = curBlk;
	char* preendMem2 = curMemEnd;
	FILE *rfile1 = fopen(traingraphfile, "r");
	FILE *rfile2 = fopen(Zfile, "r");
	FILE *rfile3 = fopen(pairfile, "r");
	if (rfile1 == NULL){
		printf("could not open train graph file to read\n");
		exit(2);
	}
	if (rfile2 == NULL){
		printf("could not open latent space file to read\n");
		exit(2);
	}
	if (rfile3 == NULL){
		printf("could not open test pair file to read\n");
		exit(2);
	}
	////////////////////////////////////////////////
	//read latent space file (start)
	///////////////////////////////////////////////
	SparseMatrix Z;
	int nodenum;
	int rv = fscanf(rfile2, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of latent space file is not correct\n");
		exit(2);
	}
	Readcommunity(Z, nodenum, rfile2);
	fclose(rfile2);
	////////////////////////////////////////////////
	//read latent space file (end)
	///////////////////////////////////////////////

	////////////////////////////////////////////////
	//read train graph file (start)
	///////////////////////////////////////////////
	Node *Gpre;
	rv = fscanf(rfile1, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of latent space file is not correct\n");
		exit(2);
	}
	ReadGraph(Gpre, nodenum, rfile1);
	fclose(rfile1);
	////////////////////////////////////////////////
	//read train graph file (end)
	///////////////////////////////////////////////
	int i;
	int j;
	int pregt;
	int gt;
	double value;
	double avg1 = Getaverage(Gpre, Z, nodenum);
	double avg2;
	double minv;
	double maxv;
	Getnonlink(Gpre, Z, nodenum, minv, maxv, avg2);
	while (feof(rfile3) == false){
		int c = fscanf(rfile3, "%d\t%d\t%d\t%d\n", &i, &j, &pregt,&gt);
		if (c != 4){
			printf("the format of test pair file is not correct!\n");
			exit(2);
		}
		if (ltype==2)
			value = Z.Rowdotproduct2(i, j);
		else
			value=Linkfunction(Z, i, j, pregt, avg1, avg2);
		res.push_back(value);
	}
	fclose(rfile3);
	Z.clear();
	Z.deletemem();
	releasegraphmemory(Gpre, nodenum);
	if (Gpre != NULL){
		free(Gpre);
		Gpre = NULL;
	}
	curMemPos = precurMem2;
	curBlk = precurblk2;
	curMemEnd = preendMem2;
}
inline void EvaluateRep(char *graphfile, char *pregraphfile, char *zfile){
	char *precurMem2 = curMemPos;
	int precurblk2 = curBlk;
	char* preendMem2 = curMemEnd;
	FILE *rfile = fopen(graphfile, "r");
	FILE *rfile2 = fopen(pregraphfile, "r");
	FILE *rfile3 = fopen(zfile, "r");
	if (rfile == NULL){
		printf("could not open graph file to read\n");
		exit(1);
	}
	if (rfile2 == NULL){
		printf("could not open previous graph file to read\n");
		exit(1);
	}
	if (rfile3 == NULL){
		printf("could not open latent space file to read\n");
		exit(1);
	}
	Node *G;
	Node *Gpre;
	SparseMatrix Z;
	int nodenum;
	int rv = fscanf(rfile, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of graph file is not correct!");
		exit(2);
	}
	rv = fscanf(rfile2, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of graph file is not correct!");
		exit(2);
	}
	rv = fscanf(rfile3, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of community file is not correct!");
		exit(2);
	}
	ReadGraph(G, nodenum, rfile);
	fclose(rfile);
	ReadGraph(Gpre, nodenum, rfile2);
	fclose(rfile2);
	Readcommunity(Z, nodenum, rfile3);
	fclose(rfile3);
	char *linkfile = new char[500];
	strcpy(linkfile, graphfile);
	strcat(linkfile, "graphlink.txt");
	FILE *wfile = fopen(linkfile, "w");
	if (wfile == NULL){
		printf("could not open file to write\n");
		exit(1);
	}
	int i;
	int j;
	int idx;
	int gt;
	int pvalue;
	double value;
	for (i = 0; i < nodenum; i++){
		for (j = i+1; j < nodenum; j++){
			value = Z.Rowdotproduct2(i, j);
			idx = binarysearch(G[i].nbv, 0, G[i].deg, j);
			if (idx >= 0){
				gt = 1;
			}
			else{
				gt = 0;
			}
			idx = binarysearch(Gpre[i].nbv, 0, Gpre[i].deg, j);
			if (idx >= 0){
				pvalue = 1;
			}
			else{
				pvalue = 0;
			}
			if (pvalue == 1 && gt == 1)
				fprintf(wfile, "%lf\t1\n", value); //repeated links
			else if (pvalue == 0 && gt == 1) //new links
				fprintf(wfile, "%lf\t2\n", value);
			else if (pvalue == 1 && gt == 0)//deleted links
				fprintf(wfile, "%lf\t3\n", value);
			else
				fprintf(wfile, "%lf\t4\n", value);//non-linked pairs
		}
	}
	fclose(wfile);
	releasegraphmemory(G, nodenum);
	releasegraphmemory(Gpre, nodenum);
	Z.clear();
	curMemPos = precurMem2;
	curBlk = precurblk2;
	curMemEnd = preendMem2;
	Z.deletemem();
	if (G != NULL){
		free(G);
		G = NULL;
	}
	if (Gpre != NULL){
		free(Gpre);
		Gpre = NULL;
	}
}
inline void Evaluateallinks(char *graphfile, char *Zfile, const char * prefix){
	char *precurMem2 = curMemPos;
	int precurblk2 = curBlk;
	char* preendMem2 = curMemEnd;
	FILE *rfile = fopen(graphfile, "r");
	FILE *rfile2 = fopen(Zfile, "r");
	if (rfile == NULL ){
		printf("could not open graph file to read\n");
		exit(1);
	}
	if (rfile2 == NULL){
		printf("could not open community file to read\n");
		exit(1);
	}
	Node *G;
	SparseMatrix Z;
	int nodenum;
	int rv=fscanf(rfile, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of graph file is not correct!");
		exit(2);
	}
	rv=fscanf(rfile2, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of graph file is not correct!");
		exit(2);
	}
	ReadGraph(G, nodenum, rfile);
	fclose(rfile);
	Readcommunity(Z, nodenum, rfile2);
	fclose(rfile2);
	int idx;
	int i;
	int j;
	int pos = 0;
	char *linkfile = new char[500];
	strcpy(linkfile, graphfile);
	strcat(linkfile, prefix);
	strcat(linkfile, "link.txt");
	FILE *wfile = fopen(linkfile, "w");
	if (wfile == NULL){
		printf("could not open file to write\n");
		exit(1);
	}
	double value;
	for (i = 0; i < nodenum; i++){
		for (j = i+1; j < nodenum; j++){
			value = Z.Rowdotproduct2(i, j);
			//printf("%d\t%d\t%lf\n", iindex,yindex,mmvalue);
			idx = binarysearch(G[i].nbv, 0, G[i].deg, j);
			if (idx >= 0){
				fprintf(wfile, "%lf\t1\n", value);
			}
			else{
				//if (value>0.00007)
				fprintf(wfile, "%lf\t0\n", value);
			}
		}
	}
	fclose(wfile);
	delete[]linkfile;
	//==========================================================
	//==============Release Used Memory  (START)================
	//==========================================================
	releasegraphmemory(G, nodenum);
	Z.clear();
	curMemPos = precurMem2;
	curBlk = precurblk2;
	curMemEnd = preendMem2;
	if (G != NULL){
		free(G);
		G = NULL;
	}
	Z.deletemem();
}

#endif