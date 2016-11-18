//#include <crtdbg.h>
#define _CRT_SECURE_NO_WARNINGS
//#define _CRTDBG_MAP_ALLOC
#include"gradiantmethod.h"
#include"dirent.h"
#include"IncrementalBCGD.h"
#include <algorithm>


void exit_with_help(char *pname){
	printf("Usage: %s graphdir [option]\n",pname);
	printf("-t: type of algorithms default t=4\n");
	printf("-t:=1 global,=2 global auto, =3 local, =4 local auto, =5 incremental, =6 incremental auto\n");
	printf("-c: number_of_dimensions [int default 20]\n");
	printf("-m: memorybound (MB) [int default 1024]\n");
	printf("-a : alpha>0 [double default 0.00001]\n");
	printf("-l : lambda [double default 0.01]\n");
	printf("-p : prefix [string]\n");
	printf("-i : iteration number [int default 300]\n");
	printf("-e : print out step size defalt=100, print out every 100 iters\n");
	printf("-b : size of output buffer (>4MB), default 60MB\n");
	printf("-z : zeta value (default sqrt(1/nodenum)\n");
	printf("-d : delta value (default zeta*2/k\n");
	exit(1);
}

bool custom_lexicographical_compare(const char *c1, const char *c2) { return strcmp(c1, c2) < 0; }

int main(int argc, char *argv[]){
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	srand((unsigned)time(NULL));
	if (argc < 2){
		exit_with_help(argv[0]);
	}
	int i = 0;
	double memorybound = 1024;
	int m = 20;
	alpha = 0.00001;
	double lambda = 0.01;
	maxiter = 300;
	string prefix("");
	int printstep=100;
	int type = 4;
	BYTE_TO_READ = 60000000;
	bool zetainput = false;
	bool deltainput = false;
	for (i = 2; i < argc; i++){
		if (argv[i][0] != '-' || strlen(argv[i]) < 2) {
			exit_with_help(argv[0]);
		}
		if (++i >= argc)
			exit_with_help(argv[0]);
		switch (argv[i - 1][1]){
		case 'c':
			m = atoi(argv[i]);
			break;
		case 'm':
			memorybound = atoi(argv[i]);
			break;
		case 'a':
			alpha = atof(argv[i]);
			break;
		case 'l':
			lambda = atof(argv[i]);
			break;
		case 'p':
			prefix = argv[i];
			break;
		case 'i':
			maxiter = atoi(argv[i]);
			break;
		case 'e':
			printstep = atoi(argv[i]);
			break;
		case 't':
			type = atoi(argv[i]);
			break;
		case 'b':
			BYTE_TO_READ = atoi(argv[i]);
			break;
		case 'z':
			zeta = atof(argv[i]);
			zetainput = true;
			break;
		case 'd':
			delta = atof(argv[i]);
			deltainput = true;
			break;
		default:
			exit_with_help(argv[0]);
		}
	}
	memorybound*=1024;
	
	Runtimecounter RC;
	RC.start();
	Initmemory();

	////////////////////////////////////////////////////////////
	///////////initializing IO memory block (START)/////////////
	///////////////////////////////////////////////////////////
	InitIOmemory();
	filebuffer = new char[BYTE_TO_READ];
	double mm = (double)BYTE_TO_READ / 1024 * 3;
	memorybound -= mm;
	if (memorybound < 0){
		cout << "minimum memory required (MB)" << mm/1024 << endl;
		exit(2);
	}
	//memorybound = 300; //for debugging small graph only
	////////////////////////////////////////////////////////////
	///////////initializing IO memory block (START)/////////////
	///////////////////////////////////////////////////////////

	DIR *dir=NULL;
	struct dirent *ent;
	vector<char *> filenames;
	filenames.reserve(20);
	if ((dir = opendir (argv[1])) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			if (ent->d_name[0] != '.'){
				//printf("%s\n", ent->d_name);
				allocatetmpmemory(sizeof(char) * 2048);
				char *name =(char*)curMemPos;
				curMemPos +=( sizeof(char) * 2048);
				strcpy(name, argv[1]);
				strcat(name, "/");
				strcat(name, ent->d_name);
				//printf("%s\n", name);
				filenames.push_back(name);
			}
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("could not open directory");
		return EXIT_FAILURE;
	}

	// Need to sort filenames array
	std::sort(filenames.begin(), filenames.end(), custom_lexicographical_compare);

	int maxnodenum = Getmaxnodenumber(filenames);
	double lowmemory = sizeof(Node)*maxnodenum;
	lowmemory += sizeof(Row)*maxnodenum;
	lowmemory += (sizeof(double) + sizeof(int))*maxnodenum*30;
	lowmemory += (sizeof(double) + sizeof(int))*maxnodenum *m*2;
	lowmemory /= 1024;
	bool inmemory = false;
	if (lowmemory < 0.5*memorybound){
		inmemory = true;
	}
	else
		inmemory = false;
	cout << "In memory " << inmemory << endl;
	switch (type){
	case 1:
		if (inmemory == false)
			BCGDGlobalSparse(m, filenames, lambda, memorybound, prefix, printstep, false, maxnodenum);
		else
			BCGDGlobalFull(m, filenames, lambda, prefix, printstep, false, maxnodenum);
		break;
	case 2:
		if (inmemory==false)
			BCGDGlobalSparse(m, filenames, lambda, memorybound, prefix, printstep, true,maxnodenum);
		else
			BCGDGlobalFull(m, filenames, lambda, prefix, printstep, true, maxnodenum);
		break;
	case 3:
		if (inmemory==false)
			BCGDLocalSparse(m, filenames, lambda, memorybound, prefix, printstep, false,maxnodenum);
		else
			BCGDLocalFull(m, filenames, lambda, prefix, printstep, false, maxnodenum);
		break;
	case 4:
		if (inmemory==false)
			BCGDLocalSparse(m, filenames, lambda, memorybound, prefix, printstep, true,maxnodenum);
		else
			BCGDLocalFull(m, filenames, lambda, prefix, printstep, true, maxnodenum);
		break;
	case 5:
		if (inmemory == false)
			IncrementalBCGDsparse(m, filenames, lambda, memorybound, prefix, printstep, false, maxnodenum,zetainput,deltainput);
		else
			IncrementalBCGDfull(m, filenames, lambda, prefix, printstep, false, maxnodenum,zetainput,deltainput);
		break;
	case 6:
		if (inmemory == false)
			IncrementalBCGDsparse(m, filenames, lambda, memorybound, prefix, printstep,true, maxnodenum,zetainput,deltainput);
		else
			IncrementalBCGDfull(m, filenames, lambda, prefix, printstep, true, maxnodenum,zetainput,deltainput);
		break;
	case 7:
		BCGDLocalSparsefaster(m, filenames, lambda, memorybound, prefix, printstep, false, maxnodenum);
		break;
	case 8:
		BCGDLocalSparsefaster(m, filenames, lambda, memorybound, prefix, printstep, true, maxnodenum);
		break;
	default:
		exit_with_help(argv[0]);
	}
	RC.stop();
	cout << "Total running time " << RC.GetRuntime() << endl;
	cout << "Total IO time " << iotime << endl;
	releaseIOmemory();
	releaseblockmemory();
	filenames.clear();
	//filenames.shrink_to_fit();
	prefix.clear();
	//prefix.shrink_to_fit();
	//_CrtDumpMemoryLeaks();
	return 0;
}
