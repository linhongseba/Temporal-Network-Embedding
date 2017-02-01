/*
 * Runtimecounter.h
 *
 *      Author: linhong zhu (linhong.seba.zhu@gmail.com)
 */

#ifndef Runtimecounter_h_
#define Runtimecounter_h_
#include <sys/time.h>
#include <stdlib.h>

/*struct timevalue{
long    tv_sec;         // seconds
long    tv_usec;        // microseconds
};

struct timezone{
int     tz_minuteswest; // minutes W of Greenwich
int     tz_dsttime;     // type of dst correction
};*/
class Runtimecounter{
public: //timezone tz;
        timeval t1;
        timeval t2;
public:
	Runtimecounter();
	void start();
	void stop();
	float GetRuntime();
    float GetRuntimeusr();

};

Runtimecounter::Runtimecounter(){
}

void Runtimecounter::start(){
    gettimeofday(&this->t1, NULL);
}
void Runtimecounter::stop(){
    gettimeofday(&this->t2, NULL);
}
float Runtimecounter::GetRuntime(){
	float t=(float)(t2.tv_sec-t1.tv_sec)*1.0+(float)(t2.tv_usec-t1.tv_usec)/1000000.0;
	return t;
}

#endif
