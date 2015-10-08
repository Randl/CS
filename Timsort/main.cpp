#include "timsort.h"
#include <windows.h>
#include <iostream>


class Timer  {
public:
	Timer() { m_dwTime = GetTickCount(); }
	DWORD GetTime() { return GetTickCount() - m_dwTime; }
private:
	DWORD m_dwTime;
};

bool checkSort (double *x, long length)  {
	double *x1=x, *x2=x+1;
	while (x2<x+length && x1<=x2)  {
		++x1;
		++x2;
	}
	return x2==(x+length);
}

const long size=100000000;
double a1[size];

int main(){
	
	
	for (int i=0; i<size; ++i)
		a1[i]=rand()%size;
	Timer time;
	timsort(&a1[0], size);
	std::cout << time.GetTime() << " ms" << "\n";
	std::cout<<checkSort(a1, size);
	system("pause");
}