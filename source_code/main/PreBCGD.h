#include"Updatecommunity.h"
inline void PreBCGDLocal(int m, char * cprefilename, char * filename, double lambda, double membound, string &prefix, int printstep, bool isauto){
	int i = 0;
	int j = 0;
	Node *G;
	SparseMatrix Z;
	SparseMatrix Zprime;
	int nodenum = 0;
	int iter = 0;
	int gnodenum = 0;
	string s("Zmatrix");
	s = prefix + s;
	char *cfilename = new char[4096];
	char *tempstr = new char[4096];
	strcpy(cfilename, s.c_str());
	itostring(0, tempstr, 10);
	strcat(cfilename, tempstr);
	double error;
	double preerror;
	double **B = new double *[m];
	for (i = 0; i < m; i++){
		B[i] = new double[m];
	}
	double epsilo;
	FILE *rFile = NULL;
	FILE *rfile3 = NULL;
	FILE *rfile2 = NULL;
	FILE *wfile = NULL;
	FILE *wfiletemp = NULL;
	double a1;
	double a2;
	double a;
	//============================================
	// Open graph file to read (START)
	//===========================================
	rFile = fopen(filename, "r");
	if (rFile == NULL){
		printf("Could not find the data file\n");
		exit(1);
	}
	int rv = fscanf(rFile, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of graph file %s is not correct", filename);
		exit(2);
	}
	//============================================
	// Open graph file to read (END)
	//===========================================
	// Initilize the memory usage (start)
	InitTempSubmemory(nodenum);
	double staticsize = sizeof(double)*nodenum;
	staticsize += (sizeof(int)*nodenum * 2);
	staticsize = staticsize / 1024;
	if (membound <= staticsize){
		cout << "Please increase the membound by " << staticsize << endl;
		exit(2);
	}
	membound -= staticsize;
	//=============================================
	// OPEN Z matrix pre file to read (START)
	//===============================================
	
	rfile3 = fopen(cprefilename, "r");
	rv = fscanf(rfile3, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of matrix file %s is not correct", cprefilename);
		exit(2);
	}

	//=============================================
	//=========OPEN Z matrix pre file to read (END)
	//===============================================
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
					updateZ(Z, G, i, lambda);
			}
			error = Fnormfast(G, Z, gnodenum);
			if (iter == 0)
				preerror = gnodenum;
			epsilo = (preerror - error) / gnodenum;
			if (iter%printstep == 0)
				printf("%d\t%f\t%.10lf\n", iter, error, epsilo);
			preerror = error;
			a1 = a2;
			iter++;
		} while (iter < maxiter);
		Z.Writetofile(wfiletemp, G);
		//==========================================================
		// Release Used Memory  (START)================
		//==========================================================
		Zprime.clear();
		Z.clear();
		releasegraphmemory(G, gnodenum);
		curMemPos = precurMem2Start;
		curMemEnd = precurMem2End;
		curBlk = precurblk2;
		if (G != NULL){
			free(G);
			G = NULL;
		}
		Zprime.deletemem();
		Z.deletemem();
		//==========================================================
		// Release Used Memory  (END)================
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
	if (wfiletemp != NULL) {
		fclose(wfiletemp);
		wfiletemp = NULL;
	}
	//replace Zmatrixt with temp.txt
	Replacefile(cfilename, "temp.txt");
    releaseTempSubmemory();
	if (B != NULL) {
		for (i = 0; i < m; i++){
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
	if (tempstr != NULL) {
		delete[]tempstr;
		tempstr = NULL;
	}
}