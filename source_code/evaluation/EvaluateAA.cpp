#include"Evaluate.h"
/*
author: Linhong Zhu
contact: linhong.seba.zhu@gmail.com
Latest update: April, 15, 2015

*/
int main(int argc, char *argv[]){
	if (argc < 2){
		printf("Usage: %s [training graph] [test pair]\n", argv[0]);
		exit(0);
	}
	/////////////////////////////////////////////////////////////
	////////////initializing memory block (START)////////////////
	////////////////////////////////////////////////////////////
	//Initialize memory blocks (START)
	curBlk = endBlk = 0;
	memBlkAr[0] = curMemPos = (char*)malloc(BLK_SZ);
	curMemEnd = curMemPos + BLK_SZ;
	//Initialize memory blocks (END)
	//Initialize memory blocks (START)
	curBlk2 = 0;
	memBlkAr2[0] = curMemPos2 = (char*)malloc(BLK_SZ2);
	curMemEnd2 = curMemPos2 + BLK_SZ2;
	//Initialize memory blocks (END)
	/////////////////////////////////////////////////////////
	////////////initializing memory block (end)////////////////
	////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	///////////initializing IO memory block (START)/////////////
	///////////////////////////////////////////////////////////
	BYTE_TO_READ = 60000000;
	inputbuffer = (char*)malloc(BYTE_TO_READ);
	outputbuffer = (char*)malloc(BYTE_TO_READ);
	outcurpos = outputbuffer;
	outendpos = outputbuffer + BYTE_TO_READ;
	temp_string = new char[65];
	////////////////////////////////////////////////////////////
	///////////initializing IO memory block (END)/////////////
	///////////////////////////////////////////////////////////
	int i = 0;
	int type;
	int nodenum;
	int j;
	int w;
	int *node2idx;
	int *idx2node;
	int *AAtarget;
	double *AAscore;
	int pos;
	int cur;
	double AAvalue;
	int idx;
	DNode *Gtest;

	//====================================================
	//==========read the traning graph (START)============
	//====================================================
	Node *G;
	FILE *rfile = fopen(argv[1], "r");
	if (rfile == NULL){
		printf("could not open training graph file to read\n");
		exit(2);
	}
	int rv = fscanf(rfile, "%d\n", &nodenum);
	if (rv != 1){
		printf("the format of graph file is not correct\n");
		exit(2);
	}
	ReadGraph(G, nodenum, rfile);
	fclose(rfile);
	//====================================================
	//==========read the traning graph (END)==============
	//====================================================

	node2idx = new int[nodenum];
	idx2node = new int[nodenum];
	AAtarget = new int[nodenum];
	AAscore = new double[nodenum];
	///////////////////////////////////////////////////////////////////////////
	//construct a test graph from test pairs (START)
	//////////////////////////////////////////////////////////////////////////
	FILE *rfile3 = fopen(argv[2], "r");
	if (rfile3 == NULL){
		printf("could not open test pair file to read\n");
		exit(2);
	}
	int testnodenum = 0;
	while (feof(rfile3) == false){
		int c = fscanf(rfile3, "%d\t%d\t%d\n", &i, &j,&w);
		if (c != 3){
			printf("the format of test pair file is not correct!\n");
			exit(2);
		}
		cur = binarysearch(idx2node, 0, testnodenum, i);
		if (cur < 0){
			insert(idx2node, -cur - 1, i, testnodenum);
			testnodenum++;
		}
		cur = binarysearch(idx2node, 0, testnodenum, j);
		if (cur < 0){
			insert(idx2node, -cur - 1, j, testnodenum);
			testnodenum++;
		}
	}

	fclose(rfile3);
	for (i = 0; i < nodenum; i++){
		node2idx[i] = -1;
	}
	for (i = 0; i < testnodenum; i++){
		node2idx[idx2node[i]] = i;
	}
	rfile3 = fopen(argv[2], "r");
	if (rfile3 == NULL){
		printf("could not open test pair file to read\n");
		exit(2);
	}
	int index1;
	int index2;
	Gtest = (DNode *)malloc(sizeof(DNode)*testnodenum);
	for (i = 0; i < testnodenum; i++){
		Gtest[i].nbv = (int*)malloc(sizeof(int) * 12);
		Gtest[i].weight = (double*)malloc(sizeof(double) * 12);
		if (Gtest[i].nbv == NULL || Gtest[i].weight == NULL){
			printf("System could not allocate more memory\n");
			exit(2);
		}
		Gtest[i].size = 12;
		Gtest[i].deg = 0;
		Gtest[i].vid = idx2node[i];
	}
	while (feof(rfile3) == false){
		int c = fscanf(rfile3, "%d\t%d\t%d\n", &i, &j, &w);
		if (c != 3){
			printf("the format of test pair file is not correct!\n");
			exit(2);
		}
		index1 = node2idx[i];
		index2 = node2idx[j];
		//dynamic allocate memory to adjacent list of node i
		if (Gtest[index1].size <= Gtest[index1].deg){
			Gtest[index1].size += 12;
			Gtest[index1].nbv = (int*)realloc(Gtest[index1].nbv, sizeof(int)*Gtest[index1].size);
			Gtest[index1].weight = (double*)realloc(Gtest[index1].weight, sizeof(double)*Gtest[index1].size);
			if (Gtest[index1].weight == NULL || Gtest[index1].nbv == NULL){
				printf("System could not allocate more memory\n");
				exit(2);
			}
		}
		cur = binarysearch(Gtest[index1].nbv, 0, Gtest[index1].deg, index2);
		if (cur < 0){
			insert(Gtest[index1].nbv, -cur - 1, index2, Gtest[index1].deg);
			insert(Gtest[index1].weight, -cur - 1, w, Gtest[index1].deg);
			Gtest[index1].deg++;
		}
	}
	fclose(rfile3);
	///////////////////////////////////////////////////////////////////////////
	//construct a test graph from test pairs (END)
	//////////////////////////////////////////////////////////////////////////
	char *AAfile = new char[500];
	strcpy(AAfile, argv[1]);
	strcat(AAfile, "AA.txt");
	FILE *wfile = fopen(AAfile, "w");
	if (wfile == NULL){
		printf("could not open file to write\n");
		exit(1);
	}

	for (i = 0; i < testnodenum; i++){
		index1 = G[i].vid;
		AA(G, index1, AAtarget, AAscore, pos, nodenum);
		for (j = 0; j < Gtest[i].deg; j++){
			index2 = Gtest[i].nbv[j];
			cur = binarysearch(AAtarget, 0, pos, G[index2].vid);
			if (cur >= 0)
				AAvalue = AAscore[cur];
			else
				AAvalue = 0;
			if (Gtest[i].weight[j]>0.00001)
				fprintf(wfile, "%lf\t1\n", AAvalue);
			else
				fprintf(wfile, "%lf\t0\n", AAvalue);

		}
	}
	fclose(wfile);
	delete[]AAfile;
	delete[]AAscore;
	delete[]AAtarget;
	delete[]idx2node;
	delete[]node2idx;
	releasegraphmemory(G, nodenum);
	releasegraphmemory(Gtest, testnodenum);
	if (G != NULL){
		free(G);
		G = NULL;
	}
	if (Gtest != NULL){
		free(Gtest);
		Gtest = NULL;
	}
	if (inputbuffer != NULL){
		free(inputbuffer);
		inputbuffer = NULL;
	}
	if (outputbuffer != NULL){
		free(outputbuffer);
		outputbuffer = NULL;
	}
	if (temp_string != NULL){
		delete[]temp_string;
		temp_string = NULL;
	}
	while (curBlk2 >= 0){
		if (memBlkAr2[curBlk2] != NULL){
			free(memBlkAr2[curBlk2]);
			curBlk2--;
		}
		else
			curBlk2--;
	}
	curBlk = 0;
	while (curBlk <= endBlk){
		if (memBlkAr[curBlk] != NULL){
			free(memBlkAr[curBlk]);
			curBlk++;
		}
		else
			curBlk++;
	}
}