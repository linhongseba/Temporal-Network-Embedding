#include"../main/SparseMatrix.h"
#include"Evaluate.h"
/*
Error.cpp:
create time: 05/19/015
purpose: 
1) compute the F-norm error in a sperate function
2) compute the average z(u).z(v)T for (u,v) edge pairs

sub component for 1)
read the Z matrix
read the G graph
Compute Gpre=ZZ^T
compute W=G-Gpre;
Compute Fnorm of W

sub component for 2)
read the Z matrix
read the G graph
for each (u, v) edge pair
compute the z(u).Z(v)T
compute the average
*/
void exit_with_help(char *name){
	printf("Usage: %s [argument]\n", name);
	printf("-t: 1 input is a data folder\n");
	printf("-t: 2 input is a graph file\n");
	printf("-g: [graphfile/dir]\n");
	printf("-z: [latent space file/dir]\n");
	exit(1);
}


int main(int argc, char *argv[]){
	if (argc < 2){
		exit_with_help(argv[0]);
	}
	Initmemory();
	InitIOmemory();
	int i;
	int type=1;
	allocatetmpmemory(sizeof(char) * 1000);
	char *filename = (char*)curMemPos;
	curMemPos += (sizeof(char) * 500);
	char *cfile = (char*)curMemPos;
	curMemPos += (sizeof(char) * 500);
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
		case 'z':
			if (i >= argc - 1)
				exit_with_help(argv[0]);
			strcpy(cfile, argv[i + 1]);
			i++;
			break;
		default:
			exit_with_help(argv[0]);
		}
	}
	if (type == 1){
		vector<char*> graphfiles;
		vector<char*> cfiles;
		listfilename(graphfiles, filename);
		listfilename(cfiles, cfile);
		if (graphfiles.size() != cfiles.size()){
			printf("error in data\n");
			exit(1);
		}
		double reserror = 0;
		double preerror = 0;
		for (i = 0; i < (int)graphfiles.size(); i++){
			cout << "reconstruction evaluation" << endl;
			reserror+=Evaluateerror(graphfiles.at(i), cfiles.at(i));
			//Evaluateallinks(graphfiles.at(i), cfiles.at(i), "./res");
			if (i >= 1){
				cout << "prediction evaluation" << endl;
				preerror+=Evaluateerror(graphfiles.at(i), cfiles.at(i - 1));
				//Evaluateallinks(graphfiles.at(i), cfiles.at(i - 1), "./pre");
				//EvaluateRep(graphfiles.at(i), graphfiles.at(i - 1), cfiles.at(i-1));
			}
		}
		reserror /= graphfiles.size();
		preerror /= (graphfiles.size() - 1);
		cout << "total reconstruction error is " << reserror << endl;
		cout << "total prediction error is " << preerror << endl;
	}
	else{
		Evaluateerror(filename, cfile);
	}
	releaseIOmemory();
	releaseblockmemory();
}