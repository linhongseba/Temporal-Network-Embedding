#ifndef Runtimecounter_h_
#define Runtimecounter_h_
#include"time.h"
#include"windows.h"
class Runtimecounter{
public:
	LARGE_INTEGER t1, t2, tc;
	Runtimecounter();
	~Runtimecounter();
	void start();
	void stop();
	double GetRuntime();

};

Runtimecounter::Runtimecounter(){
	QueryPerformanceFrequency(&tc); 
}
Runtimecounter::~Runtimecounter(){
}
void Runtimecounter::start(){
	QueryPerformanceCounter(&t1);
}
void Runtimecounter::stop(){
	QueryPerformanceCounter(&t2);
}
double Runtimecounter::GetRuntime(){
	double t=(double)(t2.QuadPart-t1.QuadPart)*1000/(double)tc.QuadPart;
	return t;
}
#endif