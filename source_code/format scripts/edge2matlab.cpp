#include<stdio.h>
#include<stdlib.h>
#include<iostream>
using namespace std;
int main(int argc, char *argv[]){
	FILE *rfile=fopen(argv[1],"r");
	if(rfile==NULL){
		cout<<"could not open file to read"<<endl;
		exit(0);
	}
	int u;
	int v;
	int w;
	//fscanf(rfile,"%d\t%d\n",&u,&v);
	FILE *wfile=fopen(argv[2],"w");
	if(wfile==NULL){
		cout<<"could not open file to write"<<endl;
		exit(0);
	}
	while(feof(rfile)==false){
		fscanf(rfile,"%d\t%d\t%d\n",&u,&v,&w);
		fprintf(wfile,"%d\t%d\t%d\n",u+1,v+1,w);
	}
	fclose(rfile);
	fclose(wfile);
	return 0;
}