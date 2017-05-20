#include"nodeMapper.h"
void exit_with_help(char *pname){
	printf("Usage: %s [graphdir] [outputgraph] [outputnodeName] \n",pname);
	exit(1);
}

int main(int argc, char *argv[]) {
	if (argc < 4){
		exit_with_help(argv[0]);
	}
	nodeMapper mapper(argv[1]);
	mapper.writeGraph(argv[1], argv[2]);
	mapper.writeName(argv[3]);
}