#ifndef Updatecommunity_h_
#define Updatecommunity_h_
#include"SparseMatrix.h"
#include"IOfunction.h"
/*
This file implements the sub functions in BCGD algorithm for dynamic ovelapping community detection algorithm

/*@author: linhong (linhong.seba.zhu@gmail.com)
*/
///----------------------------------------------------------------------//
//update community for node u based on Equ. 5
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------------
//update community for node u based only the edge terms in Equ. 5
//----------------------------------------------------------------------------------
inline void updatefastZ(SparseMatrix &Z, Node *&G, int u, double lambda, Row &Zuprime){
	int i = 0;
	int v;
	double c1;  //Wt(u, v)-Z(u).Z(v)
	double c2 = 0;  //sum_{v}Zt(v).Zt(v)
	int pos = 0;
	for (i = 0; i < G[u].deg; i++){
		v = G[u].nbv[i];
		v = node2subidx[v];
		c1 = G[u].weight[i] - Z.Rowdotproduct2(u, v);
		tempvalue[pos] = c1;
		pos++;
		c2 += Z.Rowdotproduct2(v, v);
	}
	//c=-2alpha[Fnorm(Z)*2-Z(u).Z(u)-sum_{v}Z(v).Z(v)]
	//double c = sumZ - Z.Rowdotproduct(u) - c2;
	//c = -c * 2 * alpha;
	//Z.RowsumMultiply(u, c);
	for (i = 0; i < G[u].deg; i++){
		v = G[u].nbv[i];
		v = node2subidx[v];
		if (abs(tempvalue[i]) > 0){
			Z.RowsumLeftEqual(u, v, tempvalue[i] * 2 * alpha);
		}
	}
	//finally add alpha\lambda Zt-1(u)
	Z.RowsumLeftEqual(u, Zuprime, alpha*lambda);
	Z.Rownorm2(u);
}

