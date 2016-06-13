#include"function.h"
#include"Evaluate.h"
#include"dirent.h"
#include"memoryfunction.h"
#define _CRT_SECURE_NO_WARNINGS
/*
This file implements the link prediction evaluation algorithm

/*@author: linhong (linhong.seba.zhu@gmail.com)
*/
///----------------------------------------------------------------------//
void exit_with_help(char *name){
	printf("Usage: %s [option]\n", name);
	printf("-t: 1 input is a data folder\n");
	printf("-t: 2 input is a graph file\n");
	printf("-g: [graphfile/dir]\n");
	printf("-c: [community file/dir]\n");
	printf("-f: [test pair file]\n");
	exit(1);
}
int listfilename(vector<char*> &filenames, char *dirname){
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(dirname)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_name[0] != '.'){
				printf("%s\n", ent->d_name);
				allocatetmpmemory(sizeof(char) * 500);
				char *name = (char*)curMemPos;
				curMemPos += (sizeof(char) * 500);
				strcpy(name, dirname);
				strcat(name, "/");
				strcat(name, ent->d_name);
				printf("%s\n", name);
				filenames.push_back(name);
			}
		}
		closedir(dir);
		return 0;
	}
	else {
		/* could not open directory */
		perror("");
		return EXIT_FAILURE;
	}
}
int main(int argc, char *argv[]){
	if (argc < 2){
		exit_with_help(argv[0]);
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
	int i = 0;
	int type;
	allocatetmpmemory(sizeof(char) * 1500);
	char *filename = (char*)curMemPos;
	curMemPos += (sizeof(char) * 500);
	char *cfile = (char*)curMemPos;
	curMemPos += (sizeof(char) * 500);
	char *bfile = (char*)curMemPos;
	curMemPos += (sizeof(char) * 500);
	strcpy(bfile, " ");
	for (i = 1; i<argc; i++){
		if (argv[i][0] != '-') break;
		switch (argv[i][1]){
		case 'g':
			if (i >= argc - 1)
				exit_with_help(argv[0]);
			strcpy(filename, argv[i + 1]);
			i++;
			break;
		case 't':
			if (i >= argc - 1)
				exit_with_help(argv[0]);
			type = atoi(argv[i + 1]);
			i++;
			break;
		case 'c':
			if (i >= argc - 1)
				exit_with_help(argv[0]);
			strcpy(cfile, argv[i + 1]);
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
		vector<char*> graphfiles;
		vector<char*> bfiles;
		vector<char*> cfiles;
		listfilename(graphfiles, filename);
		listfilename(cfiles, cfile);
		if (graphfiles.size() != cfiles.size() ){
			printf("error in data\n");
			exit(1);
		}
		if (strcmp(bfile, " ") != 0){
			//listfilename(bfiles, bfile);
			for (int j = 0; j < graphfiles.size(); j++){
				Evaluatetestlinks(graphfiles.at(j), cfiles.at(j), bfile);
			}
			//bfiles.clear();
		}
		else{
			for (int j = 0; j < graphfiles.size(); j++){
				Evaluateallinks(graphfiles.at(j), cfiles.at(j));
			}
		}
		graphfiles.clear();
		cfiles.clear();
	}
	else{
		if (strcmp(bfile, " ") != 0){
			Evaluatetestlinks(filename, cfile, bfile);
		}else
			Evaluateallinks(filename, cfile);
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

	//_CrtDumpMemoryLeaks();
	return 0;
}