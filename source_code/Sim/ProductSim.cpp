#define _CRT_SECURE_NO_WARNINGS
#include"../main/IOfunction.h"
#include"../main/memoryfunction.h"
#include<unordered_map>
#include<queue>
#include<string>
#include<iostream>
using namespace std;

struct product{
	int pid;
	double sim;
	product(int _id, double _sim) {
		pid = _id;
		sim = _sim;
	}
};
struct compare{
	bool operator()(product &a, product &b) {
		return a.sim > b.sim;
	}
};
class ProductSim{
	unordered_map<string, int> labels;
	priority_queue<product, vector<product>, compare> Q;
	int K = 20;
	vector<string> nodelabels;
	DIR *dir = NULL;
	struct dirent *ent;
	vector<string> filenames;
	int productnum;
public:
	ProductSim(int num){
		productnum = num;
		nodelabels.resize(num);
	}
	~ProductSim() {
		filenames.clear();
		if (ent != NULL) {
			free(ent);
			ent = NULL;
		}
	}
	void readNodeLabel(string filename) {
		ifstream infile(filename);
		if (infile.is_open() == false) {
			cout << "could not open the filename" << endl;
			exit(4);
		}
		int idx;
		string s;
		while (infile.good()) {
			infile >> idx;
			infile >> s;
			labels[s] = idx;
			nodelabels[idx] = s;
		}
		infile.close();
	}

	void listfileinDir(string dirname) {
		if ((dir = opendir(dirname.c_str())) != NULL) {
			/* print all the files and directories within directory */
			while ((ent = readdir(dir)) != NULL) {
				if (ent->d_name[0] != '.'){
					filenames.push_back(dirname+"/"+ent->d_name);
				}
			}
			closedir(dir);
		}
		else {
			/* could not open directory */
			perror("could not open directory");
			exit(EXIT_FAILURE);
		}
	}

	void computesim(string productname) {
		if (this->labels.find(productname) == labels.end()) {
			cout << "the product does not exit in our data" << endl;
			cout << " please check your input (format google:chrome)" << endl;
			exit(4);
		}
		else {
			int idx = labels[productname];
			SparseMatrix Z;
			Z.Initmemory(productnum);
			Z.setcolumnnum(20);
			FILE *rfile = NULL;
			for (int t = 0; t < filenames.size(); t++) {
				rfile = fopen(filenames[t].c_str(), "r");
				if (rfile == NULL) {
					cout << "could not open file to read" << endl;
					exit(4);
				}
				Readcommunity(Z, productnum, rfile);
				for (int j = 0; j < productnum; j++) {
					if (nodelabels[j].find_first_of("CVE") >= 0 && nodelabels[j].compare("CVE-2015") < 0) {
						continue;
					}
					if (j != idx) {
						double sim = Z.Rowdotproduct2(idx, j);
						product a(j, sim);
						if (Q.size() < K) {
							Q.push(a);
						}
						else if (sim >= Q.top().sim) {
							Q.pop();
							Q.push(a);
						}
					}
				}
				//output the similar products;
				while (!Q.empty()) {
					product o = Q.top();
					cout << t << "\t" << nodelabels[o.pid] << "\t" << o.sim << endl;
					Q.pop();
				}

				//release the memory of Z;
				Z.clear();
			}
			Z.deletemem();
		}
	}
};

int main (int argc, char *argv[]) {
	if (argc < 1) {
		cout << "Usage " << argv[0];
		cout << "[#product] [embedding-file-name] [node-label-file-name] [product-name]" << endl;
		exit(4);
	}
	int v = atoi(argv[1]);
	ProductSim a(v);
	string dirname(argv[2]);
	string labelname(argv[3]);
	string productname(argv[4]);
	Initmemory();
	InitIOmemory();
	filebuffer = new char[BYTE_TO_READ];
	a.listfileinDir(dirname);
	a.readNodeLabel(labelname);
	a.computesim(productname);
	releaseIOmemory();
	releaseblockmemory();
	return 0;
}
