#ifndef IOfunction_h_
#define IOfunction_h_
#include"function.h"
#include"SparseMatrix.h"
#include"memoryfunction.h"
#include"Runtimecounter.h"
/*
This file implements the IO functions which read/write graph and community files

/*@author: linhong (linhong.seba.zhu@gmail.com)
*/
///----------------------------------------------------------------------//

int listfilename(vector<char*> &filenames, char *dirname){
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(dirname)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_name[0] != '.'){
				//printf("%s\n", ent->d_name);
				allocatetmpmemory(sizeof(char) * 500);
				char *name = (char*)curMemPos;
				curMemPos += (sizeof(char) * 500);
				strcpy(name, dirname);
				strcat(name, "/");
				strcat(name, ent->d_name);
				//printf("%s\n", name);
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
//==================================================================================
//////////////////////////////////////////////////////////////////////////////////
//Each time we read a fixed number of bits into input file buffer (start)
//=================================================================================
inline void GetData(char *dest, int byte2read, FILE *inFile){
	if (!feof(inFile)){
		byteread = fread(dest, 1, byte2read, inFile);
		//		printf("inputbufferB[0] = %d, inputbufferB[1] = %d, inputbufferB[2] = %d, inputbufferB[3] = %d, inputbufferB[4] = %d, inputbufferB[5] = %d \n", inputbufferB[0],inputbufferB[1],inputbufferB[2],  inputbufferB[3],inputbufferB[4],inputbufferB[5]);
		//		exit(1);
		if (byteread < byte2read){
			//end of file or more to read
			if (!feof(inFile)){
				//Tests if a error has occurred in the last reading or writing operation with the given stream,
				//returning a non-zero value in case of error.
				if (ferror(inFile)){
					perror("Error reading from input file. Abort!\n");
					exit(1);
				}
			}
		}
	}
}
//==================================================================================
//////////////////////////////////////////////////////////////////////////////////
//Each time we read a fixed number of bits into input file buffer (end)
//=================================================================================
//===============================================================================
//////////////////////////////////////////////////////////////////////////////////
//Read data into input file buffer (start)
//================================================================================
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
//===============================================================================
//////////////////////////////////////////////////////////////////////////////////
//Read data into input file buffer (end)
//================================================================================




//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//parse the value of vertex id from char * type input buffer
//===================================================================================
inline void Parseid(int &v){
	v=0;
	do{
		v=(10 * v)+int(*curpos)-48;
	}while(*(++curpos) != ',');
}

//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//Skip lines
//===================================================================================
inline void Skipline(){
	while(*curpos!='\n')
		curpos++;
}

//================================================================================
//parse the value of node degree from char *type input buffer
//================================================================================
inline void Parsedeg(int &dv, int nodenum){
	dv=0;
	if(*(++curpos) != '0'){
		dv=0;
		do{
			dv=(10 * dv)+int(*curpos)-48;
		}while(*(++curpos) != ':');
		if(dv<0||dv>nodenum){
			printf("error in graph degree\n");
			exit(1);
		}
	}
}

//==============================================================================
//parse the value of neighbore id and its weight from char *type input buffer
//==============================================================================
inline void Parseneighbor(int &neighbore, double &weight, int nodenum, int v){
	++curpos;
	neighbore = 0;
	do{
		neighbore=(10 * neighbore)+int(*curpos)-48;
	}while(*(++curpos) != ',');
	if(neighbore<0||neighbore>=nodenum){
		printf("error,please check line %d\n",v);
		exit(1);
	}
	int integer=0;
	++curpos;
	while(*curpos > '/' && *curpos < ':'){
		//	while(*(++curpos) != '.'){
		integer=(10 * integer)+int(*curpos++)-48;
	}

	double dp=1;
	double decimal=0;
	if(*curpos == '.'){
		++curpos;
		while(*curpos > '/' && *curpos < ':'){
			decimal=(10 * decimal)+int(*curpos++)-48;
			dp*=10;
		}
	}
	weight = integer + decimal / dp;

}

//================================================================================
//Scan input graph file to select seed node based on the size of bounded memory
//The input graph file is with the following format
////Input text format (START)
//u,n:v1,v1_weight:v2,v2_weight:...vn,vn_weight
//u,0
//Example:
//3,3:0,1.0842:1,2.21:5,7
//4,1:2,1.00001
//6,0
//Input text format (END)
///////////////////////////////////////////////////////////////////////////////////////////////////
//===============================================================================
inline void ReadSeeNode(int &gnodenum, int nodenum, int avgdeg,double memorybound, FILE *inFile){
	Runtimecounter IORC;
	IORC.start();
	curpos=endpos0=inputbuffer;	//initialize the data position pointers to the start of the buffer
	FillInputBuffer(inFile);
	int i=0;
	int v=0;
	int dv=0;
	double weight;
	int neighbore=0;
	int j=0;
	double sumSsize=0;
	/////////////////////////////////////////////////////////////////////////////
	//////Select base node/ seed node///////////////////////////////////////////
	/////make base node and its neighbors as 't'(start)///////////////////////
	/////base node is in the range of vmin and vmax///////////////////////////
	while(1){
		////////////////////////////////////
		//starting base node/seed node selection
		Parseid(v);
		//graphs that has been processed before
		//just skip
		if (v >= nodenum){
			printf("error in graph ID\n");
			exit(1);
		}
		if(v<=totalfinish){
			Skipline();
			//when adjacent list is not in inputbuffer
			//reload a portion of inFile into inputbuffer
			if(++curpos >= endpos){
				if(feof(inFile))
					break;
				FillInputBuffer(inFile);
			}
			continue;
		}//end of skip
		if((sumSsize/1024)<memorybound){
			if (v < Vmin)
				Vmin = v;
			if (v > Vmax)
				Vmax = v;
			if(isseednode[v]!='t'){
				isseednode[v]='t';
				gnodenum++;
			}
			Parsedeg(dv,nodenum);
			j=0;
			while(j<dv){
				Parseneighbor(neighbore,weight,nodenum,v);
				if(isseednode[neighbore]!='t'){
					isseednode[neighbore]='t';
					gnodenum++;
				}
				j++;
			}
			while(*curpos!='\n')
				curpos++;
			if(dv>0){
				sumSsize+=sizeof(Node);
				sumSsize+=(sizeof(int)*dv*avgdeg);
				sumSsize += (sizeof(double)*dv*avgdeg);
				sumSsize += (sizeof(double) * 20);
			}
			else{
				sumSsize+=sizeof(Node);
				sumSsize += (sizeof(double) * 20);
			}
		}else
			break;
		//when adjacent list is not in inputbuffer
		//reload a portion of inFile into inputbuffer
		if(++curpos >= endpos){
			if(feof(inFile))
				break;
			FillInputBuffer(inFile);
		}//end of reload
	}
	rewind(inFile);//back to the start position of inFile
	int rv=fscanf(inFile,"%d\n",&nodenum);
	IORC.stop();
	iotime+=IORC.GetRuntime();
	/////////////////////////////////////////////////////////////////////////////
	//////Select base node/ seed node///////////////////////////////////////////
	/////make base node and its neighbores as 't'(end)/////////////////////////
	/////base node is in the range of vmin and vmax///////////////////////////
	//============================================================================//
}
inline void ReadSubGraph(Node *&G, int gnodenum, int nodenum,FILE *inFile){
	Runtimecounter IORC;
	IORC.start();
	int i=0;
	int v=0;
	int dv=0;
	double weight;
	int neighbore=0;
	int j=0;
	///////////////////////////////////////////////////////////////////////////////////////////////
	//////////scanning the graph again to initializing the S and Splus nodes' adjacent lists(start)
	//////////////////////////////////////////////////////////////////////////////////////////////
	G=(Node*)malloc(sizeof(Node)*gnodenum);
	if(G==NULL){
		printf("system could not allocate more memory\n");
		exit(0);
	}
	for(i=0;i<gnodenum;i++){
		G[i].vid=-1;
		G[i].deg=0;
		G[i].nbv=NULL;
		G[i].weight=NULL;
	}
	curpos=endpos0=inputbuffer;	//initialize the data position pointers to the start of the buffer
	FillInputBuffer(inFile);
	//starting scanning
	int pos=0;
	while(1){
		Parseid(v);
		//skip the lines where the format of graph has some problem
		if(v<0||v>=nodenum){
			Skipline();
			if(++curpos >= endpos){
				if(feof(inFile))
					break;
				FillInputBuffer(inFile);
			}
			continue;
		}//end of skip "bad" lines

		//skip the line where vid is smaller than vmin, i.e., the nodes that 
		//have been processed
		//skip the nodes that are not S and S+ nodes
		if(isseednode[v]!='t'){
			Skipline();
			if(++curpos >= endpos){
				if(feof(inFile))
					break;
				FillInputBuffer(inFile);
			}
			continue;
		}
		if(pos>=gnodenum){
			gnodenum+=10;
			Node *Gtemp=(Node*)realloc(G,sizeof(Node)*gnodenum);
			if(Gtemp==NULL){
				printf("could not allocate more memory\n");
				exit(0);
			}
			G = Gtemp;
		}
		G[pos].vid=v;
		node2subidx[v]=pos;
		dv=0;
		Parsedeg(dv,nodenum);
		if(dv>0){
			/////////////////////////////////////////////////////////////
			//for each node v,
			//allocate memory space to store a heap entry (dv,v,nb(v)) (START)
			///////////////////////////////////////////////////////////
			if(sizeof(double)*dv>=BLK_SZ2){
					G[pos].nbv=(int*)malloc(sizeof(int)*dv);
					G[pos].weight=(double*)malloc(sizeof(double)*dv);
					if (G[pos].nbv == NULL || G[pos].weight == NULL){
						printf("system could not allocate more memory\n");
						exit(2);
					}
				}else{
                    allocatetmpmemory(sizeof(int)*dv);
					G[pos].nbv=(int *)curMemPos;
					curMemPos+=(sizeof(int)*dv);
                    allocatetmpmemory(sizeof(double)*dv);
					G[pos].weight=(double*)curMemPos;
					curMemPos+=(sizeof(double)*dv);
			}

			/////////////////////////////////////////////////////////////
			//for each node v,
			//allocate memory space to store a heap entry (dv,v,nb(v)) (END)
			///////////////////////////////////////////////////////////
			
			/////////////////////////////////////////////////////////////////////////
			//read and initialize the value of neighbore index and weight (START)
			//////////////////////////////////////////////////////////////////////////
			j=0;
			G[pos].deg=0;
			while(j<dv){
				Parseneighbor(neighbore,weight,nodenum,v);
				if(isseednode[neighbore]=='t'){
					G[pos].nbv[G[pos].deg]=neighbore;
					G[pos].weight[G[pos].deg]=weight;
					G[pos].deg++;
				}
				j++;
			}
			/////////////////////////////////////////////////////////////////////////
			//read and initialize the value of neighbore index and weight (END)
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////
			///////resize the allocated memory to save memory (start)
			//previous G[pos].nbv, G[pos].deg is allocated with globe degree
			//Now shrink the size to the local degree in the subgraph
			///////////////////////////////////////////////////////////////////
			if(sizeof(double)*dv>=BLK_SZ2&&sizeof(double)*G[pos].deg<BLK_SZ2){
                if(G[pos].deg>0){
                    allocatetmpmemory(sizeof(int)*G[pos].deg);
                    int *tempnbv=(int*)curMemPos;
                    curMemPos+=(sizeof(int)*G[pos].deg);
                    memcpy(tempnbv,G[pos].nbv,sizeof(int)*G[pos].deg);
                    if(G[pos].nbv!=NULL){
                        free(G[pos].nbv);
                    }
                    G[pos].nbv=tempnbv;
                    tempnbv=NULL;
                    allocatetmpmemory(sizeof(double)*G[pos].deg);
                    double *tempweight=(double*)curMemPos;
                    curMemPos+=(sizeof(double)*G[pos].deg);
                    memcpy(tempweight,G[pos].weight,sizeof(double)*G[pos].deg);
                    if(G[pos].weight!=NULL)
                        free(G[pos].weight);
                    G[pos].weight=tempweight;
                    tempweight=NULL;
                }
			}
			//////////////////////////////////////////////////////////////////
			///////resize the allocated memory to save memory (end)
			///////////////////////////////////////////////////////////////////
			while(*curpos!='\n')
				curpos++;
		}//end: if(deg >0)
		else{
			G[pos].deg=0;
			G[pos].nbv=NULL;
			G[pos].weight=NULL;
			while(*curpos!='\n')
				curpos++;
		}
		pos++;
		if(++curpos >= endpos){
			if(feof(inFile))
				break;
			FillInputBuffer(inFile);

		}
	}//end: while(i)
	///////////////////////////////////////////////////////////////////////////////////////////////
	//////////scanning the graph again to initializing the S and Splus nodes' adjacent lists(end)
	//////////////////////////////////////////////////////////////////////////////////////////////
	rewind(inFile);
	int rv=fscanf(inFile,"%d\n",&nodenum);
	IORC.stop();
	iotime+=IORC.GetRuntime();
}

inline void Readsubcommunity(SparseMatrix &Z, int gnodenum,int nodenum,FILE *inFile){
	Runtimecounter IORC;
	IORC.start();
	int i=0;
	int v=0;
	int dv=0;
	double weight;
	int neighbore=0;
	int maxidx = 0;
	int j=0;
	int pos=0;
	Z.Initmemory(gnodenum);
	curpos=endpos0=inputbuffer;	//initialize the data position pointers to the start of the buffer
	FillInputBuffer(inFile);
	//starting scanning
	while(1){
		if (pos >= nodenum){
			printf("bad operation in file openning\n");
			exit(2);
		}
		Parseid(v);
		//skip the lines where the format of graph has some problem
		if(v<0||v>=nodenum){
			Skipline();
			if(++curpos >= endpos){
				if(feof(inFile))
					break;
				FillInputBuffer(inFile);
			}
			continue;
		}//end of skip "bad" lines

		//skip the line where vid is smaller than vmin, i.e., the nodes that 
		//have been processed
		//skip the nodes that are not S and S+ nodes
		if(isseednode[v]!='t'){
			Skipline();
			if (++curpos >= endpos){
				if (feof(inFile))
					break;
				FillInputBuffer(inFile);
			}
			continue;
		}
		dv=0;
		Parsedeg(dv,nodenum);
		if(dv>0){
			Z.Initrowmemory(pos,dv);
			/////////////////////////////////////////////////////////////////////////
			//read and initialize the value of neighbore index and weight (START)
			//////////////////////////////////////////////////////////////////////////
			j=0;
			while(j<dv){
				Parseneighbor(neighbore,weight,nodenum,v);
				Z.addelement(pos,neighbore,weight);
				if (neighbore > maxidx)
					maxidx = neighbore;
				j++;
			}
			/////////////////////////////////////////////////////////////////////////
			//read and initialize the value of neighbore index and weight (END)
			//////////////////////////////////////////////////////////////////////////
			while(*curpos!='\n')
				curpos++;
		}//end: if(deg >0)
		else{
			Z.Initrowmemory(pos,dv);
			while(*curpos!='\n')
				curpos++;
		}
		pos++;
		if(++curpos >= endpos){
			if(feof(inFile))
				break;
			FillInputBuffer(inFile);

		}
	}//end: while(i)
	///////////////////////////////////////////////////////////////////////////////////////////////
	//////////scanning the graph again to initializing the S and Splus nodes' adjacent lists(end)
	//////////////////////////////////////////////////////////////////////////////////////////////
	rewind(inFile);
	int rv=fscanf(inFile,"%d\n",&nodenum);
	IORC.stop();
	iotime+=IORC.GetRuntime();
	maxidx++;
	Z.setcolumnnum(maxidx);
}
inline void Readcommunity(SparseMatrix &Z, int nodenum, FILE *inFile){
	Runtimecounter IORC;
	IORC.start();
	int i = 0;
	int v = 0;
	int dv = 0;
	double weight;
	int neighbore = 0;
	int maxidx = 0;
	int j = 0;
	int pos = 0;
	Z.Initmemory(nodenum);
	curpos = endpos0 = inputbuffer;	//initialize the data position pointers to the start of the buffer
	FillInputBuffer(inFile);
	//starting scanning
	while (1){
		Parseid(v);
		//skip the lines where the format of graph has some problem
		if (v < 0 || v >= nodenum){
			Skipline();
			if (++curpos >= endpos){
				if (feof(inFile))
					break;
				FillInputBuffer(inFile);
			}
			continue;
		}//end of skip "bad" lines
		dv = 0;
		Parsedeg(dv, nodenum);
		if (dv > 0){
			Z.Initrowmemory(pos, dv);
			/////////////////////////////////////////////////////////////////////////
			//read and initialize the value of neighbore index and weight (START)
			//////////////////////////////////////////////////////////////////////////
			j = 0;
			while (j < dv){
				Parseneighbor(neighbore, weight, nodenum,v);
				Z.addelement(pos, neighbore, weight);
				if (neighbore > maxidx)
					maxidx = neighbore;
				j++;
			}
			/////////////////////////////////////////////////////////////////////////
			//read and initialize the value of neighbore index and weight (END)
			//////////////////////////////////////////////////////////////////////////
			while (*curpos != '\n')
				curpos++;
		}//end: if(deg >0)
		else{
			Z.Initrowmemory(pos, dv);
			while (*curpos != '\n')
				curpos++;
		}
		pos++;
		if (++curpos >= endpos){
			if (feof(inFile))
				break;
			FillInputBuffer(inFile);

		}
	}//end: while(i)
	///////////////////////////////////////////////////////////////////////////////////////////////
	//////////scanning the graph again to initializing the S and Splus nodes' adjacent lists(end)
	//////////////////////////////////////////////////////////////////////////////////////////////
	rewind(inFile);
	int rv=fscanf(inFile, "%d\n", &nodenum);
	IORC.stop();
	iotime += IORC.GetRuntime();
	maxidx++;
	Z.setcolumnnum(maxidx);
}
inline void Replacefile(char *destfilename, char *srcfilename){
	FILE* source = fopen(srcfilename, "rb");
	FILE* dest = fopen(destfilename, "wb");
	if(source!=NULL&&dest!=NULL){
		size_t size;
		// clean and more secure
		// feof(FILE* stream) returns non-zero if the end of file indicator for stream is set

		while (size = fread(filebuffer, 1, BYTE_TO_READ, source)) {
			fwrite(filebuffer, 1, size, dest);
		}

		fclose(source);
		fclose(dest);
	}
}
inline void ReadGraph(Node *&G, int nodenum,FILE *inFile){
	Runtimecounter IORC;
	IORC.start();
	int i=0;
	int v=0;
	int dv=0;
	double weight;
	int neighbore=0;
	int j=0;
	///////////////////////////////////////////////////////////////////////////////////////////////
	//////////scanning the graph again to initializing the S and Splus nodes' adjacent lists(start)
	//////////////////////////////////////////////////////////////////////////////////////////////
	G=(Node*)malloc(sizeof(Node)*nodenum);
	if(G==NULL){
		printf("system could not allocate more memory\n");
		exit(2);
	}
	for(i=0;i<nodenum;i++){
		G[i].vid=-1;
		G[i].deg=0;
		G[i].nbv=NULL;
		G[i].weight=NULL;
	}
	curpos=endpos0=inputbuffer;	//initialize the data position pointers to the start of the buffer
	FillInputBuffer(inFile);
	//starting scanning
	int pos=0;
	while(1){
		Parseid(v);
		//skip the lines where the format of graph has some problem
		if(v<0||v>=nodenum){
			Skipline();
			if(++curpos >= endpos){
				if(feof(inFile))
					break;
				FillInputBuffer(inFile);
			}
			continue;
		}//end of skip "bad" lines
		if (pos >= nodenum)
			break;
		G[pos].vid=v;
		dv=0;
		Parsedeg(dv,nodenum);
		if(dv>0){
			/////////////////////////////////////////////////////////////
			//for each node v,
			//allocate memory space to store a heap entry (dv,v,nb(v)) (START)
			///////////////////////////////////////////////////////////
			if(sizeof(double)*dv>=BLK_SZ2){
				G[pos].nbv=(int*)malloc(sizeof(int)*dv);
				G[pos].weight=(double*)malloc(sizeof(double)*dv);
				if (G[pos].nbv==NULL||G[pos].weight==NULL){
					printf("System could not allocate more memory\n");
					exit(2);
				}
			}else{
				allocatetmpmemory(sizeof(int)*dv);
				G[pos].nbv=(int *)curMemPos;
				curMemPos+=(sizeof(int)*dv);
				allocatetmpmemory(sizeof(double)*dv);
				G[pos].weight=(double*)curMemPos;
				curMemPos+=(sizeof(double)*dv);
			}

			/////////////////////////////////////////////////////////////
			//for each node v,
			//allocate memory space to store a heap entry (dv,v,nb(v)) (END)
			///////////////////////////////////////////////////////////

			/////////////////////////////////////////////////////////////////////////
			//read and initialize the value of neighbore index and weight (START)
			//////////////////////////////////////////////////////////////////////////
			j=0;
			G[pos].deg=0;
			while(j<dv){
				Parseneighbor(neighbore,weight,nodenum,v);
				G[pos].nbv[G[pos].deg]=neighbore;
				G[pos].weight[G[pos].deg]=weight;
				G[pos].deg++;
				j++;
			}
			/////////////////////////////////////////////////////////////////////////
			//read and initialize the value of neighbore index and weight (END)
			//////////////////////////////////////////////////////////////////////////
			while(*curpos!='\n')
				curpos++;
		}//end: if(deg >0)
		else{
			G[pos].deg=0;
			G[pos].nbv=NULL;
			G[pos].weight=NULL;
			while(*curpos!='\n')
				curpos++;
		}
		pos++;
		if(++curpos >= endpos){
			if(feof(inFile))
				break;
			FillInputBuffer(inFile);

		}
	}//end: while(i)
	///////////////////////////////////////////////////////////////////////////////////////////////
	//////////scanning the graph again to initializing the S and Splus nodes' adjacent lists(end)
	//////////////////////////////////////////////////////////////////////////////////////////////
	rewind(inFile);
	int rv=fscanf(inFile,"%d\n",&nodenum);
	if (rv != 1){
		printf("the format of file is not correct");
		exit(2);

	}
	IORC.stop();
	iotime+=IORC.GetRuntime();
}
#endif