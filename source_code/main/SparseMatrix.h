#ifndef SparseMatrix_h_
#define SparseMatrix_h_
#include"memoryfunction.h"
#include"Runtimecounter.h"
/*
This file implements the Sparse Matrix data structure

/*@author: linhong (linhong.seba.zhu@gmail.com)
*/
///----------------------------------------------------------------------//
class SparseMatrix{
public:
	Row * matrix;
	int n; //number of rows
	int m; //number of columns
public:
	SparseMatrix();
	SparseMatrix(const SparseMatrix &obj);
	SparseMatrix & operator=(const SparseMatrix & obj);  //override = operator
	void Initmemory(int rownum); //allocate memory for sparse matrix with rownum
	~SparseMatrix();
	void Addmemory(int snum); //add more memory for sparse matrix
	void Initrowmemory(int r,int dv); //allocate dv space memory for row r
	void addelement(int r, int index, double weight); //insert weight into row r and column index
	void clear();  //release the memory allocated for sparse matrix
	void deletemem(); //release the row structure memory
	void RowsumMultiply(int r, double c); //Row[r1]=Row[r1]+c*Row[r1];
	void RowMultiply(int r, double c);    //Row[r1]=c*Row[r1];
	double Rowdotproduct(int r);  //return res=Row[r] dot product Row[r]
	double Rowdotproduct2(int r1,int r2); //return res=Row [r1] dot product Row [r2]
	void RowsumLeftEqual(int r1,int r2, double c); //return Row[r1]=Row[r1]+Row[r2]*c;
	void RowsumLeftEqual(int r1,Row &entry2, double c); //return Row[r1]=Row[r1]+entry*c
	void RowsumLeftEqual(int r1, double *entry, double c); //return Row[r1]=Row[r1]+entry*c;
	void Gettransposesqure(double **&B);
	//given matrix Z, compute a B matrix with m*m such that B=Z^T * Z
	double Fnorm(); //return the Fnorm of Sparse matrix
	void Rownorm1(int r); //row sum equal to one normalization for row r
	void Rownorm2(int r); //row square sum equal to one normalization for row r
	void Writetofile(FILE *wfile, Node *&G); //save the sparse matrix into file
	void Multiplymatrix(int r1, double **B, double *&res); //res = Row(r1)*B, Row(r1)=1*m, B=m*m
	void Printrow(int r);
	void Shrinkrow(int r);
	void setcolumnnum(int intcnum){
		m=intcnum;
	}
};

