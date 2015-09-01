#include "../main/cpp/PrivateKey.h"
#include "../main/cpp/SearchPrivateKey.h"
#include <chrono>

#define N 64

void generateSmallPrivateKey(){
	std::cout<<"Generating Private Key of size " << N << "..." <<endl;
    auto start = std::chrono::high_resolution_clock::now();

    PrivateKey<N> pk;

    auto stop = std::chrono::high_resolution_clock::now();
    auto total =std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();
    std::cout<<"Total time: "<< total <<" nanos"<<std::endl;
    std::cout<<"Mean time: "<< total/double(N) <<" nanos"<<std::endl;
}

void generateRegularPrivateKey(){
	std::cout<<"Generating Private Key of size " << 2*N << "..." <<endl;
    auto start = std::chrono::high_resolution_clock::now();

    PrivateKey<2*N> pk;

    auto stop = std::chrono::high_resolution_clock::now();
    auto total =std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();
    std::cout<<"Total time: "<< total <<" nanos"<<std::endl;
    std::cout<<"Mean time: "<< total/double(2*N) <<" nanos"<<std::endl;
}

void generateSmallSearchPrivateKey(){
	std::cout<<"Generating Search Private Key of size " << N << "..." <<endl;
    auto start = std::chrono::high_resolution_clock::now();

    SearchPrivateKey<N> sk;

    auto stop = std::chrono::high_resolution_clock::now();
    auto total =std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();
    std::cout<<"Total time: "<< total <<" nanos"<<std::endl;
    std::cout<<"Mean time: "<< total/double(N) <<" nanos"<<std::endl;
}

void generateRegularSearchPrivateKey(){
	std::cout<<"Generating Private Key of size " << 2*N << "..." <<endl;
    auto start = std::chrono::high_resolution_clock::now();

    SearchPrivateKey<2*N> sk;

    auto stop = std::chrono::high_resolution_clock::now();
    auto total =std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();
    std::cout<<"Total time: "<< total <<" nanos"<<std::endl;
    std::cout<<"Mean time: "<< total/double(2*N) <<" nanos"<<std::endl;
}

int main(){
	generateSmallPrivateKey();
	generateRegularPrivateKey();
	generateSmallSearchPrivateKey();
	generateRegularSearchPrivateKey();
	return 0;
}