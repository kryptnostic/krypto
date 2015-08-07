#include "../main/cpp/MultiQuadTuple.h"
#include "../main/cpp/BridgeKey.h"
#include "../main/cpp/PublicKey.h"
#include <iostream>
#include <time.h>

using namespace std;

#define L 1
#define M 1 
#define H 2 
#define N 1
#define DEBUG false
#define OPRUNS 64
#define TESTRUNS 10

void testOps() {
	PrivateKey<N, 2> pk;

	BitMatrix<N> K = BitMatrix<N>::randomMatrix(N << 6);
	BridgeKey<N, 2> bk(pk);
	PublicKey<N, 2> pub(bk);

	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<N> y = BitVector<N>::randomVector();
	BitVector<2*N> encryptedX = pk.encrypt(x);
	BitVector<2*N> encryptedY = pk.encrypt(y);

	clock_t begin = clock();

	for (int i = 0; i < OPRUNS; ++i) {
		BitVector<2*N> encryptedLMM = pub.homomorphicLMM(K, encryptedX);
		BitVector<2*N> encryptedXOR = pub.homomorphicXOR(encryptedX, encryptedY);
		BitVector<2*N> encryptedAND = pub.homomorphicAND(encryptedX, encryptedY);
	}

 	clock_t end = clock();
 	cout << "Time elapsed over " << OPRUNS << " operations of LMM, XOR, and AND: " << double(end - begin) / (CLOCKS_PER_SEC) << " sec" << endl;
}

void testRuns() {
		clock_t begin = clock();

	for (int i = 0; i < TESTRUNS; ++i) {
		// clock_t begin_i = clock();
		PrivateKey<N, 2> pk;

		BitMatrix<N> K = BitMatrix<N>::randomMatrix(N << 6);
		BridgeKey<N, 2> bk(pk);
		PublicKey<N, 2> pub(bk);
		
		// clock_t end_i = clock();
		// cout << "Test Run #" << i << " time: " << double(end_i - begin_i) / (CLOCKS_PER_SEC) << " sec" << endl;
	}

 	clock_t end = clock();
 	cout << "Average time elapsed over " << TESTRUNS << " runs of key generation: " << double(end - begin) / (CLOCKS_PER_SEC * TESTRUNS) << " sec" << endl;
}

int main(int argc, char **argv) {
	testOps();
	testRuns();

 	fclose(urandom);
	return 0;
}