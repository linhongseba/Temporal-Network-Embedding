#include"IOfunction.h"
#include"SparseMatrix.h"
int main(int argc, char *argv[]){
	if(argc<2){
		printf("Usage: %s [input_embedding_file][output_file]\n", argv[0]);
		exit(0);
	}
	Initmemory();
	InitIOmemory();
	FILE *rfile;
	rfile = fopen(argv[1], "r");
	if (rfile == NULL){
		printf("could not open file to read\n");
		exit(1);
	}
	int nodenum;
	SparseMatrix Z;
	Readcommunity(Z, nodenum, rfile);
	if (rfile != NULL){
		fclose(rfile);
	}
	for (int i = 0; i < nodenum; i++) {
		Z.RownormMax(i);
		Z.Rownorm2(i);
	}
	FILE *wfile = fopen(argv[2], "w");
	if (wfile == NULL){
		printf("could not write result to disk\n");
		exit(1);
	}
	fprintf(wfile, "%d\n", nodenum);
	Z.Writetofile(wfile);
	if (wfile != NULL) {
		fclose(wfile);
	}
	releaseIOmemory();
	releaseblockmemory();
}