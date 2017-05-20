#ifndef nodeMapper_h_
#define nodeMapper_h_
#include "../main/function.h"
#include <map>
#include <unordered_map>
#include "nodeMapperUtil.h"

class nodeMapper{
private:
	unordered_map<string, long> termmap;
	unordered_map<string, long> pinmap;
	unordered_map<string, long> querymap;
	long termNum;
	long pinNum;
	long queryNum;
	void constructMap(string filename);
	void addToMap(string node, string type);
public:
	nodeMapper();
	~nodeMapper();
	nodeMapper(char *graphfile);
	long getTermNum();
	long getPinNum();
	long getQueryNum();
	long getNodeID(string node, string type);
	long getTermID(string term);
	long getPinID(string pin);
	long getQueryID(string query);
	void constructEdge(string filename, ofstream &fout);
	void writeGraph(char *graphfile, const char *outputfile);
	void writeName(const char *mapfilename);
};


nodeMapper::nodeMapper(){

}

nodeMapper::~nodeMapper(){
	termmap.clear();
	pinmap.clear();
	querymap.clear();
}
void nodeMapper::addToMap(string node, string type) {
	switch (hashit(type)) {
		case pin:
			if (pinmap.find(node) == pinmap.end()) {
				pinmap.insert(make_pair(node, pinNum));
				pinNum ++;
			}
			break;
		case query:
			if (querymap.find(node) == querymap.end()) {
				querymap.insert(make_pair(node, queryNum));
				queryNum ++;
			}
			break;
		case term:
			if (termmap.find(node) == termmap.end()) {
				termmap.insert(make_pair(node, termNum));
				termNum ++;
			}
			break;
		default:
			break;
	}
}


void nodeMapper::constructMap(string filename) {
	ifstream fin(filename);
	if (!fin.good()) {
		cout << "debug " << endl;
		return;
	} else {
		string source;
		string target;
		string source_type;
		string target_type;
		double weight;
		string strLine;
		cout << "debug " << endl;
		int linenum = 0;
		while (!fin.eof()) {
			getline(fin, strLine);
			if (linenum %10000 == 0) {
				cout << linenum << "\t" << strLine <<endl;
			}
			vector<string> tokens = getTokens(strLine, "\t");
			if (tokens.size() < 4) {
				continue;
			}
			source = tokens[0];
			source_type = tokens[1];
			target = tokens[2];
			target_type = tokens[3];
			addToMap(source, source_type);
			addToMap(target, target_type);
			linenum ++;
		}
		cout << "finish reading " <<endl;
		fin.close();
	}
}

nodeMapper::nodeMapper(char *graphfile) {
	vector<string> filenames = getfilenames(graphfile);
	termNum = 0;
	queryNum = 0;
	pinNum = 0;
	for (int i = 0; i < filenames.size(); i++) {
		cout << filenames.at(i) << endl;
		constructMap(filenames.at(i));
	}
	cout << "finish map construction" << endl;
}

void nodeMapper::writeGraph(char *graphfile, const char *outputfile) {
	vector<string> filenames = getfilenames(graphfile);
	ofstream fout(outputfile);
	for (int i = 0; i < filenames.size(); i++) {
		constructEdge(filenames.at(i), fout);
	}
	fout.close();
}

void nodeMapper::writeName(const char*mapfilename) {
	ofstream fout(mapfilename);
	for (auto& entry: pinmap) {
		fout << entry.first << "\t" << entry.second << endl;
	}
	for (auto& entry: querymap) {
		fout << entry.first << "\t" << entry.second + pinNum << endl;
	}
	for (auto& entry: termmap) {
		fout << entry.first << "\t" << entry.second + pinNum + queryNum <<endl;
	}
	fout.close();
}

long nodeMapper::getTermID(string term) {
	return termmap.find(term) != termmap.end() ? termmap[term] : -1;
}

long nodeMapper::getQueryID(string query) {
	return querymap.find(query) != querymap.end() ? querymap[query] : -1;
}

long nodeMapper::getPinID(string pin) {
	return pinmap.find(pin) != pinmap.end() ? pinmap[pin] : -1;
}

long nodeMapper::getTermNum() {
	return termNum;
}

long nodeMapper::getQueryNum() {
	return queryNum;
}

long nodeMapper::getPinNum() {
	return pinNum;
}

long nodeMapper::getNodeID(string node, string type) {
	switch (hashit(type)) {
		case pin:
			return getPinID(node);
			break;
		case query:
			return getQueryID(node) + pinNum;
			break;
		case term:
			return getTermID(node) + pinNum + queryNum;
			break;
		default:
			return -1;
			break;
	}
}
void nodeMapper::constructEdge(string filename, ofstream &fout) {
	ifstream fin(filename);
	if (!fin.good()) {
		return;
	} else {
		string source;
		string target;
		string source_type;
		string target_type;
		string weight;
		int linenum = 0;
		string strLine;
		while (!fin.eof()) {
			getline(fin, strLine);
			vector<string> tokens = getTokens(strLine, "\t");
			if (linenum %10000 == 0) {
				cout << linenum << "\t" << tokens.size() << endl;
				cout << tokens[3] << endl;
				cout << strLine <<endl;
			}
			if (tokens.size() < 5) {
				linenum ++;
				continue;
			}
			source = tokens[0];
			source_type = tokens[1];
			target = tokens[2];
			target_type = tokens[3];
			weight = tokens[4];
			long sid = getNodeID(source, source_type);
			long tid = getNodeID(target, target_type);
			fout << sid << "\t" << tid << "\t" << weight << endl;
			linenum ++;
		}
		fin.close();
	}
}
#endif