#include"Updatecommunity.h"
inline void InitAffectedNode(Node *&Gt, Node *&Gtpre, char *&affected,int nodenum, SparseMatrix &Ztpre, SparseMatrix &Z){
	int i = 0;
	//Initilize S as the set of updated nodes (start)
	for (i = 0; i < nodenum; i++){
		if (Gt[i].deg != Gtpre[i].deg){
			affected[i] = 'a';
		}
		else{
			bool flag = isidentical(Gt[i].nbv, Gtpre[i].nbv,Gt[i].deg);
			if (flag == false)
				affected[i] = 'a';
			else
				copyRow(Z.matrix[i], Ztpre.matrix[i]);
		}
	}
	//Initilize S as the set of updated nodes (end)
}
inline bool Isrecover(SparseMatrix &Zt, SparseMatrix &Ztpre, int u, double delta){
	int i = 0;
	int j = 0;
	while (i < Zt.matrix[u].clength&&j < Ztpre.matrix[u].clength){
		if (Zt.matrix[u].idx[i] < Ztpre.matrix[u].idx[j]){
			i++;
		}else
			if (Zt.matrix[u].idx[i] > Ztpre.matrix[u].idx[j]){

			}
			else{
				if (abs(Zt.matrix[u].weight[i] - Ztpre.matrix[u].weight[j]) >= delta)
					return false;
				i++;
				j++;
			}
	}
	while (i < Zt.matrix[u].clength){
		if (Zt.matrix[u].weight[i] >= delta)
			return false;
		i++;
	}
	while (j < Ztpre.matrix[u].clength){
		if (Ztpre.matrix[u].weight[j] >= delta)
			return false;
		j++;
	}
	return true;
}
inline bool Isaffected(SparseMatrix &Zt, SparseMatrix &Ztpre, int u, int w, double zeta){
	bool flag = false;
	double a1 = Zt.Rowdotproduct2(u, w);
	double a2 = Ztpre.Rowdotproduct2(u, w);
	if (abs(a1 - a2) >= zeta)
		return true;
	else
		return false;
}
inline void ComputeAffectedNode(Node *Gt, SparseMatrix &Zt, SparseMatrix &Ztpre, char *&affected, FILE *&wfile2, double delta, double zeta){
	//FILE *wfile2 = fopen("Pa.txt", "a+");
	//if (wfile2 == NULL){
		//printf("could not open file to write\n");
		//exit(1);
	//}
	int i = 0;
	int j = 0;
	int nodenum = Zt.n;
	int communitynum = Zt.m;
	int u = 0;
	bool flag;
	int w;
	double pa;
	double pr;
	pr = 0;
	pa = 0;
	int nbtotal = 0;
	int pos = 0;
	int soldsize = 0;
	for (i = 0; i < nodenum; i++){
		if (affected[i] == 'n' || affected[i] == 'c')
			continue;
		u = i;
		soldsize++;
		flag = Isrecover(Zt, Ztpre, u, delta);
		if (flag == true){
			affected[i] = 'n';
			pr++;
		}
		nbtotal += Gt[u].deg;
		for (j = 0; j < Gt[u].deg; j++){
			w = Gt[u].nbv[j];
			if (node2subidx != NULL){
				w = node2subidx[w];
			}
			flag = Isaffected(Zt, Ztpre, u, w, zeta);
			if (flag == true && affected[w] == 'n'){
				affected[w] = 'c';
				pa++;
			}
		}
	}
	for (i = 0; i < nodenum; i++){
		if (affected[i] == 'c')
			affected[i] = 'a';
	}
	pr = pr / soldsize;
	pa = pa / nbtotal;
	fprintf(wfile2, "pa\t%lf\tpr\t%lf\n", pa, pr);
	//fclose(wfile2);
	//cout << "pa\t" << pa << "\tpr\t" << pr << endl;
}
/*
Incremental compute the Z matrix at time t based on Z matrix at time t-1, and graph change at time t \Delta G_t
In addition, the entire graph matrix and Z matrix can be fully stored into memory
*/
void IncrementalBCGDfull(int m, vector<char *> &filenames, double lambda, string &prefix, int printstep, bool isauto, int maxnodenum, bool zetainput, bool deltainput){
	int i = 0;
	int j = 0;
	int t = 0;
	int k = 0;
	Node *G;
	Node *Gpre;
	SparseMatrix Z;
	SparseMatrix Zprime;
	int nodenum = 0;
	int iter = 0;
	int u;
	int w;
	int c;
	double a1;
	double a2;
	double a;
	double error;
	double preerror;
	double epsilo;
	string ssize("Ssize");
	string sprob("PA");
	string s("Zmatrix");
	s = prefix + s;
	ssize = prefix + ssize;
	sprob = prefix + sprob;
	string tempstring;
	char *cfilename = new char[2048];
	char *cprefilename = new char[2048];
	char *ssizefile=new char[2048];
	char *spfile=new char[2048];
	double **B = new double*[m];
	for (i = 0; i < m; i++){
		B[i] = new double[m];
	}
	char *tempstr = new char[2048];
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
	FILE *rgFile = NULL;
	FILE *wfile2 = NULL;
	FILE *wfile3 = NULL;
	char *affected = new char[maxnodenum];
	memset(affected, 'n', sizeof(char)*maxnodenum);
	char *precurMem=NULL;
	int precurblk=0;
	char *preendMem=NULL;
	int sssize = 0;
	int rv = 0;
	for (t = 0; t < (int)filenames.size(); t++){
		precurMem = curMemPos;
		precurblk = curBlk;
		preendMem = curMemEnd;
		//============================================
		//=====Open graph file to read (START)
		//===========================================
		rFile = fopen(filenames.at(t), "r");
		if (rFile == NULL){
			printf("Could not find the data file\n");
			exit(1);
		}
		rv = fscanf(rFile, "%d\n", &nodenum);
		if (rv != 1){
			printf("the format of graph file %s is not correct", filenames.at(t));
			exit(2);
		}
		ReadGraph(G, nodenum, rFile);
		fclose(rFile);
		//============================================
		//=====Open graph file to read (END)
		//===========================================

		//=============================================
		//=========OPEN Z matrix pre file to read (START)
		//===============================================
		if (t == 0){
			strcpy(cprefilename, s.c_str());
			itostring(t, tempstr, 10);
			strcat(cprefilename, tempstr);
		}
		else{
			strcpy(cprefilename, s.c_str());
			itostring(t - 1, tempstr, 10);
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
		fclose(rfile3);
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

		//================================================
		//==========Open Ssize file to write (START)
		//================================================
		/* tempstring = ssize + std::to_string(t);
		wfile2 = fopen(tempstring.c_str(), "w"); */
		strcpy(ssizefile,ssize.c_str());
		itostring(t, tempstr, 10);
		strcat(ssizefile,tempstr);
		wfile2=fopen(ssizefile,"w");
		if (wfile2 == NULL){
			printf("could not open file to write\n");
			exit(1);
		}
		//================================================
		//==========Open Ssize file to write (END)
		//================================================

		//================================================
		//==========Open pa file to write (START)
		//================================================
		/* tempstring = sprob + std::to_string(t);
		wfile3 = fopen(tempstring.c_str(), "w"); */
		strcpy(spfile,sprob.c_str());
		itostring(t, tempstr, 10);
		strcat(spfile,tempstr);
		wfile3=fopen(spfile,"w");
		if (wfile3 == NULL){
			printf("could not open file to write\n");
			exit(1);
		}
		//================================================
		//==========Open pa file to write (END)
		//================================================
		if (t == 0){
			Z = Zprime;
			iter = 0;
			a1 = 1;
			error = 0;
			do{
				Z.Gettransposesqure(B);
				a2 = (1 + sqrt(4 * a1*a1 + 1)) / 2;
				a = (a2 + a1 - 1) / a2;
				if (isauto == true){
					alpha = 1.0 / ComputeL(nodenum,m);
					if (iter%printstep == 0)
						printf("stepsize value now is %lf\n", alpha);
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
					printf("%d\t%f\t%lf\n", iter, error, epsilo);
				preerror = error;
				a1 = a2;
				iter++;
			} while (iter < maxiter&&epsilo>0.0000000001);
			Vmin = 0;
			Vmax = nodenum - 1;
			Z.Writetofile(wfile, G);
			if (wfile != NULL)
				fclose(wfile);
		}
		else{
			//============================================
			//=====Open previous graph file to read (START)
			//===========================================
			rgFile = fopen(filenames.at(t - 1), "r");
			if (rgFile == NULL){
				printf("Could not find the data file\n");
				exit(1);
			}
			int rv = fscanf(rgFile, "%d\n", &nodenum);
			if (rv != 1){
				printf("the format of graph file %s is not correct", filenames.at(t));
				exit(2);
			}
			ReadGraph(Gpre, nodenum, rgFile);
			fclose(rgFile);
			//============================================
			//=====Open previous graph file to read (END)
			//===========================================

			//================================================
			//Initilize the Z matrix (START)
			//================================================
			memset(affected, 'n', sizeof(char)*nodenum);
			Z.Initmemory(nodenum);
			InitAffectedNode(G, Gpre, affected, nodenum, Zprime, Z);
			Z.setcolumnnum(m);
			for (i = 0; i < nodenum; i++){
				if (affected[i] == 'n')
					continue;
				u = i;
				for (j = 0; j < m; j++){
					tempvalue[j] = 0.0;
				}
				for (j = 0; j < G[u].deg; j++){
					w = G[u].nbv[j];
					for (k = 0; k < Zprime.matrix[w].clength; k++){
						c = Zprime.matrix[w].idx[k];
						tempvalue[c] += Zprime.matrix[w].weight[k];
					}
				}
				int dv = 0;
				for (j = 0; j < m; j++){
					if (tempvalue[j]>0)
						dv++;
				}
				Z.Initrowmemory(u, dv);
				for (j = 0; j < m; j++){
					if (tempvalue[j]>0){
						Z.addelement(u, j, tempvalue[j]);
					}
				}
				Z.Rownorm2(u);
			}
			//================================================
			//Initilize the Z matrix (END)
			//================================================

			a1 = 1;
			iter = 0;
			if (zetainput == false){
				zeta = sqrt(-log(1 - 1.0 / nodenum));
				cout << "zeta " << zeta << endl;
			}
			if (deltainput == false){
				delta = 2 * sqrt(-log(1 - 1.0 / nodenum)) / m;
				cout << "delta " << delta << endl;
			}
			do{
				Z.Gettransposesqure(B);
				a2 = (1 + sqrt(4 * a1*a1 + 1)) / 2;
				a = (a2 + a1 - 1) / a2;
				if (isauto == true){
					alpha = 1.0 / ComputeL(nodenum,m);
					if (iter%printstep == 0)
						printf("stepsize value now is %lf\n", alpha);
				}
				sssize = 0;
				for (i = 0; i < nodenum; i++){
					if (affected[i] == 'n')
						continue;
					u = i;
					sssize++;
					if (isauto == false)
						updateZauto(Z, G, u, lambda, B, Zprime.matrix[u], 1.0);
					else
						updateZauto(Z, G, u, lambda, B, Zprime.matrix[u], a);
				}
				if (iter%printstep == 0)
					fprintf(wfile2, "%d\t%d\n", iter, sssize);
				ComputeAffectedNode(G, Z, Zprime, affected, wfile3, delta, zeta);
				error = Fnormfast(G, Z, nodenum);
				if (iter == 0)
					preerror = nodenum;
				epsilo = (preerror - error) / nodenum;
				if (iter%printstep == 0)
					printf("%d\t%f\t%lf\n", iter, error, epsilo);
				preerror = error;
				a1 = a2;
				iter++;
			} while (iter < maxiter&&epsilo>0.0000000001);
			Vmin = 0;
			Vmax = nodenum - 1;
			Z.Writetofile(wfile, G);
			if (wfile != NULL)
				fclose(wfile);
			releasegraphmemory(Gpre, nodenum);
			if (Gpre != NULL){
				free(Gpre);
				Gpre = NULL;
			}
			if (rgFile != NULL)
				fclose(rgFile);
		}//end of else
		//===========================================================
		// Realease used memory (START)
		//============================================================
		Zprime.clear();
		Zprime.deletemem();
		Z.clear();
		Z.deletemem();
		releasegraphmemory(G, nodenum);
		if (G != NULL){
			free(G);
			G = NULL;
		}
		curMemPos = precurMem;
		curBlk = precurblk;
		curMemEnd = preendMem;
		//===========================================================
		// Realease used memory (END)
		//============================================================
		if (wfile2 != NULL)
			fclose(wfile2);
		if (wfile3 != NULL)
			fclose(wfile3);
	}
	//_CrtDumpMemoryLeaks();
	if (tempvalue != NULL){
		delete []tempvalue;
		tempvalue = NULL;
	}
	if (cfilename!=NULL)
		delete[]cfilename;
	if (cprefilename != NULL){
		delete[]cprefilename;
	}
	if (tempstr!=NULL)
		delete[]tempstr;
	if (affected!=NULL)
		delete[]affected;
	if (ssizefile!=NULL)
		delete[]ssizefile;
	if (spfile!=NULL)
		delete[]spfile;
    for(i=0;i<m;i++){
        delete []B[i];
        B[i]=NULL;
    }
    delete []B;
}

/*
Incremental compute the Z matrix at time t based on Z matrix at time t-1, and graph change at time t \Delta G_t
In addition, the entire graph matrix and Z matrix can not be fully stored into memory
Therefore, we sequential scan the graph and matrix file, load partical of it into memory, do the computation, and 
save results back to disk
*/
void IncrementalBCGDsparse(int m, vector<char *> &filenames, double lambda, double membound, string &prefix, int printstep, bool isauto, int maxnodenum, bool zetainput, bool deltainput){
	int i = 0;
	int j = 0;
	int t = 0;
	int k = 0;
	int gnodenum = 0;
	int updatednum = 0;
	Node *G;
	Node *Gpre;
	SparseMatrix Z;
	SparseMatrix Zprime;
	int nodenum = 0;
	int iter = 0;
	int u;
	int w;
	int c;
	double a1;
	double a2;
	double a;
	double error;
	double preerror;
	double epsilo;
	string ssize("Ssize");
	string sprob("PA");
	string s("Zmatrix");
	s = prefix + s;
	ssize = prefix + ssize;
	sprob = prefix + sprob;
	string tempstring;
	char *cfilename = new char[2048];
	char *cprefilename = new char[2048];
	char *tempstr = new char[2048];
	char *ssizefile=new char[2048];
	char *spfile=new char[20480];
	double **B = new double*[m];
	for (i = 0; i < m; i++){
		B[i] = new double[m];
	}
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
	FILE *rfile3 = NULL;  //read Zmatrix pre file
	FILE *rfile2 = NULL;  //read Zmatrix file
	FILE *wfile = NULL;  //write Zmatrix into temp.txt
	FILE *rFile = NULL;  //read graph file
	FILE *rgFile = NULL; //read pre graph file
	FILE *wfile2 = NULL; //write Ssize into file
	FILE *wfile3 = NULL; //write PA, PC into file
	//===============================================================
	//Update the Zmatrix for time 0 (START)
	//===================================================================
	//============================================
	//=====Open graph file to read (START)
	//===========================================
	rFile = fopen(filenames.at(0), "r");
	if (rFile == NULL){
		printf("Could not find the data file\n");
		exit(1);
	}
	int rv = fscanf(rFile, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of graph file %s is not correct", filenames.at(t));
		exit(2);
	}
	//============================================
	//=====Open graph file to read (END)
	//===========================================

	//=============================================
	//=========OPEN Z matrix file to read (START)
	//===============================================
	strcpy(cfilename, s.c_str());
	itostring(0, tempstr, 10);
	strcat(cfilename, tempstr);
	rfile2 = fopen(cfilename, "r");
	rv = fscanf(rfile2, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of matrix file %s is not correct", cfilename);
		exit(2);
	}

	//=============================================
	//=========OPEN Z matrix file to read (END)
	//===============================================
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
	totalfinish = -1;
	Vmin = nodenum;
	Vmax = 0;
	memset(node2subidx, -1, sizeof(int)*nodenum);
	while (1){
		//remember where we are in memory usage (START)
		char *precurMem2 = curMemPos;
		int precurblk2 = curBlk;
		char* preendMem2 = curMemEnd;
		//remember where we are in memory usage (START)
		gnodenum = 0;
		memset(isseednode, 'v', sizeof(char)*nodenum);
		ReadSeeNode(gnodenum, nodenum, 10, membound, rFile);
		ReadSubGraph(G, gnodenum, nodenum, rFile);
		//GraphNorm(G, gnodenum);
		if (iter%printstep == 0)
			printf("in memory seed set is [%d, %d]\n", Vmin, Vmax);
		Readsubcommunity(Z, gnodenum, nodenum, rfile2);
		error = 0;
		a1 = 1;
		iter = 0;
		Zprime = Z;
		do{
			double sumZ = Z.Fnorm();
			sumZ = pow(sumZ, 2);
			a2 = (1 + sqrt(4 * a1*a1 + 1)) / 2;
			a = (a2 + a1 - 1) / a2;
			if (isauto == true){
				alpha = 1.0 / ComputeL(nodenum,m);
				if (iter%printstep == 0)
					printf("stepsize value now is %lf\n", alpha);
			}
			for (i = 0; i < gnodenum; i++){
				if (isauto == false)
					updateZauto(Z, G, i, lambda, B, Zprime.matrix[i], 1.0);
				else
					updateZauto(Z, G, i, lambda, B, Zprime.matrix[i], a);
			}
			error = Fnormfast(G, Z, gnodenum);
			if (iter == 0)
				preerror = gnodenum;
			epsilo = (preerror - error) / gnodenum;
			if (iter%printstep == 0)
				printf("%d\t%f\t%lf\n", iter, error, epsilo);
			preerror = error;
			a1 = a2;
			iter++;
		} while (iter < maxiter&&epsilo>0.000000001);
		////////////////////////////////////////////////////////////
		Z.Writetofile(wfile, G);
		//==========================================================
		//==============Release Used Memory  (START)================
		//==========================================================
		Zprime.clear();
		releasegraphmemory(G, gnodenum);
		Z.clear();
		curMemPos = precurMem2;
		curBlk = precurblk2;
		curMemEnd = preendMem2;
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
	if (wfile != NULL)
		fclose(wfile);
	if (rfile2 != NULL)
		fclose(rfile2);
	if (rFile != NULL)
		fclose(rFile);
	Replacefile(cfilename, "temp.txt");
	//===============================================================
	//Update the Zmatrix for time 0 (END)
	//===================================================================
	char *affected = new char[nodenum];
	memset(affected, 'n', sizeof(char)*nodenum);
	for (t = 1; t < (int)filenames.size(); t++){
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
		//============================================
		//=====Open graph file to read (END)
		//===========================================

		//============================================
		//=====Open previous graph file to read (START)
		//===========================================
		rgFile = fopen(filenames.at(t - 1), "r");
		if (rgFile == NULL){
			printf("Could not find the data file\n");
			exit(1);
		}
		rv = fscanf(rgFile, "%d\n", &nodenum);
		if (rv != 1){
			printf("the format of graph file %s is not correct", filenames.at(t));
			exit(2);
		}

		//=============================================
		//=========OPEN Z matrix pre file to read (START)
		//===============================================
		strcpy(cprefilename, s.c_str());
		itostring(t - 1, tempstr, 10);
		strcat(cprefilename, tempstr);
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
		//=============================================
		//=========OPEN Z matrix pre file to read (END)
		//===============================================

		//================================================
		//==========Open Ssize file to write (START)
		//================================================
		/* tempstring = ssize + std::to_string(t);
		wfile2 = fopen(tempstring.c_str(), "w"); */
		strcpy(ssizefile,ssize.c_str());
		itostring(t, tempstr, 10);
		strcat(ssizefile,tempstr);
		wfile2=fopen(ssizefile,"w");
		if (wfile2 == NULL){
			printf("could not open file to write\n");
			exit(1);
		}
		//================================================
		//==========Open Ssize file to write (END)
		//================================================

		//================================================
		//==========Open pa file to write (START)
		//================================================
		/* tempstring = sprob + std::to_string(t);
		wfile3 = fopen(tempstring.c_str(), "w"); */
		strcpy(spfile,sprob.c_str());
		itostring(t, tempstr, 10);
		strcat(spfile,tempstr);
		wfile3=fopen(spfile,"w");
		if (wfile3 == NULL){
			printf("could not open file to write\n");
			exit(1);
		}
		//================================================
		//==========Open pa file to write (END)
		//================================================

		//================================================
		//==========Open Zmatrix file to write (START)
		//================================================
		strcpy(cfilename, s.c_str());
		itostring(t, tempstr, 10);
		strcat(cfilename, tempstr);
		wfile = fopen(cfilename, "w");
		if (wfile == NULL){
			printf("could not open file to write\n");
			exit(1);
		}
		fprintf(wfile, "%d\n", nodenum);
		//================================================
		//==========Open Zmatrix file to write (END)
		//================================================
		totalfinish = -1;
		Vmin = nodenum;
		Vmax = 0;
		memset(node2subidx, -1, sizeof(int)*nodenum);
		while (1){
			//remember where we are in memory usage (START)
			char *precurMem2 = curMemPos;
			int precurblk2 = curBlk;
			char* preendMem2 = curMemEnd;
			//remember where we are in memory usage (END)
			gnodenum = 0;
			memset(isseednode, 'v', sizeof(char)*nodenum);
			ReadSeeNode(gnodenum, nodenum, 10, membound, rFile);
			ReadSubGraph(G, gnodenum, nodenum, rFile);
			ReadSubGraph(Gpre, gnodenum, nodenum, rgFile);
			//GraphNorm(G, gnodenum);
			if (iter%printstep == 0)
				printf("in memory seed set is [%d, %d]\n", Vmin, Vmax);
			Readsubcommunity(Zprime, gnodenum, nodenum, rfile3);
			Z.Initmemory(gnodenum);
			if (zetainput == false){
				zeta = sqrt(1.0 / gnodenum);
				cout << "zeta " << zeta << endl;
			}
			if (deltainput == false){
				delta = 2 * zeta / m;
				cout << "delta " << delta << endl;
			}
			memset(affected, 'n', sizeof(char)*nodenum);
			InitAffectedNode(G, Gpre,affected, gnodenum, Zprime, Z);
			Z.setcolumnnum(m);
			updatednum = 0;
			for (i = 0; i < gnodenum; i++){
				if (affected[i] == 'n')
					continue;
				updatednum++;
				u = i;
				for (j = 0; j < m; j++){
					tempvalue[j] = 0.0;
				}
				for (j = 0; j < G[u].deg; j++){
					w = G[u].nbv[j];
					if (node2subidx != NULL)
						w = node2subidx[w];
					for (k = 0; k < Zprime.matrix[w].clength; k++){
						c = Zprime.matrix[w].idx[k];
						tempvalue[c] += Zprime.matrix[w].weight[k];
					}
				}
				int dv = 0;
				for (j = 0; j < m; j++){
					if (tempvalue[j]>0)
						dv++;
				}
				Z.Initrowmemory(u, dv);
				for (j = 0; j < m; j++){
					if (tempvalue[j]>0){
						Z.addelement(u, j, tempvalue[j]);
					}
				}
				Z.Rownorm2(u);
			}
			cout << "updated node num " << updatednum << endl;
			//================================================
			//Initilize the Z matrix (END)
			//================================================
			a1 = 1;
			iter = 0;
			error = 0;
			do{
				a2 = (1 + sqrt(4 * a1*a1 + 1)) / 2;
				a = (a2 + a1 - 1) / a2;
				Z.Gettransposesqure(B);
				if (isauto == true){
					alpha = 1.0 / ComputeL(nodenum,m);
					if (iter%printstep == 0)
						printf("stepsize value now is %.10lf\n", alpha);
				}
				int sssize = 0;
				for (i = 0; i < gnodenum; i++){
					if (affected[i] == 'n')
						continue;
					u = i;
					if (isauto == false)
						updateZauto(Z, G, u, lambda, B, Zprime.matrix[u], 1.0);
					else
						updateZauto(Z, G, u, lambda, B, Zprime.matrix[u], a);
				}
				if (iter%printstep == 0)
					fprintf(wfile2, "%d\t%d\n", iter, sssize);
				ComputeAffectedNode(G, Z, Zprime, affected, wfile3, delta, zeta);
				error = Fnormfast(G, Z, gnodenum);
				if (iter == 0)
					preerror = gnodenum;
				epsilo = (preerror - error) / gnodenum;
				if (iter%printstep == 0)
					printf("%d\t%f\t%.10lf\n", iter, error, epsilo);
				preerror = error;
				a1 = a2;
				iter++;
			} while (iter < maxiter&&epsilo>0.0000000001);
			//=========================================================
			//========Release Memory Usage (START)
			//=========================================================
			Z.Writetofile(wfile, G);
			Z.clear();
			Zprime.clear();
			releasegraphmemory(G, gnodenum);
			releasegraphmemory(Gpre, gnodenum);
			curMemPos = precurMem2;
			curBlk = precurblk2;
			curMemEnd = preendMem2;
			if (G != NULL){
				free(G);
				G = NULL;
			}
			if (Gpre != NULL){
				free(Gpre);
				Gpre = NULL;
			}
			Z.deletemem();
			Zprime.deletemem();
			//=========================================================
			//========Release Memory Usage (END)
			//=========================================================
			//update in-memory seed set
			Vmin = nodenum;
			totalfinish = Vmax;
			if (Vmax == nodenum - 1)
				break;
		}//end of while (1)
		if (wfile != NULL)
			fclose(wfile);
		if (wfile2 != NULL)
			fclose(wfile2);
		if (wfile3 != NULL)
			fclose(wfile3);
		if (rFile != NULL)
			fclose(rFile);
		if (rgFile != NULL)
			fclose(rgFile);
		if (rfile3 != NULL)
			fclose(rfile3);
	}//end of time t
    releaseTempSubmemory();
	delete[]cfilename;
	delete[]cprefilename;
	delete[]tempstr;
	delete[]affected;
	delete[]spfile;
	delete[]ssizefile;
    for(i=0;i<m;i++){
        delete []B[i];
        B[i]=NULL;
    }
	delete[]B;
}