double Getaverage(Node *&G, SparseMatrix &Z, int N){
	double avg = 0;
	double count = 0;
	for (int i = 0; i < N; i++){
		for (int j = 0; j < G[i].deg; j++){
			int nb = G[i].nbv[j];
			avg += Z.Rowdotproduct2(i, nb);
			count++;
		}
	}
	avg /= count;
	return avg;
}
/*
Getnonlink: compute the average link probability for nonlinks

*/
inline double Getnonlink2(Node *&G, SparseMatrix &Z, int N){
	int count = 0;
	int a; 
	int b;
	int cur;
	double avgvalue = 0;
	int randcount = 0;
	while (randcount < 100000||count<10000){
		a = rand();
		b = rand();
		a = a%N;
		b = b%N;
		cur = binarysearch(G[a].nbv, 0, G[a].deg, G[b].vid);
		if (cur < 0){
			avgvalue += Z.Rowdotproduct2(a, b);
			count++;
		}
		randcount++;
	}
	if (count>0)
		avgvalue /= count;
	return avgvalue;
}
inline double Getnonlink(Node *&G, SparseMatrix &Z, int N){
	double avgvalue;
	avgvalue = 0;
	int count = 0;
	double prob = 0.1;
	for (int i = 0; i < N; i++){
		if (count > 100000)
			break;
		if (G[i].deg > 0){
			int minidx = G[i].nbv[0];
			int maxidx = G[i].nbv[G[i].deg];
			for (int j = 0; j < minidx; j++){
				if (j > i){
					int a = rand();
					int b = rand();
					double p=0.1;
					if ((a+b)>0)
						p = (double)a / (a + b);
					if (p > prob)
						continue;
					double value = Z.Rowdotproduct2(i, j);
					avgvalue += value;
					count++;
				}
				if (count > 100000)
					break;

			}
			for (int j = maxidx + 1; j < N; j++){
				if (j > i){
					int a = rand();
					int b = rand();
					double p = 0.1;
					if ((a+b)>0)
						p = (double)a / (a + b);
					if (p > prob)
						continue;
					double value = Z.Rowdotproduct2(i, j);
					avgvalue += value;
					count++;
				}
				if (count > 100000)
					break;
			}
		}
	}
	if (count>0)
		avgvalue /= count;
	return avgvalue;
}
void Getnonlink(Node *&G, SparseMatrix &Z, int N, double &minvalue, double &maxvalue, double &avgvalue){
	minvalue = 1;
	maxvalue = 0;
	avgvalue = 0;
	int count = 0;
	double prob = 0.1;
	for (int i = 0; i < N; i++){
		if (count > 100000)
			break;
		if (G[i].deg > 0){
			int minidx = G[i].nbv[0];
			int maxidx = G[i].nbv[G[i].deg];
			for (int j = 0; j < minidx; j++){
				if (j > i){
					int a = rand();
					int b = rand();
					double p = (double)a / (a + b);
					if (p > prob)
						continue;
					double value = Z.Rowdotproduct2(i, j);
					if (value < minvalue)
						minvalue = value;
					if (value > maxvalue)
						maxvalue = value;
					avgvalue += value;
					count++;
				}
				if (count > 100000)
					break;
				
			}
			for (int j = maxidx + 1; j < N; j++){
				if (j > i){
					int a = rand();
					int b = rand();
					double p = (double)a / (a + b);
					if (p > prob)
						continue;
					double value = Z.Rowdotproduct2(i, j);
					if (value < minvalue)
						minvalue = value;
					if (value > maxvalue)
						maxvalue = value;
					avgvalue += value;
					count++;
				}
				if (count > 100000)
					break;
			}
		}
	}
	avgvalue /= count;
}
//---------------------------------------------------------------
//compute ||G-Z.ZT||_F^2
//---------------------------------------------------------------
inline double Fnormexactfull(Node *&G, SparseMatrix &Z, int N){
	double sum = 0;
	int i = 0;
	int j = 0;
	double value;
	int idx;
	for (i = 0; i<N; i++){
		for (j = 0; j<N; j++){
			value = Z.Rowdotproduct2(i, j);
			tempvalue[j] = -value;
		}
		for (j = 0; j<G[i].deg; j++){
			idx = G[i].nbv[j];
			tempvalue[idx] += G[i].weight[j];
		}
		for (j = 0; j<N; j++){
			if (j != i)
				sum += tempvalue[j] * tempvalue[j];
		}
	}
	sum = sqrt(sum);
	return sum;
}
inline double Fnormestimatefull(Node *&G, SparseMatrix &Z, int N,double avg){
	double sum = 0;
	int i = 0;
	int j = 0;
	double value;
	int idx;
	int m = 0;
	for (i = 0; i < N; i++){
		m += G[i].deg;
		for (j = 0; j < G[i].deg; j++){
			idx = G[i].nbv[j];
			value = Z.Rowdotproduct2(i, idx);
			sum += (G[i].weight[j] - value)*(G[i].weight[j] - value);
		}
	}
	sum += (N*avg*avg*N - m*avg*avg);
	sum = sqrt(sum);
	return sum;
}

//---------------------------------------------------------
//compute ||G-Z.Z^T||_F^2 while both G and Z is a subpart of 
//the graph Z and node community for only seed node
/////////////////////////////////////////////////////////////
inline double Fnormexact(Node *&G, SparseMatrix &Z, int N){
	double sum = 0;
	int i = 0;
	int j = 0;
	double value;
	int idx;
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			value = Z.Rowdotproduct2(i, j);
			tempvalue[j] = -value;
		}
		for (j = 0; j < G[i].deg; j++){
			idx = G[i].nbv[j];
			idx = node2subidx[idx];
			tempvalue[idx] += G[i].weight[j];
		}
		for (j = 0; j < N; j++){
			if (j != i)
				sum += tempvalue[j] * tempvalue[j];
		}
	}
	sum = sqrt(sum);
	return sum;
}

//-----------------------------------------------------------------
//compute ||G_ZZ^T||_F^2 where Z is a non-overlapping partitioning
//----------------------------------------------------------------
inline double Fnormexactfull(Node *&G, int *&partids, int N){
	double sum = 0;
	int i = 0;
	int j = 0;
	double value = 0;
	int idx;
	for (i = 0; i<N; i++){
		for (j = 0; j<N; j++){
			if (partids[i] == partids[j]){
				tempvalue[j] = 1;
			}
			else
				tempvalue[j] = 0;
		}
		for (j = 0; j<G[i].deg; j++){
			idx = G[i].nbv[j];
			tempvalue[idx] -= G[i].weight[j];
		}
		for (j = 0; j<N; j++){
			if (j != i)
				sum += tempvalue[j] * tempvalue[j];
		}
	}
	sum = sqrt(sum);
	return sum;
}

