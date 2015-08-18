#include "../main/cpp/KryptnosticEngine.h"
#include <iostream>
#include <time.h>

using namespace std;

#define N 1
#define L 2
#define DEBUG false
#define OPRUNS 1
#define TESTRUNS 1

void testInstantiateUUID() {
	clock_t diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		clock_t begin = clock();

		KryptnosticEngine<N, L> engine1("yo");

		clock_t end = clock();
 		diff += (end - begin);
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " instantiations of KryptnosticEngine: " << double(diff) / (CLOCKS_PER_SEC * OPRUNS * TESTRUNS) << " sec" << endl;
}


int main(int argc, char **argv) {
	cout << "UUID tests" << endl;
	testInstantiateUUID();
	return 0;
}