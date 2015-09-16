#include "../main/cpp/SearchPrivateKey.h"
#include <iostream>
#include <time.h>

using namespace std;

#define N 128
#define L 2
#define OPRUNS 1
#define TESTRUNS 1

void testAddress() {
	SearchPrivateKey<N> sk;
	PrivateKey<N> pk;

	std::pair<BitVector<N>, BitMatrix<N> > objectIndexPair = sk.getObjectIndexPair();

	BitVector<N> token = BitVector<N>::randomVector();

	BitVector<N> metadatumAddress = sk.getMetadataAddress(objectIndexPair, token);
}


int main(int argc, char **argv) {
	cout << "Search tests" << endl;
	testAddress();
	return 0;
}