inline double Fnormexact(Node *&G, int *&partids, int N){
	double sum = 0;
	int i = 0;
	int j = 0;
	double value = 0;
	int idx;
	for (i = 0; i<N; i++){
		for (j = 0; j<N; j++){
			if (partids[G[i].vid] == partids[G[j].vid]){
				tempvalue[j] = 1;
			}
			else
				tempvalue[j] = 0;
		}
		for (j = 0; j<G[i].deg; j++){
			idx = G[i].nbv[j];
			idx = node2subidx[idx];
			tempvalue[idx] -= G[i].weight[j];
		}
		for (j = 0; j<N; j++){
			if (j != i)
				sum += tempvalue[j] * tempvalue[j];
		}
	}
	sum = sqrt(sum);
	return sum;
}

//-----------------------------------------------------------------
//compute only the (i, j) pairs that are edges in ||G_ZZ^T||_F^2 
//and where Z is a non-overlapping partitioning partids
//----------------------------------------------------------------
inline double Fnormfastfull(Node *&G, int *&partids, int N){
	double sum = 0;
	int i = 0;
	int j = 0;
	int idx;
	for (i = 0; i<N; i++){
		for (j = 0; j<G[i].deg; j++){
			idx = G[i].nbv[j];
			if (partids[i] == partids[idx]){
				sum += (G[i].weight[j] - 1)*(G[i].weight[j] - 1);
			}
			else
				sum += G[i].weight[j] * G[i].weight[j];
		}
	}
	sum = sqrt(sum);
	return sum;
}

inline double Fnormfast(Node *&G, int *&partids, int N){
	double sum = 0;
	int i = 0;
	int j = 0;
	int idx;
	for (i = 0; i<N; i++){
		for (j = 0; j<G[i].deg; j++){
			idx = G[i].nbv[j];
			if (partids[G[i].vid] == partids[idx]){
				sum += (G[i].weight[j] - 1)*(G[i].weight[j] - 1);
			}
			else
				sum += G[i].weight[j] * G[i].weight[j];
		}
	}
	sum = sqrt(sum);
	return sum;
}

//-----------------------------------------------------------------
//compute only the (i, j) pairs that are edges in ||G_ZZ^T||_F^2 
//
//----------------------------------------------------------------
inline double Fnormfast(Node *&G, SparseMatrix &Z, int N){
	double sum = 0;
	int i = 0;
	int j = 0;
	double value;
	//int m = 0;
	int idx;
	/*double avg;
	if (node2subidx == NULL)
		avg = Getnonlink(G, Z, N);
	else{
		avg = Getnonlink2(G, Z, N);
	}*/
	for (i = 0; i < N; i++){
		//m += G[i].deg;
		for (j = 0; j < G[i].deg; j++){
			idx = G[i].nbv[j];
			if (node2subidx!=NULL)
				idx = node2subidx[idx];
			value = G[i].weight[j];
			value -= Z.Rowdotproduct2(i, idx);
			sum += value * value;
		}
	}
	//sum += (N*avg*avg*N - m*avg*avg);
	sum = sqrt(sum);
	return sum;
}

