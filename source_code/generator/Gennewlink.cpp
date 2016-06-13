//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include"../main/function.h"
#include"../main/IOfunction.h"
//#include <crtdbg.h>
#define _CRT_SECURE_NO_WARNINGS

int main(int argc, char*argv[]){
	if (argc < 5){
		printf("%s [newgraphfile] [oldgraphfile] [out_put_prefix] [pairnumber] [randomprob] [0/1]\n", argv[0]);
		printf("0: newlinks\n");
		printf("1: anylinks\n");
		printf(".txt: source\tdestination\n");
		printf(".tsv 1: source\tdestination\tgt\n");
		printf("-pre.tsv 2: source\tdestination\tpregt\tgt\n");
		exit(1);
	}
	srand(time(NULL));
	Initmemory();
	InitIOmemory();
	double probthres = 0.5;
	FILE *rfile;
	//read the new graph (start)//
	rfile = fopen(argv[1], "r");
	if (rfile == NULL){
		printf("could not open file to read\n");
		exit(1);
	}
	int n;
	fscanf(rfile, "%d\n", &n);
	//read the new graph (start)//
	Node *G;
	ReadGraph(G, n, rfile);
	fclose(rfile);
	//read the new graph (end)//

	//read the past graph (start)//
	rfile = fopen(argv[2], "r");
	if (rfile == NULL){
		printf("could not open file to read\n");
		exit(1);
	}
	fscanf(rfile, "%d\n", &n);
	Node *Gpre;
	ReadGraph(Gpre, n, rfile);
	fclose(rfile);
	//read the past graph (end)//

	FILE *wfile; //write source destination format
	FILE *wfile2; //write source destination gt format
	FILE *wfile3; //write source destination pregt, newgt format
	char *outfile1 = new char[1000];
	char *outfile2 = new char[1000];
	char *outfile3 = new char[1000];
	strcpy(outfile1, argv[3]);
	strcpy(outfile2, argv[3]);
	strcpy(outfile3, argv[3]);
	strcat(outfile1, ".txt");
	strcat(outfile2, ".tsv");
	strcat(outfile3, "-pre.tsv");
	wfile = fopen(outfile1, "w");
	wfile2 = fopen(outfile2, "w");
	wfile3 = fopen(outfile3, "w");
	if (wfile == NULL||wfile2==NULL||wfile3==NULL){
		printf("could not open file to write\n");
		exit(2);
	}
	int m =10000;
	if(argc>4)
		m=atoi(argv[4]);
	if(argc>5)
		probthres = atof(argv[5]);
	int type=0;
	if(argc>6)
		type=atoi(argv[6]);
	FILE *wfile4 = NULL;
	char *outfile;
	if (type == 1){
		outfile= new char[1000];
		strcpy(outfile, argv[3]);
		strcat(outfile, "Gpre.txt");
		wfile4 = fopen(outfile, "w");
		if (wfile4 == NULL){
			printf("could not open file to write\n");
			exit(2);
		}
	}
	int pcount;
	int ncount;
	int a;
	int b;
	vector<int> newlink(n); //keep the target nodes of new links;
	vector<int> dellink(n);//keep the target nodes of del links;
	double prob;
	if (type == 1){
		pcount = 0;
		ncount = 0;
		int pregt;
		while (pcount < m){
			for (int i = 0; i < n; i++){
				if (pcount >= m || ncount >= m)
					break;
				//start generate random linked pairs
				for (int j = 0; j < G[i].deg; j++){
					if (pcount == m)
						break;
					a = rand();
					b = rand();
					prob = (double)a / (a + b);
					if (prob < probthres){
						int cur = binarysearch(Gpre[i].nbv, 0, Gpre[i].deg, G[i].nbv[j]);
						if (cur < 0)
							pregt = 0;
						else
							pregt = 1;
						fprintf(wfile, "%d\t%d\n", i, G[i].nbv[j]);
						fprintf(wfile2, "%d\t%d\t1\n", i, G[i].nbv[j]);
						fprintf(wfile3, "%d\t%d\t%d\t1\n", i, G[i].nbv[j], pregt);
						fprintf(wfile4, "%d\t1\n", pregt);
						pcount++;
					}
				}
			}
		}//finish generate random linked pairs

		////////////////////////////////////////////
		//Generate random non-linked pairs(start)//
		//////////////////////////////////////////
		while (ncount < m){
			a = rand();
			b = rand();
			a = a%n;
			b = b%n;
			int cur = binarysearch(G[a].nbv, 0, G[a].deg, b);
			if (cur < 0){
				int cur2 = binarysearch(Gpre[a].nbv, 0, Gpre[a].deg, b);
				if (cur2 < 0)
					pregt = 0;
				else
					pregt = 1;
				fprintf(wfile, "%d\t%d\n", a, b);
				fprintf(wfile2, "%d\t%d\t0\n", a, b);
				fprintf(wfile3, "%d\t%d\t%d\t0\n", a, b, pregt);
				fprintf(wfile4, "%d\t0\n", pregt);
				ncount++;
			}
		}//finish generate random non-linked pairs
		//Generate random non-linked pairs(end)///
		//////////////////////////////////////////
	}
	else{
		pcount = 0;
		ncount = 0;
		for (int i = 0; i < n; i++){
			if (pcount >= m || ncount >= m)
				break;
			newlink.clear();
			dellink.clear();
			setdifference(G[i].nbv, G[i].deg, Gpre[i].nbv, Gpre[i].deg, newlink, dellink);
			for (int j = 0; j < (int)newlink.size(); j++){
				if (pcount == m)
					break;
				a = rand();
				b = rand();
				prob = (double)a / (a + b);
				if (prob < probthres){
					fprintf(wfile, "%d\t%d\n", i, newlink.at(j));
					fprintf(wfile2, "%d\t%d\t1\n", i, newlink.at(j));
					fprintf(wfile3, "%d\t%d\t0\t1\n", i, newlink.at(j));
					pcount++;
				}
			}//finish output new links
			for (int j = 0; j < (int)dellink.size(); j++){
				if (ncount == m)
					break;
				a = rand();
				b = rand();
				prob = (double)a / (a + b);
				if (prob < probthres){
					fprintf(wfile, "%d\t%d\n", i, dellink.at(j));
					fprintf(wfile2, "%d\t%d\t0\n", i, dellink.at(j));
					fprintf(wfile3, "%d\t%d\t1\t0\n", i, dellink.at(j));
					ncount++;
				}
			}//finish output deleted links
		}//finish all nodes
		while (ncount < pcount){
			a = rand();
			b = rand();
			a = a%n;
			b = b%n;
			int cur = binarysearch(G[a].nbv, 0, G[a].deg, b);
			int pregt;
			if (cur < 0){
				int cur2 = binarysearch(Gpre[a].nbv, 0, Gpre[a].deg, b);
				if (cur2 < 0)
					pregt = 0;
				else
					pregt = 1;
				fprintf(wfile, "%d\t%d\n", a, b);
				fprintf(wfile2, "%d\t%d\t0\n", a, b);
				fprintf(wfile3, "%d\t%d\t%d\t0\n", a, b, pregt);
				ncount++;
			}
		}//finish generate random non-linked pairs
	}
	fclose(wfile);
	fclose(wfile2);
	fclose(wfile3);
	if (type == 1){
		fclose(wfile4);
		delete[]outfile;
	}
	newlink.clear();
	//comment out C++11 function
	//newlink.shrink_to_fit();
	dellink.clear();
	//comment out c++11 function
	//dellink.shrink_to_fit();
	delete[]outfile1;
	delete[]outfile2;
	delete[]outfile3;
	////////////////////////////////////////////////////
	//Release memory usage of a graph (START)
	////////////////////////////////////////////////////
	releasegraphmemory(G, n);
	if (G != NULL){
		free(G);
		G = NULL;
	}
	releasegraphmemory(Gpre, n);
	if (Gpre != NULL){
		free(Gpre);
		Gpre = NULL;
	}
	releaseIOmemory();
	releaseblockmemory();
	////////////////////////////////////////////////////
	//Release memory usage of a graph (END)
	////////////////////////////////////////////////////
	//only for windows when debuging memory leaks
	//_CrtDumpMemoryLeaks();
	return 0;
}