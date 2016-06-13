#ifndef memoryfunction_h_
#define memoryfunction_h_
#include"function.h"
/*
This file implements the memory functions which dynamically allocate memory for graphs and community matrices

/*@author: linhong (linhong.seba.zhu@gmail.com)
*/
///----------------------------------------------------------------------//
//========================================================================================================//
//functions that are related to memory management//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

inline void Initmemory(){
	/////////////////////////////////////////////////////////////
	////////////initializing memory block (START)////////////////
	////////////////////////////////////////////////////////////
	//Initialize memory blocks (START)
	curBlk = endBlk = 0;
	memBlkAr[0] = curMemPos = new char[BLK_SZ];
	curMemEnd = curMemPos + BLK_SZ;
	//Initialize memory blocks (END)
	//Initialize memory blocks (START)
	curBlk2 = 0;
	memBlkAr2[0] = curMemPos2 = new char[BLK_SZ2];
	curMemEnd2 = curMemPos2 + BLK_SZ2;
	//Initialize memory blocks (END)
	/////////////////////////////////////////////////////////
	////////////initializing memory block (end)////////////////
	////////////////////////////////////////////////////////
}
inline void InitIOmemory(){
	////////////////////////////////////////////////////////////
	///////////initializing IO memory block (START)/////////////
	///////////////////////////////////////////////////////////
	BYTE_TO_READ = 60000000;
	inputbuffer = new char[BYTE_TO_READ];
	outputbuffer = new char[BYTE_TO_READ];
	outcurpos = outputbuffer;
	outendpos = outputbuffer + BYTE_TO_READ;
	temp_string = new char[128];
	////////////////////////////////////////////////////////////
	///////////initializing IO memory block (START)/////////////
	///////////////////////////////////////////////////////////
}
inline void InitTempSubmemory(int maxnodenum){
    //Initilize the memory usage (start)
    //group id used in community initilization
    groupid = new int[maxnodenum];
    if (groupid == NULL){
        printf("could not allocate more memory\n");
        exit(2);
    }
    memset(groupid, 0, sizeof(int)*maxnodenum);
    //a temp array with integer for storing temp values
    tempidsize = new int[maxnodenum];
    if (tempidsize == NULL){
        printf("could not allocate more memory\n");
        exit(2);
    }
    memset(tempidsize, 0, sizeof(int)*maxnodenum);
    //a temp array with double for storing temp values
    tempvalue = new double[maxnodenum];
    if (tempvalue == NULL){
        printf("could not allocate more memory\n");
        exit(2);
    }
    //isseednode: a char array which used to label nodes
    isseednode =new char[maxnodenum];
    if (isseednode == NULL){
        printf("could not allocate more memory\n");
        exit(2);
    }
    memset(isseednode, 'v', sizeof(char)*maxnodenum);
    //node2subidx: an integer array which maps node id in
    //original graph to node id in sub graphs
    node2subidx = new int [maxnodenum];
    if (node2subidx == NULL){
        printf("could not allocate more memory\n");
        exit(2);
    }
    //Initilize the memory usage (end)
}
inline void releaseInitZmemory(){
    if (tempidsize != NULL){
        delete[]tempidsize;
        tempidsize = NULL;
    }
    if (groupid != NULL){
        delete[]groupid;
        groupid = NULL;
    }
}
inline void releaseTempSubmemory(){
    if (isseednode != NULL){
        delete []isseednode;
        isseednode = NULL;
    }
    if (node2subidx != NULL){
        delete []node2subidx;
        node2subidx = NULL;
    }
    if (tempvalue != NULL){
        delete []tempvalue;
        tempvalue = NULL;
    }
}
inline void InitTempFullmemory(int maxnodenum){
    //Initilize the memory usage (start)
    //group id used in community initilization
    groupid = new int[maxnodenum];
    if (groupid == NULL){
        printf("could not allocate more memory\n");
        exit(2);
    }
    memset(groupid, 0, sizeof(int)*maxnodenum);
    //a temp array with integer for storing temp values
    tempidsize = new int[maxnodenum];
    if (tempidsize == NULL){
        printf("could not allocate more memory\n");
        exit(2);
    }
    memset(tempidsize, 0, sizeof(int)*maxnodenum);
    //a temp array with double for storing temp values
    tempvalue = new double[maxnodenum];
    if (tempvalue == NULL){
        printf("could not allocate more memory\n");
        exit(2);
    }
    //Initilize the memory usage (end)

}
//check whether tmpblock can allocate a memory block with length size
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
				curMemPos = new char[BLK_SZ];
				if(curMemPos==NULL){
					printf("system is unable to allocate more temporal memory\n");
					printf("number of block is %d\n",curBlk);
					printf("number of static block is %d\n",curBlk2);
					exit(2);
				}
				memBlkAr[++curBlk]=curMemPos;
				curMemEnd=curMemPos+BLK_SZ;
			}
		}//end of if free mem is not enough
	}

