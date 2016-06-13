#ifndef function_h_
#define function_h_
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string>
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include"dirent.h"
#define _CRT_SECURE_NO_WARNINGS
using namespace std;
/*
This files declares and defines the basic variables,
and struct of all the projects
*/
/*@author: linhong (linhong.seba.zhu@gmail.com)
*/
///----------------------------------------------------------------------//

/////////////////////////////////////////////////////////////////////////
//global variable definition (start)//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//variables that are related to memory management
//===========================================================================================================//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Parameters for tmp memory management (START)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int BLK_NUM=1024;
const int BLK_SZ=4194304;	//1024*1024*4
//const int BLK_SZ = 24;	//1024*1024*4

char *memBlkAr[BLK_NUM];	//An array of pointers to allocated blocks
char *curMemPos=NULL;	//The first free pos (startpos) in the first free block for writing
char *curMemEnd=NULL;	//The end pos of the first free block for writing
int curBlk=0;  //curBlk: the ID of the current block
int endBlk=0;  //endBlk: the ID of the last allocated block
									
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Parameters for tmp memory management (END)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Parameters for permanent memory management (START)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int BLK_NUM2=1024;
const int BLK_SZ2=4194304;	//1024*1024*4

char *memBlkAr2[BLK_NUM2];	//An array of pointers to allocated blocks
char *curMemPos2=NULL;	//The first free pos (startpos) in the first free block for writing
char *curMemEnd2=NULL;	//The end pos of the first free block for writing
int curBlk2=0;	//curBlk: the ID of the current block
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Parameters for permanent memory management (END)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int * node2subidx=NULL;
char *isseednode=NULL;  //an array where each entry indicates whether a node is seed or not
int BYTE_TO_READ=6000000; //number of bytes that read into input buffer and output buffer
char *inputbuffer=NULL;   //input buffer
char *outputbuffer=NULL;  //output buffer
char *filebuffer=NULL;
char *outcurpos=NULL;     //current cursor of outputbuffer
char *outendpos=NULL;     //end cursor of outputbuffer
char *curpos=NULL;       //current cursor of inputbuffer
char *endpos=NULL;      //current end cursor of inputbuffer
char *endpos0=NULL;
int byteread=0;        //an temporal variable
//char *readbuffer=NULL;
//int readbuffersize=10000;
double iotime=0;
int totalfinish=0;
int Vmin=0;
int Vmax=0;
char *temp_string=NULL;// used for store value of inline function itostring
double *tempvalue=NULL; //used for store temp double value array with size equal to node number
int *tempidsize=NULL;  //used for store temp int value array with size equal to node number
int *groupid=NULL; //initialize an non-overlapping partitioning
int maxiter;
double alpha;
double zeta;
double delta;
/////////////////////////////////////////////////////////////////////////
//global variable definition (END)//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/////////////global structure definition(start)//////////////////////////
/////////////////////////////////////////////////////////////////////////


struct Node{
	int deg;//degree
	double *weight;//its weighted adjacency lists
	int vid;//id of node
	int *nbv;//its adjacency lists
	//note that size of allocated memory=degree
};

// Node structure that the memory associated with neighbors and weights can be dynamically reallocated
struct DNode{
	int deg;//degree
	double *weight;//its weighted adjacency lists
	int *nbv;//its adjacency lists
	int size; //size of allocated memory
	int vid;
};

struct Row{
	int clength;
	double *weight;
	int *idx;
	int size;//size of allocated memory
};

//////////////////////////////////////////////////////////////////////////
/////////////global structure definition(end)///////////////////////
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////global function definition (start)///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

//binary search over an integer array
inline int binarysearch(int *arrays, int l, int h, int v){
	int cur=l;
	int low=l;
	int high=h;
	while(low<high){
		cur=low+((high-low)/2);
		if(arrays[cur]==v)
				return cur;
			else{
				if(arrays[cur]>v)
					high=cur;
				else
					low=cur+1;
			}
		}
	return -low-1;
}
//binary search over an integer vector
inline int binarysearch(vector<int> &arrays, int l, int h, int v){
	int cur=l;
	int low=l;
	int high=h;
	while(low<high){
		cur=low+((high-low)/2);
		if(arrays.at(cur)==v)
				return cur;
			else{
				if(arrays.at(cur)>v)
					high=cur;
				else
					low=cur+1;
			}
		}
	return -low-1;
}

//insert a value into the pos-th position of an integer array
inline void insert(int *&arrays, int pos, int v, int length){
	int k=0;
	for(k=length;k>pos;k--)
		arrays[k]=arrays[k-1];
	arrays[pos]=v;
}