//---------------------------------------------------------------------------
// Random init Z such that Z is the best among 300 iterations
//---------------------------------------------------------------------------
inline void InitZ(int m, bool isfast, char *&gfilename, char *&cfilename, double membound){
	int nodenum;
	FILE *rFile = fopen(gfilename,"r");
	if (rFile == NULL){
		printf("Could not find the data file\n");
		exit(1);
	}
	int rv = fscanf(rFile, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of graph file %s is not correct", gfilename);
		exit(2);
	}
	Node *G;
	double minvalue = nodenum * 5;
	Runtimecounter IORC;
	int j;
	int pid;
	double value;
	int gnodenum;
	for (int iter = 0; iter < 100; iter++){
		for (int i = 0; i < nodenum; i++){
			pid = rand() % m;
			tempidsize[i] = pid;
		}
		totalfinish = -1;
		Vmin = nodenum;
		Vmax = 0;
		memset(node2subidx, -1, sizeof(int)*nodenum);
		value = 0;
		while (1){
			//remember where we are in memory usage (START)
			char *precurMem2 = curMemPos;
			int precurblk2 = curBlk;
			char* preendMem2 = curMemEnd;
			//remember where we are in memory usage (END)
			gnodenum = 0;
			memset(isseednode, 'v', sizeof(char)*nodenum);
			ReadSeeNode(gnodenum, nodenum, 30, membound, rFile);
			ReadSubGraph(G, gnodenum, nodenum, rFile);
			if (isfast == true)
				value += Fnormfast(G, tempidsize, gnodenum);
			else
				value += Fnormexact(G, tempidsize, gnodenum);
			releasegraphmemory(G, gnodenum);
			curMemPos = precurMem2;
			curBlk = precurblk2;
			curMemEnd = preendMem2;
			if (G != NULL){
				free(G);
				G = NULL;
			}
			//==========================================================
			//==============Release Used Memory  (END)================
			//==========================================================
			//update in-memory seed set
			Vmin = nodenum;
			totalfinish = Vmax;
			if (Vmax == nodenum - 1)
				break;
		}
		if (value < minvalue){
			minvalue = value;
			memcpy(groupid, tempidsize, sizeof(int)*nodenum);
		}
	}
	if (rFile != NULL)
		fclose(rFile);
	//Write matrix to file;
	FILE *wfile = fopen(cfilename, "w");
	if (wfile == NULL){
		printf("could not open file %s to write\n",cfilename);
		exit(1);
	}
	outcurpos = outputbuffer;
	j = itostring(nodenum, outputbuffer, 10);
	outcurpos += j;
	*outcurpos = '\n';
	outcurpos++;
	for (int i = 0; i<nodenum; i++){
		if (outendpos - outcurpos <= 100){
			j = outcurpos - outputbuffer;
			IORC.start();
			fwrite(outputbuffer, sizeof(char), j, wfile);
			IORC.stop();
			iotime += IORC.GetRuntime();
			outcurpos = outputbuffer;
		}
		j = itostring(i, outcurpos, 10);
		outcurpos += j;
		*outcurpos = ',';
		outcurpos++;
		*outcurpos = '1';
		outcurpos++;
		*outcurpos = ':';
		outcurpos++;
		j = itostring(groupid[i], outcurpos, 10);
		outcurpos += j;
		*outcurpos = ',';
		outcurpos++;
		*outcurpos = '1';
		outcurpos++;
		*outcurpos = '\n';
		outcurpos++;
	}
	j = outcurpos - outputbuffer;
	fwrite(outputbuffer, sizeof(char), j, wfile);
	outcurpos = outputbuffer;
	fclose(wfile);
}