SparseMatrix::SparseMatrix(){
	matrix=NULL;
}
SparseMatrix::SparseMatrix(const SparseMatrix &obj){
	this->m = obj.m;
	this->n=obj.n;
	matrix = (Row*)malloc(sizeof(Row)*n);
	if (matrix == NULL){
		printf("could not allocate more memory\n");
		exit(1);
	}
	for (int i = 0; i < n; i++){
		copyRow(matrix[i], obj.matrix[i]);
	}
}
SparseMatrix & SparseMatrix::operator=(const SparseMatrix & obj){
	if (this == &obj)
		return *this;
	this->m = obj.m;
	this->n = obj.n;
	matrix = (Row*)malloc(sizeof(Row)*n);
	if (matrix == NULL){
		printf("could not allocate more memory\n");
		exit(2);
	}
	for (int i = 0; i < n; i++){
		copyRow(matrix[i], obj.matrix[i]);
	}
	return *this;
}
void SparseMatrix::Initmemory(int rownum){
	n=rownum;
	matrix=(Row*)malloc(sizeof(Row)*n);
	if(matrix==NULL){
		printf("could not allocate more memory\n");
		exit(1);
	}
	for(int i=0;i<n;i++){
		matrix[i].weight=NULL;
		matrix[i].idx=NULL;
		matrix[i].clength=0;
		matrix[i].size=0;
	}
}
SparseMatrix::~SparseMatrix(){
}
void SparseMatrix::clear(){
	if (matrix != NULL){
		for (int i = 0; i<n; i++){
			if (sizeof(double)*matrix[i].size >= BLK_SZ2){
				if (matrix[i].idx != NULL){
					free(matrix[i].idx);
					matrix[i].idx = NULL;
				}
				if (matrix[i].weight != NULL){
					free(matrix[i].weight);
					matrix[i].weight = NULL;
				}
			}
			matrix[i].size = 0;
			matrix[i].clength = 0;
		}
	}
	this->n = 0;
	this->m = 0;
}
void SparseMatrix::deletemem(){
	if (matrix != NULL){
		free(matrix);
		matrix = NULL;
	}
}
void SparseMatrix::Addmemory(int snum){
	Row *tempres=(Row*)realloc(matrix,sizeof(Row)*(n+snum));
	if(tempres==NULL){
		printf("could not allocate more memory\n");
		exit(1);
	}
	matrix = tempres;
	for(int i=n;i<(n+snum);i++){
		matrix[i].weight=NULL;
		matrix[i].idx=NULL;
		matrix[i].clength=0;
		matrix[i].size=0;
	}
	n = n + snum;
}
void SparseMatrix::Initrowmemory(int r, int dv){
	/////////////////////////////////////////////////////////////
	//for each node v,
	//allocate memory space to store a row entry in sparse matrix (START)
	///////////////////////////////////////////////////////////
	if(dv==0){
		matrix[r].idx=NULL;
		matrix[r].weight=NULL;
		matrix[r].size=0;
	}else{
		if(sizeof(double)*dv>=BLK_SZ){
			matrix[r].idx=(int*)malloc(sizeof(int)*dv);
			matrix[r].weight=(double*)malloc(sizeof(double)*dv);
			if (matrix[r].idx==NULL||matrix[r].weight==NULL){
				printf("system could not allocate more memory\n");
				exit(0);
			}
		}else{
			allocatetmpmemory(sizeof(int)*dv);
			matrix[r].idx=(int *)curMemPos;
			curMemPos+=(sizeof(int)*dv);
			allocatetmpmemory(sizeof(double)*dv);
			matrix[r].weight=(double*)curMemPos;
			curMemPos+=(sizeof(double)*dv);
		}
		matrix[r].size=dv;
	}
	matrix[r].clength=0;
	/////////////////////////////////////////////////////////////
	//for each node v,
	//allocate memory space to store a row entry  (END)
	///////////////////////////////////////////////////////////
}
//add value (index,weight) to row r
void SparseMatrix::addelement(int r, int index,double weight){
	int pos=matrix[r].clength;
	matrix[r].idx[pos]=index;
	matrix[r].weight[pos]=weight;
	pos++;
	matrix[r].clength=pos;
}


//Row[r1]=c*Row[r1];
void SparseMatrix::RowMultiply(int r, double c){
	int i=0;
	for(i=0;i<matrix[r].clength;i++){
		matrix[r].weight[i]*=c;
	}
}
//Row[r1]=Row[r1]+c*Row[r1];
void SparseMatrix::RowsumMultiply(int r, double c){
	int i=0;
	for(i=0;i<matrix[r].clength;i++){
		matrix[r].weight[i]*=(1+c);
	}
}
double SparseMatrix::Rowdotproduct(int r){
	double result=0;
	for(int i=0;i<matrix[r].clength;i++){
		result+=matrix[r].weight[i]*matrix[r].weight[i];
	}
	return result;
}
/*
B m*m matric
*/
void SparseMatrix::Multiplymatrix(int r1, double **B, double *&res){
	int i = 0;
	int j = 0;
	int c;
	for (i = 0; i < m; i++)
		res[i] = 0;
	for (i = 0; i < matrix[r1].clength; i++){
		c = matrix[r1].idx[i];
		for (j = 0; j < m; j++){
			res[j] += matrix[r1].weight[i] * B[c][j];
		}
	}
}

void SparseMatrix::Gettransposesqure(double **&B){
	int i;
	int j;
	int k;
	int bi;
	int bj;
	for (i = 0; i < m; i++){
		for (j = 0; j < m; j++){
			B[i][j] = 0;
		}
	}
	for (i = 0; i < n; i++){
		for (j = 0; j < matrix[i].clength; j++){
			bi = matrix[i].idx[j];
			for (k = 0; k < matrix[i].clength; k++){
				bj = matrix[i].idx[k];
				B[bi][bj] += matrix[i].weight[j] * matrix[i].weight[k];
			}
		}
	}
}

