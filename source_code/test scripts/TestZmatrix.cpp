//#define _CRTDBG_MAP_ALLOC
//#define _CRT_SECURE_NO_WARNINGS
#include"../main/gradiantmethod.h"
#include"../main/dirent.h"
#include"../evaluation/Evaluate.h"
#include"../main/IncrementalBCGD.h"
//#include <crtdbg.h> 
//only for windows debug
int main(int argc, char *argv[]){
	Initmemory();
	SparseMatrix Z;
	Z.Initmemory(10);
	Z.setcolumnnum(3);
	for (int i = 0; i < 10; i++){
		Z.Initrowmemory(i,3);
	}
	for (int i = 0; i < 10; i++){
		Z.addelement(i, 0, 0.2);
		Z.addelement(i, 1, 0.5);
		Z.addelement(i, 2, 1.0 / (i + 1));
	}
	for (int i = 0; i < 10; i++){
		Z.Rownorm2(i);
	}
	double **B = new double*[3];
	for (int i = 0; i < 3; i++)
		B[i] = new double[3];
	Z.Gettransposesqure(B);
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			printf("%lf ", B[i][j]);
		}
		printf("\n");
	}
	double fnorm = 0;
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			fnorm += B[i][j] * B[i][j];
		}
	}
	fnorm = sqrt(fnorm);
	printf("%lf\n", fnorm);
	double *res = new double[3];
	Z.Multiplymatrix(1, B, res);
	for (int i = 0; i < 3; i++){
		printf("%lf ", res[i]);
	}
	printf("\n");
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			if (i == j)
				B[i][j] -= 1;
		}
	}
	fnorm = 0;
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			fnorm += B[i][j] * B[i][j];
		}
	}
	fnorm = sqrt(fnorm);

	printf("%lf\n", fnorm);
	delete []res;
	Z.clear();
	Z.deletemem();
	delete[]B;
	releaseblockmemory();
}