inline void InitZfull(int m, bool isfast, char *&gfilename, char *&cfilename){
	char *precurMem = curMemPos;
	int precurblk = curBlk;
	char* preendMem = curMemEnd;
	FILE *rFile = fopen(gfilename, "r");
	if (rFile == NULL){
		printf("Could not find the data file\n");
		exit(1);
	}
	int nodenum = 0;
	int rv = fscanf(rFile, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of graph file %s is not correct", gfilename);
		exit(2);
	}
	Node *G;
	ReadGraph(G, nodenum, rFile);
	//GraphNorm(G, nodenum);
	fclose(rFile);
	double minvalue = nodenum * 5;
	Runtimecounter IORC;
	int j;
	int pid;
	for (int iter = 0; iter<300; iter++){
		for (int i = 0; i<nodenum; i++){
			pid = rand() % m;
			tempidsize[i] = pid;
		}
		double value;
		if (isfast == true)
			value = Fnormfastfull(G, tempidsize, nodenum);
		else
			value = Fnormexactfull(G, tempidsize, nodenum);
		if (value<minvalue){
			minvalue = value;
			memcpy(groupid, tempidsize, sizeof(int)*nodenum);
		}
	}
	//Write matrix to file;
	FILE *wfile = fopen(cfilename, "w");
	if (wfile == NULL){
		printf("could not open file %s to write\n", cfilename);
		exit(1);
	}
	outcurpos = outputbuffer;
	j = itostring(nodenum, outputbuffer, 10);
	outcurpos += j;
	*outcurpos = '\n';
	outcurpos++;
	for (int i = 0; i<nodenum; i++){
		if (outendpos - outcurpos <= 100){
			j = outcurpos - outputbuffer;
			IORC.start();
			fwrite(outputbuffer, sizeof(char), j, wfile);
			IORC.stop();
			iotime += IORC.GetRuntime();
			outcurpos = outputbuffer;
		}
		j = itostring(i, outcurpos, 10);
		outcurpos += j;
		*outcurpos = ',';
		outcurpos++;
		*outcurpos = '1';
		outcurpos++;
		*outcurpos = ':';
		outcurpos++;
		j = itostring(groupid[i], outcurpos, 10);
		outcurpos += j;
		*outcurpos = ',';
		outcurpos++;
		*outcurpos = '1';
		outcurpos++;
		*outcurpos = '\n';
		outcurpos++;
	}
	j = outcurpos - outputbuffer;
	fwrite(outputbuffer, sizeof(char), j, wfile);
	outcurpos = outputbuffer;
	fclose(wfile);
	//==========================================================
	//==============Release Used Memory  (START)================
	//==========================================================
	releasegraphmemory(G, nodenum);
	curMemPos = precurMem;
	curBlk = precurblk;
	curMemEnd = preendMem;
	if (G != NULL){
		free(G);
		G = NULL;
	}
	//==========================================================
	//==============Release Used Memory  (END)================
	//==========================================================
}




///----------------------------------------------------------------------//
//alpha: step size
//lambda: controls the contribution from switch cost and learning accuracy
//===============================================================================
//Z(u)^r=z(u)^(r-1)+partial JZ(u)+(a1-1)/a2(z(u)^(r-1)+partial JZ(u)-Z(u)^(r-1))
//Zu^(r)=Z(u)^(r-1)+(a2+a1-1)/a2*partial JZ(u);
//a=(a2+a1-1)/a2;
//================================================================================
//inline void updateZauto(SparseMatrix &Z, Node *&G, int u, double lambda, double sumZ, Row &Zuprime, double a){
//	int i = 0;
//	int v;
//	double c1;  //Wt(u, v)-Z(u).Z(v)
//	double c2 = 0;  //sum_{v}Zt(v).Zt(v)
//	int pos = 0;
//	for (i = 0; i<G[u].deg; i++){
//		v = G[u].nbv[i];
//		if (node2subidx!=NULL)
//			v = node2subidx[v];
//		c1 = G[u].weight[i] - Z.Rowdotproduct2(u, v);
//		tempvalue[pos] = c1;
//		pos++;
//		c2 += Z.Rowdotproduct2(v, v);
//	}
//	//c=-2alpha[Fnorm(Z)*2-Z(u).Z(u)-sum_{v}Z(v).Z(v)]
//	double c = sumZ - Z.Rowdotproduct(u) - c2;
//	c = -c * 2 * alpha;
//	c = c*a;
//	Z.RowsumMultiply(u, a*c);
//	for (i = 0; i<G[u].deg; i++){
//		v = G[u].nbv[i];
//		if (node2subidx!=NULL)
//			v = node2subidx[v];
//		if (abs(tempvalue[i])>0){
//			Z.RowsumLeftEqual(u, v, tempvalue[i] * 2 * alpha*a);
//		}
//	}
//	//finally add alpha\lambda Zt-1(u)
//	Z.RowsumLeftEqual(u, Zuprime, alpha*lambda*a);
//	Z.Rownorm2(u);
//}


