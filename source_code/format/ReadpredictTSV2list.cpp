#include"../main/function.h"
#include"../main/memoryfunction.h"
#include"../main/IOfunction.h"
int main(int argc, char *argv[]){
	if (argc < 2){
		printf("Usage:%s [graphfile] [tsvfile] [outputfile]\n", argv[0]);
		exit(1);
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
	temp_string = new char[4096];
	////////////////////////////////////////////////////////////
	///////////initializing IO memory block (END)/////////////
	///////////////////////////////////////////////////////////

	FILE *rfile = fopen(argv[1], "r");
	if (rfile == NULL){
		printf("could not open graph file to read\n");
		exit(1);
	}
	int nodenum;
	fscanf(rfile, "%d\n", &nodenum);
	Node *G;
	ReadGraph(G, nodenum, rfile);
	fclose(rfile);
	FILE *rfile2 = fopen(argv[2], "r");
	if (rfile2 == NULL){
		printf("could not open TSV file to read\n");
		exit(1);
	}
	int i;
	int j;
	double value;
	int cur = 0;
	FILE *wfile = fopen(argv[3], "w");
	if (wfile == NULL){
		printf("could not open file to write\n");
		exit(1);
	}
	while (feof(rfile2)==false){
		fscanf(rfile2, "%d%d%lf\n", &i, &j, &value);
		cur = binarysearch(G[i].nbv, 0, G[i].deg, j);
		if (cur >= 0){
			fprintf(wfile, "%lf\t1\n", value);
		}
		else{
			fprintf(wfile, "%lf\t0\n", value);
		}

	}
	fclose(wfile);
	fclose(rfile2);
	////////////////////////////////////////////////////
	//Release memory usage of a graph (START)
	////////////////////////////////////////////////////
	for (int i = 0; i<nodenum; i++){
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
	if (G != NULL){
		free(G);
		G = NULL;
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
	////////////////////////////////////////////////////
	//Release memory usage of a graph (END)
	////////////////////////////////////////////////////
	return 0;
}
