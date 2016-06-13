#include"../main/function.h"
#include"Evaluate.h"
#include"../main/dirent.h"
#include"../main/memoryfunction.h"
#define _CRT_SECURE_NO_WARNINGS
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
/*
This file implements the link prediction evaluation algorithm

/*@author: linhong (linhong.seba.zhu@gmail.com)
*/
///----------------------------------------------------------------------//
void exit_with_help(char *name){
	printf("Usage: %s [option]\n", name);
	printf("-t: 1 input is a folder\n");
	printf("-t: 2 input is a file\n");
	printf("-z: [latent space file/dir]\n");
	printf("-g: [pre graph file/dir]\n");
	printf("-f: [test pair file]\n");
	printf("-l: 1, 2,link function selection\n");
	exit(1);
}
int main(int argc, char *argv[]){
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	if (argc < 2){
		exit_with_help(argv[0]);
	}
	Initmemory();
	InitIOmemory();
	int i = 0;
	int type;
	int ltype = 1;
	allocatetmpmemory(sizeof(char) * 1500);
	char *cfile = (char*)curMemPos;
	curMemPos += (sizeof(char) * 500);
	char *bfile = (char*)curMemPos;
	curMemPos += (sizeof(char) * 500);
	char *gfile = (char*)curMemPos;
	curMemPos += (sizeof(char) * 500);
	strcpy(bfile, " ");
	for (i = 1; i<argc; i++){
		if (argv[i][0] != '-') break;
		switch (argv[i][1]){
		case 't':
			if (i >= argc - 1)
				exit_with_help(argv[0]);
			type = atoi(argv[i + 1]);
			i++;
			break;
		case 'z':
			if (i >= argc - 1)
				exit_with_help(argv[0]);
			strcpy(cfile, argv[i + 1]);
			i++;
			break;
		case 'l':
			if (i >= argc - 1)
				exit_with_help(argv[0]);
			ltype = atoi(argv[i + 1]);
			i++;
			break;
		case 'g':
			if (i >= argc - 1)
				exit_with_help(argv[0]);
			strcpy(gfile, argv[i + 1]);
			i++;
			break;
		case 'f':
			if (i >= argc - 1)
				exit_with_help(argv[0]);
			strcpy(bfile, argv[i + 1]);
			i++;
			break;
		default:
			exit_with_help(argv[0]);
		}
	}

	if (type == 1){
		vector<char*> cfiles;
		vector<char*> gfiles;
		listfilename(cfiles, cfile);
		listfilename(gfiles, gfile);
		if (cfiles.size() != gfiles.size()){
			cout << "number of training graphs is not equal to number of latent spaces" << endl;
			exit(3);
		}
		if (strcmp(bfile, " ") != 0){
			//listfilename(bfiles, bfile);
			vector<double> *allresults = new vector<double>[cfiles.size()];
			for (int j = 0; j < (int)cfiles.size(); j++){
				allresults[j].reserve(20000);
				Predictionlinks(cfiles.at(j), bfile, allresults[j],gfiles.at(j),ltype);
			}
			Writeresults(allresults, cfiles.size(), bfile);
			//release memory of allresults;
			for (int j = 0; j < (int)cfiles.size(); j++){
				allresults[j].clear();
				//comment out because it only availables in C++11
				//allresults[j].shrink_to_fit();
			}
			delete[]allresults;
			//bfiles.clear();
		}
		cfiles.clear();
		//function only availables in C++11
		//cfiles.shrink_to_fit();
	}
	else{
		if (strcmp(bfile, " ") != 0){
			Predictionlinks(cfile, bfile,gfile,ltype);
		}
	}
	releaseIOmemory();
	releaseblockmemory();
	//_CrtDumpMemoryLeaks();
	return 0;
}