///----------------------------------------------------------------------//
//alpha: step size
//lambda: controls the contribution from switch cost and learning accuracy
//===============================================================================
//Z(u)^r=z(u)^(r-1)+partial JZ(u)+(a1-1)/a2(z(u)^(r-1)+partial JZ(u)-Z(u)^(r-1))
//Zu^(r)=Z(u)^(r-1)+(a2+a1-1)/a2*partial JZ(u);
//a=(a2+a1-1)/a2;
//================================================================================
inline void updateZautofull(SparseMatrix &Z, Node *&G, int u, double lambda, double **B, Row &Zuprime, Row &Zunext,double a){
	int i = 0;
	int v;
	Z.Multiplymatrix(u, B, tempvalue); //tempvalue=Z(u)*Z^T*Z
	Z.RowsumMultiply(u, a * 2 * alpha); //Z(u)=Z(u)+2\alpha Z(u)
	for (i = 0; i<G[u].deg; i++){
		v = G[u].nbv[i];
		if (node2subidx != NULL)
			v = node2subidx[v];
		Z.RowsumLeftEqual(u, v, G[u].weight[i] * 2 * alpha*a);
	}//Z(u)=Z(u)+2\alpha*sum_{v}w(u,v)*Z(v)
	//add -2\alpha*Z(u)*Z^T*Z
	Z.RowsumLeftEqual(u, tempvalue, -2 * alpha*a);
	//finally add alpha\lambda Zt-1(u)
	Z.RowsumLeftEqual(u, Zuprime, alpha*lambda*a);
	Z.RowsumLeftEqual(u, Zunext, alpha*lambda*a);
	Z.Rownorm2(u);
	Z.Shrinkrow(u);
}
inline void updateZauto(SparseMatrix &Z, Node *&G, int u, double lambda, double **B, Row &Zuprime, double a){
	int i = 0;
	int v;
	Z.Multiplymatrix(u, B, tempvalue); //tempvalue=Z(u)*Z^T*Z
	Z.RowsumMultiply(u, a*2*alpha); //Z(u)=Z(u)+2\alpha Z(u)
	for (i = 0; i<G[u].deg; i++){
		v = G[u].nbv[i];
		if (node2subidx!=NULL)
			v = node2subidx[v];
		Z.RowsumLeftEqual(u, v, G[u].weight[i] * 2 * alpha*a);
	}//Z(u)=Z(u)+2\alpha*sum_{v}w(u,v)*Z(v)
	//add -2\alpha*Z(u)*Z^T*Z
	Z.RowsumLeftEqual(u, tempvalue, -2 * alpha*a);
	//finally add alpha\lambda Zt-1(u)
	Z.RowsumLeftEqual(u, Zuprime, alpha*lambda*a);
	Z.Rownorm2(u);
	Z.Shrinkrow(u);
}
//============================================================================
//compute the Lipschitz constant (start)
//============================================================================
//inline double ComputeL(Node *&G, int nodenum){
//	int m = 0;
//	int i = 0;
//	int j = 0;
//	double L = 0;
//	for (i = 0; i < nodenum; i++){
//		for (j = 0; j < G[i].deg; j++){
//			L += (3 - 0.5*G[i].weight[j])*(3 - 0.5*G[i].weight[j]);
//			m++;
//		}
//	}
//	L /= nodenum;
//	double c = (nodenum - 1 - (double)m / nodenum);
//	L += c*c * 9;
//	L = 4 * sqrt(L);
//	//L = L*sqrt(nodenum);
//	return L;
//}
inline double ComputeL(int nodenum, int m){
	double L = 0;
	L = (nodenum-2+(double)m/nodenum);
	L = sqrt(L);
	L *= 2;
	L *= sqrt(nodenum);
	return L;
}
//============================================================================
//compute the Lipschitz constant (end)
//============================================================================


//===========================================================================
//read the maximum number of nodes from file (	START) 
//=============================================================================
inline int Getmaxnodenumber(vector<char*> &filenames){
	int maxnodenum = 0;  //maximum number of nodes over time
	FILE *rFile = NULL;
	int t = 0;
	//compute the maximum node num for each graph (start)
	int nodenum;
	for (t = 0; t < (int)filenames.size(); t++){
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
		if (nodenum > maxnodenum){
			maxnodenum = nodenum;
		}
		fclose(rFile);
	}
	//compute the maximum node num for each graph (end)
	return maxnodenum;
}
//===========================================================================
//read the maximum number of nodes from file (END)
//=============================================================================
#endif