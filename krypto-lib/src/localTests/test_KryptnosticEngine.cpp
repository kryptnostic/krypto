#include "../main/cpp/KryptnosticEngine.h"
#include <iostream>
#include <time.h>

using namespace std;

#define N 1
#define L 2
#define DEBUG false
#define OPRUNS 1
#define TESTRUNS 1

void testInstantiateKE() {
	clock_t diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		clock_t begin = clock();

		string serverGlobal = "yo";
		KryptnosticEngine<N, L> engine(serverGlobal);

		UUID objectId1;
		objectId1.randomize();

		cout << "Current docKey = ";
		engine.getDocKey(objectId1).print();

		cout << "Randomly generated docKey2 = ";
		UUID docKey2;
		docKey2.randomize();
		docKey2.print();

		cout << "Current docKey = ";
		engine.setDocKey(objectId1, docKey2);
		engine.getDocKey(objectId1).print();

		clock_t end = clock();
 		diff += (end - begin);
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " instantiations of KryptnosticEngine: " << double(diff) / (CLOCKS_PER_SEC * OPRUNS * TESTRUNS) << " sec" << endl;
}


int main(int argc, char **argv) {
	cout << "KryptnosticEngine tests" << endl;
	testInstantiateKE();
	return 0;
}