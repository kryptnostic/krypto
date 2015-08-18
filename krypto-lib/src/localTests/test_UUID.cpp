#include "../main/cpp/UUID.h"
#include <iostream>
#include <time.h>

using namespace std;

#define N 1
#define DEBUG false
#define OPRUNS 1
#define TESTRUNS 1

void testInstantiateUUID() {
	clock_t diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		clock_t begin = clock();

		UUID id1;
		UUID id2;

		if (equal_to<UUID>()(id1, id2)) cout << "yes" << endl;
		else cout << "no" << endl;

		id1.print();
		id2.print();

		id1.randomize();
		id2.randomize();

		id1.print();
		id2.print();

		clock_t end = clock();
 		diff += (end - begin);
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " instantiations of UUID: " << double(diff) / (CLOCKS_PER_SEC * OPRUNS * TESTRUNS) << " sec" << endl;
}


int main(int argc, char **argv) {
	cout << "UUID tests" << endl;
	testInstantiateUUID();
	return 0;
}