//check whether the pemblock can allocate a memory block with length size
void inline allocatepermemory(unsigned int size){
		if(size>=(size_t)(curMemEnd2 - curMemPos2)||(curMemPos2==NULL)||(curMemEnd2 - curMemPos2)>BLK_SZ2){//free mem in cur block is not enough
			//free mem in cur block is not enough
			//allocate a new block
			if(curBlk2<BLK_NUM2-1){
				++curBlk2;
				curMemPos2=new char[BLK_SZ2];
				if(curMemPos2==NULL){
					printf("system is unable to allocate more static memory\n");
					printf("number of block is %d\n",curBlk);
					printf("number of static block is %d\n",curBlk2);
					exit(2);
				}
				memBlkAr2[curBlk2]=curMemPos2;
				curMemEnd2=curMemPos2+BLK_SZ2;
			}else{
				printf("system is unable to allocate more static memory\n");
				exit(2);
			}
		}
}

/*
 insert (index,v) into position pos at row into a m-dimension sparse matrix, usually m is very small <100
 */
inline void insert(Row &rowvector, int pos, int index, double v, int m){
    if (rowvector.clength >= rowvector.size){
        rowvector.size=m;
        unsigned int mysize = sizeof(double)*rowvector.size;
        mysize += (sizeof(double)*rowvector.size);
        allocatetmpmemory(mysize);
        double *newweight = (double*)curMemPos;
        curMemPos += (sizeof(double)*rowvector.size);
        int *newidx = (int*)curMemPos;
        curMemPos += (sizeof(int)*rowvector.size);
        if (rowvector.clength > 0){
            memcpy(newweight, rowvector.weight, sizeof(double)*rowvector.clength);
            memcpy(newidx, rowvector.idx, sizeof(int)*rowvector.clength);
        }
        rowvector.weight = newweight;
        rowvector.idx = newidx;
    }
    int k = 0;
    for (k = rowvector.clength; k > pos; k--){
        rowvector.idx[k] = rowvector.idx[k - 1];
        rowvector.weight[k] = rowvector.weight[k - 1];
    }
    rowvector.idx[pos] = index;
    rowvector.weight[pos] = v;
    rowvector.clength++;
}

