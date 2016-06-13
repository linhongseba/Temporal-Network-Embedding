#define _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
const int BYTE_TO_READ=60000000;
char *inputbuffer=NULL;
char *curpos=NULL;
char *endpos=NULL;
char *endpos0=NULL;
int byteread=0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Parameters for tmp memory management (START)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int BLK_NUM=1024;
const int BLK_SZ=4194304;	//1024*1024*4

char *memBlkAr[BLK_NUM];	//An array of pointers to allocated blocks
char *curMemPos=NULL;	//The first free pos (startpos) in the first free block for writing
char *curMemEnd=NULL;	//The end pos of the first free block for writing
int curBlk=0;
int endBlk=0;	//curBlk: the ID of the current block
//endBlk: the ID of the last allocated block
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Parameters for tmp memory management (END)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
inline void allocatetmpmemory(unsigned int size);
inline void allocatepermemory(unsigned int size);
inline int binarysearch(int *&arrays, int l, int h, int v);
inline void insert(int *&arrays, int pos, int v, int length);
inline void FillInputBuffer(FILE *inFile);
inline void GetData(char *dest,int byte2read, FILE *inFile);
//binearysearch: put bst as an array, for any given v,
//search it agaist arrays, if found, return its position
//otherwise, record the largest position low that arrays[low]<v;
//return -low-1;
inline int binarysearch(int *&arrays, int l, int h, int v){
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

//insert v into arrays, make sure the index of arrays is not out of range
inline void insert(int *&arrays, int pos, int v, int length){
	int k=0;
	for(k=length;k>pos;k--)
		arrays[k]=arrays[k-1];
	arrays[pos]=v;
}
void inline allocatetmpmemory(unsigned int size){
	if(size>=(size_t)(curMemEnd - curMemPos)||(curMemPos==NULL)||(curMemEnd - curMemPos)>BLK_SZ){//free mem in cur block is not enough
		if(curBlk < endBlk){
			//we have already allocated free blocks
			curMemPos=memBlkAr[++curBlk];
			curMemEnd=curMemPos+BLK_SZ;
		}else{
			//allocate a new block
			++endBlk;
			if(endBlk>=BLK_NUM){
				printf("system is unable to allocate more temporal memory\n");
				printf("number of block is %d\n",endBlk);
				exit(0);
			}
			curMemPos=(char*)malloc(BLK_SZ);
			if(curMemPos==NULL){
				printf("system is unable to allocate more temporal memory\n");
				printf("number of block is %d\n",curBlk);
				printf("number of static block is %d\n",curBlk2);
				exit(0);
			}
			memBlkAr[++curBlk]=curMemPos;
			curMemEnd=curMemPos+BLK_SZ;
		}
	}//end of if free mem is not enough
}
void inline allocatepermemory(unsigned int size){
	if(size>=(size_t)(curMemEnd2 - curMemPos2)||(curMemPos2==NULL)||(curMemEnd2 - curMemPos2)>BLK_SZ2){//free mem in cur block is not enough
		//free mem in cur block is not enough
		//allocate a new block
		if(curBlk2<BLK_NUM2-1){
			++curBlk2;
			curMemPos2=(char*)malloc(BLK_SZ2);
			if(curMemPos2==NULL){
				printf("system is unable to allocate more static memory\n");
				printf("number of block is %d\n",curBlk);
				printf("number of static block is %d\n",curBlk2);
				exit(0);
			}
			memBlkAr2[curBlk2]=curMemPos2;
			curMemEnd2=curMemPos2+BLK_SZ2;
		}else{
			printf("system is unable to allocate more static memory\n");
			exit(0);
		}
	}
}

inline void FillInputBuffer(FILE *inFile){
	int i=0;
	endpos=endpos0;
	if(endpos > curpos){
		i=endpos-curpos;
		//this essentially moves data after curpos to the start of the buffer
		//and then call GetData to fill data to the position starting from endpos
		memmove(inputbuffer,curpos,i);
		endpos=inputbuffer+i;
		//get data from Input file
		GetData(endpos,BYTE_TO_READ-i,inFile);	//new end position
	}
	else{	
		endpos=inputbuffer;
		//get data from Input file
		GetData(endpos,BYTE_TO_READ,inFile);	//new end position
	}

	endpos+=byteread;

	if(!feof(inFile)){
		//we seek back to the end of the last complete line
		endpos0=endpos--;

		while(*endpos != '\n'){
			--endpos;
		}
		++endpos;	//endpos is one pos after '\n'
	}

	curpos=inputbuffer;
}
inline void GetData(char *dest,int byte2read, FILE *inFile){
	if(!feof(inFile)){
		byteread=fread(dest,1,byte2read,inFile);
		//		printf("inputbufferB[0] = %d, inputbufferB[1] = %d, inputbufferB[2] = %d, inputbufferB[3] = %d, inputbufferB[4] = %d, inputbufferB[5] = %d \n", inputbufferB[0],inputbufferB[1],inputbufferB[2],  inputbufferB[3],inputbufferB[4],inputbufferB[5]);
		//		exit(1);
		if(byteread < byte2read){
			//end of file or more to read
			if(!feof(inFile)){
				//Tests if a error has occurred in the last reading or writing operation with the given stream,
				//returning a non-zero value in case of error.
				if(ferror(inFile)){
					perror("Error reading from input file. Abort!\n");
					exit(1);
				}
			}
		}
	}
}
int main(int argc, char *argv[]){
	if(argc<2){
		printf("Usage: %s [graphfilename]\n",argv[0]);
		exit(1);
	}
	/////////////////////////////////////////////////////////
	////////////initlizing memory block (START)////////////////
	////////////////////////////////////////////////////////
	//Initialize memory blocks (START)
	curBlk=endBlk=0;
	memBlkAr[0]=curMemPos=(char*)malloc(BLK_SZ);
	curMemEnd=curMemPos+BLK_SZ;
	//Initialize memory blocks (END)
	//Initialize memory blocks (START)
	curBlk2=0;
	memBlkAr2[0]=curMemPos2=(char*)malloc(BLK_SZ2);
	curMemEnd2=curMemPos2+BLK_SZ2;
	//Initialize memory blocks (END)
	/////////////////////////////////////////////////////////
	////////////initlizing memory block (end)////////////////
	//////////////////////////////////////////////////////
	FILE *GHfile=fopen(argv[1],"rb");
	if(GHfile==NULL){
		printf("could not open the graph file to read\n");
		exit(0);
	}
	char *newfilename=(char*)malloc(sizeof(char)*300);
	strcpy(newfilename,argv[1]);
	strcat(newfilename,"_n.txt");
	FILE *wfile=fopen(newfilename,"w");
	if(wfile==NULL){
		printf("could not open the graph file to write\n");
		exit(0);
	}
	int nodenum=0;
	int linenum=0;
	fscanf(GHfile,"%d\n",&nodenum);
	//fscanf(GHfile,"%d %d\n",&nodenum,&linenum);
	//fscanf(GHfile,"%d %d\n",&nodenum,&linenum);
	//fprintf(wfile,"%d\n",nodenum);
	inputbuffer=(char*)malloc(BYTE_TO_READ);
	if(inputbuffer==NULL){
		printf("could not allocate so much memory\n");
		exit(0);
	}
	curpos=endpos0=inputbuffer;	//initialize the data position pointers to the start of the buffer
	FillInputBuffer(GHfile);
	int i=0;
	int j=0;
	int v=0;
	int dv=0;
	int neighbore=0;
	int cur=0;
	i=0;
	int maxdeg=0;
	double weight;
	while(i<nodenum){
		v=0;
		do{
			v=(10 * v)+int(*curpos)-48;
		}while(*(++curpos) != ',');
		//printf("node id now is %d\n",v);
		if(v<0||v>=nodenum){
			while(*curpos!='\n')
				curpos++;
			if(++curpos >= endpos){
				if(feof(GHfile))
					break;
				FillInputBuffer(GHfile);
			}
			//i++;
			continue;
		}
		dv=0;
		if(*(++curpos) != '0'){
			dv=0;
			do{
				dv=(10 * dv)+int(*curpos)-48;
			}while(*(++curpos) != ':');
			if(dv>maxdeg)
				maxdeg=dv;
			/////////////////////////////////////////////////////////////
			//for each node v,
			//allocate memory space to store a heap entry (dv,v,nb(v)) (START)
			///////////////////////////////////////////////////////////
			if(v%100000==0){
				printf("%d,%d\n",v,dv);
			}
			j=0;
			while(j<dv){
				++curpos;
				neighbore = 0;
				do{
					neighbore = (10 * neighbore) + int(*curpos) - 48;
				} while (*(++curpos) != ',');
				if (neighbore < 0 || neighbore >= nodenum){
					printf("error,please check line %d\n", v);
					exit(1);
				}
				int integer = 0;
				++curpos;
				while (*curpos > '/' && *curpos < ':'){
					//	while(*(++curpos) != '.'){
					integer = (10 * integer) + int(*curpos++) - 48;
				}

				int dp = 1;
				double decimal = 0;
				if (*curpos == '.'){
					++curpos;
					while (*curpos > '/' && *curpos < ':'){
						decimal = (10 * decimal) + int(*curpos++) - 48;
						dp *= 10;
					}
				}
				weight = integer + decimal / (double)dp;
				fprintf(wfile,"%d %d 1\n",v,neighbore);
				j++;
			}
			while(*curpos!='\n')
				curpos++;
		}//end: if(*(++curpos) != '0')
		else{
			while(*curpos!='\n')
				curpos++;
		}
		if(++curpos >= endpos){
			if(feof(GHfile))
				break;
			FillInputBuffer(GHfile);
		}
		i++;
	}	//end: while(1)
	free(inputbuffer);
	inputbuffer=NULL;
	while(curBlk2 >= 0){
		if(memBlkAr2[curBlk2]!=NULL){
			free(memBlkAr2[curBlk2]);
			curBlk2--;
		}else
			curBlk2--;
	}

	curBlk=0;
	while(curBlk <= endBlk){
		if(memBlkAr[curBlk]!=NULL){
			free(memBlkAr[curBlk]);
			curBlk++;
		}else
			curBlk++;
	}

	//////////////////////////////////////////////////////
	//free used temporal and permanent memory space (end)
	////////////////////////////////////////////////////////
	fclose(GHfile);
	fclose(wfile);
}