double SparseMatrix::Rowdotproduct2(int r1,int r2){
	double result=0;
	int i=0;
	int j=0;
	while(i<matrix[r1].clength&&j<matrix[r2].clength){
		if(matrix[r1].idx[i]<matrix[r2].idx[j]){
			i++;
		}else
			if(matrix[r1].idx[i]>matrix[r2].idx[j]){
				j++;
			}else{
				result+=(matrix[r1].weight[i]*matrix[r2].weight[j]);
				i++;
				j++;
			}
	}
	return result;
}

void SparseMatrix::Printrow(int r){
	int i = 0;
	for (i = 0; i < matrix[r].clength; i++){
		cout << matrix[r].idx[i] << "\t" << matrix[r].weight[i] << endl;
	}
}
//Row[r1]=Row[r1]+Row[r2]*c;
void SparseMatrix::RowsumLeftEqual(int r1,int r2, double c){
	int i=0;
	int j=0;
	while(i<matrix[r1].clength&&j<matrix[r2].clength){
		if(matrix[r1].idx[i]<matrix[r2].idx[j]){
			i++;
		}else
			if(matrix[r1].idx[i]>matrix[r2].idx[j]){
				insert(matrix[r1], i, matrix[r2].idx[j], matrix[r2].weight[j]*c);
				i++;
				j++;
			}else{
				matrix[r1].weight[i]=matrix[r1].weight[i]+matrix[r2].weight[j]*c;
				i++;
				j++;
			}
	}
	while(j<matrix[r2].clength){
		insert(matrix[r1], i, matrix[r2].idx[j], matrix[r2].weight[j]*c);
		i++;
		j++;
	}
}
void SparseMatrix::RowsumLeftEqual(int r1,Row &entry2, double c){
	int i=0;
	int j=0;
	while(i<matrix[r1].clength&&j<entry2.clength){
		if(matrix[r1].idx[i]<entry2.idx[j]){
			i++;
		}else
			if(matrix[r1].idx[i]>entry2.idx[j]){
				insert(matrix[r1], i, entry2.idx[j], entry2.weight[j]*c);
				i++;
				j++;
			}
			else{
				matrix[r1].weight[i]=matrix[r1].weight[i]+entry2.weight[j]*c;
				i++;
				j++;
			}
	}
	while(j<entry2.clength){
		insert(matrix[r1], i, entry2.idx[j], entry2.weight[j]*c);
		i++;
		j++;
	}
}

