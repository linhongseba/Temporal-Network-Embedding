#include<stdlib.h>
#include<stdio.h>
#include<string.h>
inline int binarysearch(int *&source1, int length1, int v){
	int low=0;
	int high=length1;
	while(low<high){
		int mid=low+((high-low)/2);
		if(source1[mid]<v)
			low=mid+1;
		else
			high=mid;
	}
	if((low<length1)&&(source1[low]==v))
		return low;
	else
		return -(low+1);
}

inline bool insert(int *&Gu, int v){
	int maxlength=Gu[0];
	int deg=Gu[1];
	if(deg==0){
		Gu[deg+2]=v;
		Gu[1]++;
		return true;
	}else{
		if(deg>=maxlength-2){
		Gu=(int *)realloc(Gu,sizeof(int)*(maxlength+100));
		Gu[0]=maxlength+100;
		}
		int *temp=Gu+2;
		int cur=binarysearch(temp,deg,v);
	//if v is not in G[u], then insert v into G[u] 
	//and reorder G[u]
		if(cur<0){
			for(int i=deg+2;i>-cur+1;i--){
				Gu[i]=Gu[i-1];
			}
			Gu[-cur+1]=v;
			Gu[1]++;
			return true;
		}else
			return false;
	}	
}
int main(int argc, char *argv[]){
	char *filename=(char*)malloc(sizeof(char)*100);
	strcpy(filename,argv[1]);
	FILE *rfile=fopen(filename,"r");
	if(rfile==NULL){
		printf("could not find the graph file\n");
		exit(0);
	}
	int nodenum;
	int startnum=atoi(argv[2]);
	char *str1=(char *)malloc(sizeof(char)*500);
	char *str2=(char *)malloc(sizeof(char)*100);
	//fscanf(rfile,"%[^\n]\n",str1);
	//fscanf(rfile,"%[^\n]\n",str1);
	//fscanf(rfile,"#%s%d%s%d\n",str1,&nodenum,str2,&edgenum);
	//fscanf(rfile,"#%s%s\n",str1,str2);
	fscanf(rfile,"%d",&nodenum);
	int u,v;
	int **G=(int **)malloc(sizeof(int *)*nodenum);
	for(int i=0;i<nodenum;i++)
		G[i]=(int*)malloc(sizeof(int)*100);
	int arcnum=0;
	for(int i=0;i<nodenum;i++){
		G[i][0]=99;
		G[i][1]=0;
	}
	int i=0;
	int nnum=0;
	while(!feof(rfile)){
		fscanf(rfile,"%d%d",&u,&v);
		if(nnum<u)
			nnum=u;
		if(nnum<v)
			nnum=v;
		u=u%nodenum;
		v=v%nodenum;
		if(u==v)
			continue;
		bool flag1=insert(G[u],v);
		bool flag2=insert(G[v],u);
		if(flag1==true&&flag2==true)
			arcnum++;
		//bool flag1=insert(G[u],v);
		//if(flag1==true)
			//arcnum++;
		if(i%1000==0){
			printf("now it is in edge %d\n",i);
		}
		i++;
	}
	fclose(rfile);
	strcat(filename,"_new.txt");
	FILE *wfile=fopen(filename,"w");
	if(wfile==NULL){
		printf("could not write to files\n");
		exit(0);
	}
	int nnumreal=nodenum<nnum?nodenum:nnum;
	//fprintf(wfile,"%d %d\n",nnumreal,nnumreal);
	for(int i=0;i<nnumreal;i++){
		fprintf(wfile,"%d,%d",i+startnum,G[i][1]);
		for(int j=0;j<G[i][1];j++){
			fprintf(wfile,":%d,%d",G[i][j+2]+startnum,1);
		}
		fprintf(wfile,"\n");
	}
	printf("number of edges: %d\n",arcnum);
}