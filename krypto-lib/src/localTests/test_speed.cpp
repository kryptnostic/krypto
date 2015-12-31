#include "../main/cpp/MultiQuadTuple.h"
#include "../main/cpp/BridgeKey.h"
#include "../main/cpp/PublicKey.h"
#include <iostream>
#include <time.h>

using namespace std;

#define N 64
#define DEBUG false
#define OPRUNS 10
#define TESTRUNS 100

void testLMM() {
 	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);
	clock_t diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		BitVector<N> x = BitVector<N>::randomVector();
		BitVector<2*N> encryptedX = pk.encrypt(x);

		BitMatrix<N> K = BitMatrix<N>::randomMatrix();

		clock_t begin = clock();

		for (int i = 0; i < OPRUNS; ++i) {
			BitMatrix<2*N, 4*N> Z = bk.getLMMZ(K);
			BitVector<2*N> encryptedLMM = pub.homomorphicLMM(Z, encryptedX);
		}

 		clock_t end = clock();
 		diff += (end - begin);
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " operations of LMM: " << double(diff) / (CLOCKS_PER_SEC * OPRUNS * TESTRUNS) << " sec" << endl;
}

void testXOR() {
 	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);
	clock_t diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		BitVector<N> x = BitVector<N>::randomVector();
		BitVector<N> y = BitVector<N>::randomVector();
		BitVector<2*N> encryptedX = pk.encrypt(x);
		BitVector<2*N> encryptedY = pk.encrypt(y);

		clock_t begin = clock();

		for (int i = 0; i < OPRUNS; ++i) {
			BitVector<2*N> encryptedXOR = pub.homomorphicXOR(encryptedX, encryptedY);
		}

 		clock_t end = clock();
 		diff += (end - begin);
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " operations of XOR: " << double(diff) / (CLOCKS_PER_SEC * OPRUNS * TESTRUNS) << " sec" << endl;
}

void testAND() {
 	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);
	clock_t diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		BitVector<N> x = BitVector<N>::randomVector();
		BitVector<N> y = BitVector<N>::randomVector();
		BitVector<2*N> encryptedX = pk.encrypt(x);
		BitVector<2*N> encryptedY = pk.encrypt(y);

		clock_t begin = clock();

		for (int i = 0; i < OPRUNS; ++i) {
			BitVector<2*N> encryptedAND = pub.homomorphicAND(encryptedX, encryptedY);
		}

 		clock_t end = clock();
 		diff += (end - begin);
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " operations of AND: " << double(diff) / (CLOCKS_PER_SEC * OPRUNS * TESTRUNS) << " sec" << endl;
}

void testADD() {
 	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);
	clock_t diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		BitVector<N> x = BitVector<N>::randomVector();
		BitVector<N> y = BitVector<N>::randomVector();
		BitVector<2*N> encryptedX = pk.encrypt(x);
		BitVector<2*N> encryptedY = pk.encrypt(y);

		clock_t begin = clock();

		for (int i = 0; i < OPRUNS; ++i) {
			BitVector<2*N> encryptedADD = pub.homomorphicADD(encryptedX, encryptedY);
		}

 		clock_t end = clock();
 		diff += (end - begin);
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " operations of ADD: " << double(diff) / (CLOCKS_PER_SEC * OPRUNS * TESTRUNS) << " sec" << endl;
}

void testMULT() {
 	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);
	clock_t diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		BitVector<N> x = BitVector<N>::randomVectorLeadingZeroes(N/2);
		BitVector<N> y = BitVector<N>::randomVectorLeadingZeroes(N/2);
		BitVector<2*N> encryptedX = pk.encrypt(x);
		BitVector<2*N> encryptedY = pk.encrypt(y);

		clock_t begin = clock();

		for (int i = 0; i < OPRUNS; ++i) {
			BitVector<2*N> encryptedADD = pub.homomorphicMULT(encryptedX, encryptedY);
		}

 		clock_t end = clock();
 		diff += (end - begin);
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " operations of MULT: " << double(diff) / (CLOCKS_PER_SEC * OPRUNS * TESTRUNS) << " sec" << endl;
}

void testClientRuns() {
	clock_t begin = clock();

	for (int i = 0; i < TESTRUNS; ++i) {
		// clock_t begin_i = clock();
		PrivateKey<N> pk;

		BitMatrix<N> K = BitMatrix<N>::randomMatrix();
		BridgeKey<N> bk(pk);
		
		// clock_t end_i = clock();
		// cout << "Test Run #" << i << " time: " << double(end_i - begin_i) / (CLOCKS_PER_SEC) << " sec" << endl;
	}

 	clock_t end = clock();
 	cout << "Average time elapsed over " << TESTRUNS << " runs of private & bridge key generation: " << double(end - begin) / (CLOCKS_PER_SEC * TESTRUNS) << " sec" << endl;
}

void testPublicKeyRuns() {
	clock_t diff = 0;

	for (int i = 0; i < TESTRUNS; ++i) {
		PrivateKey<N> pk;

		BitMatrix<N> K = BitMatrix<N>::randomMatrix();
		BridgeKey<N> bk(pk);

		clock_t begin_i = clock();
		PublicKey<N> pub(bk);
		clock_t end_i = clock();

		diff += (end_i - begin_i);
	}

 	cout << "Average time elapsed over " << TESTRUNS << " runs of public key generation: " << double(diff) / (CLOCKS_PER_SEC * TESTRUNS) << " sec" << endl;
}

int main(int argc, char **argv) {
	// cout << "Speed tests with " << (N << 6) << " bit plaintext" << endl;
	testLMM();
	testXOR();
	testAND();
	testADD();
	testMULT();
	testClientRuns();
	testPublicKeyRuns();

 	//fclose(urandom);
	return 0;
}