void SparseMatrix::RowsumLeftEqual(int r1, double *entry,double c){
	int i = 0;
	int j = 0;
	while (i < matrix[r1].clength&&j < m){
		if (matrix[r1].idx[i] < j){
			i++;
		}
		else if (matrix[r1].idx[i]>j){
			if (entry[j] * c > 0.0000001){
				insert(matrix[r1], i, j, entry[j] * c);
				i++;
			}
			j++;
		}
		else{
			matrix[r1].weight[i] = matrix[r1].weight[i] + entry[j] * c;
			i++;
			j++;
		}
	}
	while (j < m){
		if (entry[j] * c > 0.0000001){
			insert(matrix[r1], i, j, entry[j] * c);
			i++;
		}
		j++;
	}
}
double SparseMatrix::Fnorm(){
	double res=0;
	for(int i=0;i<n;i++){
		for(int j=0;j<matrix[i].clength;j++){
			res=hypot(res,matrix[i].weight[j]);
		}
	}
	return res;
}
void SparseMatrix::Rownorm1(int r){
	double minvalue = 12345;
	for (int j = 0; j < matrix[r].clength; j++){
		if (minvalue < matrix[r].weight[j])
			minvalue = matrix[r].weight[j];
	}
	if (minvalue < 0){
		for (int j = 0; j < matrix[r].clength; j++){
			matrix[r].weight[j] -= minvalue;
			matrix[r].weight[j] += 0.0001;
		}
	}
	double sum = 0;
	for (int j = 0; j < matrix[r].clength; j++){
		sum += matrix[r].weight[j];
	}
	if (sum > 0){
		for (int j = 0; j < matrix[r].clength; j++){
			matrix[r].weight[j] /= sum;
		}
	}
}
void SparseMatrix::Rownorm2(int r){
	double minvalue = 12345;
	for (int j = 0; j < matrix[r].clength; j++){
		if (minvalue > matrix[r].weight[j])
			minvalue = matrix[r].weight[j];
	}
	if (minvalue < 0){
		for (int j = 0; j < matrix[r].clength; j++){
			matrix[r].weight[j] -= minvalue;
			matrix[r].weight[j] += 0.0001;
		}
	}
	double sum = 0;
	for (int j = 0; j < matrix[r].clength; j++){
		sum += matrix[r].weight[j]*matrix[r].weight[j];
	}
	sum = sqrt(sum);
	if (sum > 0){
		for (int j = 0; j < matrix[r].clength; j++){
			matrix[r].weight[j] /= sum;
			if (matrix[r].weight[j] < 0.0000001)
				matrix[r].weight[j] = 0;
		}
	}
}
void SparseMatrix::Shrinkrow(int r){
	char *precurMemStart = curMemPos;
	char *precurMemEnd = curMemEnd;
	int precurblk = curBlk;

	allocatetmpmemory(sizeof(double)*m*2);

	int *tempidx = (int*)curMemPos;
	curMemPos += (sizeof(int)*m);

	double *tempweight = (double*)curMemPos;
	curMemPos += (sizeof(double)*m);
	int l = 0;
	for (int j = 0; j < matrix[r].clength; j++){
		if (matrix[r].weight[j] > 0.0000001){
			tempidx[l] = matrix[r].idx[j];
			tempweight[l] = matrix[r].weight[j];
			l++;
		}
	}
	matrix[r].clength = l;
	for (int j = 0; j < l; j++){
		matrix[r].weight[j] = tempweight[j];
		matrix[r].idx[j] = tempidx[j];
	}
	curMemPos = precurMemStart;
	curMemEnd = precurMemEnd;
	curBlk = precurblk;
}
/*
Save the sparse matrix representation of node-community matrix into file in disk
Note that only the seed node's community representation is saved
*/
void SparseMatrix::Writetofile(FILE *wfile, Node *&G){
	Runtimecounter IORC;
	int j;
	int i;
	int d;
	int idx;
	double value;
	outcurpos = outputbuffer;
	if(wfile!=NULL){
		for(i=0;i<n;i++){
			if (G[i].vid<Vmin || G[i].vid>Vmax)
				continue;
			if (outendpos - outcurpos <= 100) {
				j = outcurpos - outputbuffer;
				IORC.start();
				fwrite(outputbuffer, sizeof(char), j, wfile);
				IORC.stop();
				iotime += IORC.GetRuntime();
				outcurpos = outputbuffer;
			}
			j=itostring(G[i].vid,outcurpos,10);
			outcurpos += j;
			*outcurpos = ',';
			outcurpos++;
			j=itostring(matrix[i].clength,outcurpos,10);
			outcurpos += j;
			for(d=0;d<matrix[i].clength;d++){
				idx=matrix[i].idx[d];
				value=matrix[i].weight[d];
				if (outendpos - outcurpos <= 200) {
					j = outcurpos - outputbuffer;
					IORC.start();
					fwrite(outputbuffer, sizeof(char), j, wfile);
					IORC.stop();
					iotime += IORC.GetRuntime();
					outcurpos = outputbuffer;
				}
				*outcurpos = ':';
				outcurpos++;
				j=itostring(idx,outcurpos,10);
				outcurpos += j;
				*outcurpos = ',';
				outcurpos++;
				j=dtostring(value,outcurpos);
				outcurpos += j;
			}
			if (outendpos - outcurpos < 20){
				j = outcurpos - outputbuffer;
				IORC.start();
				fwrite(outputbuffer, sizeof(char), j, wfile);
				IORC.stop();
				iotime += IORC.GetRuntime();
				outcurpos = outputbuffer;
			}
			*outcurpos = '\n';
			outcurpos++;
		}
		j=outcurpos-outputbuffer;
		fwrite(outputbuffer,sizeof(char),j,wfile);
		outcurpos=outputbuffer;
	}

}
#endif