//insert a value into the pos-th position of a double array
inline void insert(double *&arrays, int pos, double v, int length){
	int k=0;
	for(k=length;k>pos;k--)
		arrays[k]=arrays[k-1];
	arrays[pos]=v;
}

//check whether two arrays that are with same length are identical or not
inline bool isidentical(int *&array1, int *&array2, int length){
	int i = 0;
	while (i < length){
		if (array1[i] == array2[i])
			i++;
		else
			return false;
	}
	return true;
}


//compute the differences of two sorted array and store the differences into result array
inline void setdifference(int *first, int l1, int *second, int l2, int *&result, int &length){
	int i = 0;
	int j = 0;
	length = 0;
	while (i<l1&&j<l2){
		if (first[i]<second[j]){

			result[length] = first[i];
			i++;
			length++;
		}
		else
			if (first[i] == second[j]){
			i++;
			j++;
			}
			else{
				j++;
			}
	}
	while (i<l1){
		result[length] = first[i];
		length++;
		i++;
	}
}

inline void setdifference(int *first, int l1, int *second, int l2, vector<int>&add, vector<int> &del){
	int i = 0;
	int j = 0;
	while (i<l1&&j<l2){
		if (first[i]<second[j]){
			add.push_back(first[i]);
			i++;
		}
		else
			if (first[i] == second[j]){
				i++;
				j++;
			}
			else{
				del.push_back(second[j]);
				j++;
			}
	}
	while (i<l1){
		add.push_back(first[i]);
		i++;
	}
	while (j < l2){
		del.push_back(second[j]);
		j++;
	}
}

//remove an element from pos-th position of an integer array
inline void removeelement(int *&arrays, int pos, int length){
	int k=0;
	for(k=pos;k<length-1;k++)
		arrays[k]=arrays[k+1];
}

//remove an element from pos-th position of a double array
inline void removeelement(double *&arrays, int pos, int length){
	int k=0;
	for(k=pos;k<length-1;k++)
		arrays[k]=arrays[k+1];
}

/////////////////////////////////////////////////////////////////////
//implementation of quick sort algorithm (start)
////////////////////////////////////////////////////////////////////
int partition (int *&a, int  *&b, int l, int r){
	int pivot=a[l];
	int i=l+1;
	int j=l+1;
	int temp=0;
	for(j=l+1;j<=r;j++){
		if(a[j]<pivot){
			temp=a[i];
			a[i]=a[j];
			a[j]=temp;
			temp=b[i];
			b[i]=b[j];
			b[j]=temp;
			i++;
		}
	}
	temp=a[i-1];
	a[i-1]=a[l];
	a[l]=temp;
	temp=b[i-1];
	b[i-1]=b[l];
	b[l]=temp;
	return i-1;
}

 int quicksort1(int *&a, int *&b,int l, int r){
	if(l<r){
		int p=partition(a,b,l,r);
		quicksort1(a,b,l,p-1);
		quicksort1(a,b,p+1,r);
		return 0;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////
//implementation of quick sort algorithm (END)
////////////////////////////////////////////////////////////////////

 //parse an integer to string
inline int itostring(int value, char *& string, int radix){
	char *tp = temp_string;
	int i;
	unsigned vm;
	int sign;
	char *sp;
	int length=0;
	if (radix > 36 || radix <= 1){
		return 0;
	}
	sign = (radix == 10 && value < 0);
	if (sign)
		vm = -value;
	else
		vm = (unsigned)value;
	while (vm || tp == temp_string){
		i = vm % radix;
		vm = vm / radix;
		if (i < 10)
			*tp++ = i+'0';
		else
			*tp++ = i + 'a' - 10;
	}
	if (string == NULL)
		string = (char *)malloc((tp-temp_string)+sign+1);
	sp = string;
	if (sign){
		*sp++ = '-';
		length++;
	}
	while (tp > temp_string){
		*sp++ = *--tp;
		length++;
	}
	//*sp++='\0';
	//*sp = 0;
	return length;
}

//parse a double to string (n is length of string)
inline int dtostring (double value, char *& valuestring){
	int n=sprintf(valuestring,"%.8lf", value);
	return n;
} 

void GraphNorm(Node *&G, int N){
	for (int i = 0; i < N; i++){
		double rsum = 0;
		for (int j = 0; j < G[i].deg; j++){
			rsum += G[i].weight[j];
		}
		if (rsum > 0){
			for (int j = 0; j < G[i].deg; j++){
				G[i].weight[j] /= rsum;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////global function definition (end)////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
#endif