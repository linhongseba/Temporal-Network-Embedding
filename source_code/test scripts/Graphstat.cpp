#include"../main/IOfunction.h"
int main(int argc, char *argv[]){
	srand(time(NULL));
	Initmemory();
	InitIOmemory();
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
	int m = 0;
	for (int i = 0; i < n; i++){
		m += G[i].deg;
	}
	m = m / 2;
	cout << "number of edges " << m << endl;
	releasegraphmemory(G, n);
	if (G != NULL){
		free(G);
		G = NULL;
	}
	releaseIOmemory();
	releaseblockmemory();
	return 0;
}
