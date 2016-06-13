//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include"../main/function.h"
#include"../main/IOfunction.h"
//#include <crtdbg.h>
//#define _CRT_SECURE_NO_WARNINGS
int main(int argc, char*argv[]){
	if (argc < 4){
		printf("%s [graphfile] [pair_number] [out_put_file]\n", argv[0]);
		exit(1);
	}
	srand(time(NULL));
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
	filebuffer = (char*)malloc(BYTE_TO_READ);
	outcurpos = outputbuffer;
	outendpos = outputbuffer + BYTE_TO_READ;
	temp_string = new char[65];
	FILE *rfile;
	rfile = fopen(argv[1], "r");
	if (rfile == NULL){
		printf("could not open file to read\n");
		exit(1);
	}
	int n;
	fscanf(rfile, "%d\n", &n);
	Node *G = (Node *)malloc(sizeof(Node)*n);
	ReadGraph(G, n, rfile);
	fclose(rfile);
	FILE *wfile;
	int m = atoi(argv[2]);
	if (argc>3)
		wfile = fopen(argv[3], "w");
	else
		wfile = fopen("text.tsv", "w");
	if (wfile == NULL){
		printf("could not open file to write\n");
		exit(1);
	}
	if (m == 0){
		for (int i = 0; i < n; i++){
			for (int j = i + 1; j < n; j++){
				fprintf(wfile, "%d\t%d\n", i, j);
			}
		}
	}
	else{
		int pcount = 0;
		double prob;
		int a;
		int b;
		while (pcount<m){
			for (int i = 0; i < n; i++){
				if (pcount >= m)
					break;
				for (int j = 0; j < G[i].deg; j++){
					if (i < G[i].nbv[j])
						continue;
					a = rand();
					b = rand();
					prob = (double)a / (a + b);
					if (prob < 0.1){
						fprintf(wfile, "%d\t%d\n", i, G[i].nbv[j]);
						pcount++;
						if (pcount == m)
							break;
					}
				}
			}
		}
		pcount = 0;
		do{
			a = rand();
			b = rand();
			a = a%n;
			b = b%n;
			int cur = binarysearch(G[a].nbv, 0, G[a].deg, b);
			if (cur < 0){
				fprintf(wfile, "%d\t%d\n", a, b);
				pcount++;
				if (pcount == m)
					break;
			}
		} while (pcount<m);
	}
	fclose(wfile);
	////////////////////////////////////////////////////
	//Release memory usage of a graph (START)
	////////////////////////////////////////////////////
	for (int i = 0; i<n; i++){
		if (G[i].deg*sizeof(double) >= BLK_SZ2){
			if (G[i].nbv != NULL){
				free(G[i].nbv);
				G[i].nbv = NULL;
			}
			if (G[i].weight != NULL){
				free(G[i].weight);
				G[i].weight = NULL;
			}
		}
		G[i].vid = -1;
		G[i].deg = 0;
		G[i].nbv = NULL;
	}
	if(G != NULL){
		free(G);
		G = NULL;
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
	////////////////////////////////////////////////////
	//Release memory usage of a graph (END)
	////////////////////////////////////////////////////
	//_CrtDumpMemoryLeaks();
	return 0;
}