#include "../main/cpp/KryptnosticClient.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

using namespace std;

#define DEBUG false
#define OPRUNS 1
#define TESTRUNS 1
#define N 128

void testInstantiateKE() {
	clock_t diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		clock_t begin = clock();

		//PrivateKey<128> pk;
		//SearchPrivateKey<128> spk(pk);
		KryptnosticClient<N> engine;
		ClientHashFunction<N> chf;

		//cout << "size of spk is " << sizeof(spk) << endl;

		clock_t end = clock();
 		diff += (end - begin);
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " instantiations of KryptnosticClient: " << double(diff) / (CLOCKS_PER_SEC * OPRUNS * TESTRUNS) << " sec" << endl;
}


int main(int argc, char **argv) {
	cout << "KryptnosticClient tests" << endl;
	testInstantiateKE();
	return 0;
}