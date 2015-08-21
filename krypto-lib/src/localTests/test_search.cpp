#include "../main/cpp/SearchPrivateKey.h"
#include <iostream>
#include <time.h>

using namespace std;

#define N 1
#define L 2
#define OPRUNS 1
#define TESTRUNS 1

void testSearch()) {
	clock_t diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		PrivateKey<N, L> pk;
		BridgeKey<N, L> bk(pk);
		PublicKey<N, L> pub(bk);

		clock_t begin = clock();

		SearchPrivateKey spk;
		cout << "size of spk is " << sizeof(spk) << endl;

		clock_t end = clock();
 		diff += (end - begin);
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " instantiations of UUID: " << double(diff) / (CLOCKS_PER_SEC * OPRUNS * TESTRUNS) << " sec" << endl;
}


int main(int argc, char **argv) {
	cout << "Search tests" << endl;
	testSearch();
	return 0;
}