/*
insert (index,v) into position pos at row in sparse matrix 
*/
inline void insert(Row &rowvector, int pos, int index, double v){
	if (rowvector.clength >= rowvector.size){
		if (rowvector.size*sizeof(double) >= BLK_SZ2){

			double *tempdre = (double*)realloc(rowvector.weight, sizeof(double)*(rowvector.size + 10));
			int *tempire= (int*)realloc(rowvector.idx, sizeof(int)*(rowvector.size + 10));
			if (tempdre == NULL || tempire == NULL){
				printf("could not allocate more memory\n");
				exit(2);
			}
			rowvector.weight = tempdre;
			rowvector.idx = tempire;
			rowvector.size += 10;
		}
		else{
			if ((rowvector.size + 10)*sizeof(double) >= BLK_SZ2){
				double *newweight = (double*)malloc(sizeof(double)*(rowvector.size + 10));
				int *newidx = (int*)malloc(sizeof(int)*(rowvector.size + 10));
				if (newweight == NULL || newidx == NULL){
					printf("could not allocate more memory\n");
					exit(2);
				}
				memcpy(newweight, rowvector.weight, sizeof(double)*rowvector.clength);
				memcpy(newidx, rowvector.idx, sizeof(int)*rowvector.clength);
				rowvector.weight = newweight;
				rowvector.idx = newidx;
				rowvector.size += 10;
			}
			else{
				rowvector.size += 10;
				unsigned int mysize = sizeof(double)*rowvector.size;
				mysize += (sizeof(double)*rowvector.size);
				allocatetmpmemory(mysize);
				double *newweight = (double*)curMemPos;
				curMemPos += (sizeof(double)*rowvector.size);
				int *newidx = (int*)curMemPos;
				curMemPos += (sizeof(int)*rowvector.size);
				if (rowvector.clength > 0){
					memcpy(newweight, rowvector.weight, sizeof(double)*rowvector.clength);
					memcpy(newidx, rowvector.idx, sizeof(int)*rowvector.clength);
				}
				rowvector.weight = newweight;
				rowvector.idx = newidx;
			}
		}
	}
	int k = 0;
	for (k = rowvector.clength; k > pos; k--){
		rowvector.idx[k] = rowvector.idx[k - 1];
		rowvector.weight[k] = rowvector.weight[k - 1];
	}
	rowvector.idx[pos] = index;
	rowvector.weight[pos] = v;
	rowvector.clength++;
}

/*
deepcopy, allocate memory to dest,
copy value from src to dest
*/
inline void copyRow(Row &dest, Row &src){
	dest.size = src.size;
	dest.clength = src.clength;
	if (sizeof(double)*dest.size >= BLK_SZ2){
		dest.weight = (double*)malloc(sizeof(double)*dest.size);
		dest.idx = (int*)malloc(sizeof(int)*dest.size);
		if (dest.weight == NULL&&dest.idx == NULL){
			printf("system could not allocate more memory\n");
			exit(2);
		}
	}
	else{
		allocatetmpmemory(sizeof(int)*dest.size);
		dest.idx = (int *)curMemPos;
		curMemPos+= (sizeof(int)*dest.size);
		allocatetmpmemory(sizeof(double)*dest.size);
		dest.weight = (double*)curMemPos;
		curMemPos += (sizeof(double)*dest.size);
	}
	for (int i = 0; i < dest.clength; i++){
		dest.weight[i] = src.weight[i];
		dest.idx[i] = src.idx[i];
	}
}

/*
release graph memory where G is in Node structure
*/
inline void releasegraphmemory(Node *&G, int gnodenum){
	int i = 0;
	for (i = 0; i < gnodenum; i++){
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
		G[i].weight = NULL;
	}
}
/*
release graph memory where G is in DNode structure
*/
inline void releasegraphmemory(DNode *&G, int gnodenum){
	int i = 0;
	for (i = 0; i < gnodenum; i++){
		if (G[i].nbv != NULL){
			free(G[i].nbv);
			G[i].nbv = NULL;
		}
		if (G[i].weight != NULL){
			free(G[i].weight);
			G[i].weight = NULL;
		}
		G[i].vid = -1;
		G[i].deg = 0;
		G[i].size = 0;
		G[i].nbv = NULL;
		G[i].weight = NULL;
	}
}

/*
release preallocated block memory
*/
inline void releaseblockmemory(){
	while (curBlk2 >= 0){
		if (memBlkAr2[curBlk2] != NULL){
			delete[] memBlkAr2[curBlk2];
			curBlk2--;
		}
		else
			curBlk2--;
	}
	curBlk = 0;
	while (curBlk <= endBlk){
		if (memBlkAr[curBlk] != NULL){
			delete[]memBlkAr[curBlk];
			curBlk++;
		}
		else
			curBlk++;
	}
}
/*
release preallocated IO memory
*/
inline void releaseIOmemory(){
	if (inputbuffer != NULL){
		delete[]inputbuffer;
		inputbuffer = NULL;
	}
	if (outputbuffer != NULL){
		delete[]outputbuffer;
		outputbuffer = NULL;
	}
    if(filebuffer!=NULL){
        delete[]filebuffer;
        filebuffer=NULL;
    }
	if (temp_string != NULL){
		delete[]temp_string;
		temp_string = NULL;
	}
}
#endif


