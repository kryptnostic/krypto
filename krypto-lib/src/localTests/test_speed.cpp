#include "../main/cpp/MultiQuadTuple.h"
#include "../main/cpp/BridgeKey.h"
#include "../main/cpp/PublicKey.h"
#include "../main/cpp/EncryptedSearchPublicKey.h"
#include <iostream>
#include <time.h>

using namespace std;

#define L 1
#define M 1
#define H 2
#define N 1
#define DEBUG false
#define OPRUNS 10
#define TESTRUNS 100

void testOps1() {
 	PrivateKey<N, 2> pk;
	BridgeKey<N, 2> bk(pk);
	PublicKey<N, 2> pub(bk);
	clock_t diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		BitVector<N> x = BitVector<N>::randomVector();
		BitVector<2*N> encryptedX = pk.encrypt(x);

		BitMatrix<N> K = BitMatrix<N>::randomMatrix(N << 6);
		BitMatrix<4*N> Z = bk.getLMMZ(K);

		clock_t begin = clock();

		for (int i = 0; i < OPRUNS; ++i) {
			BitVector<2*N> encryptedLMM = pub.homomorphicLMM(Z, encryptedX);
		}

 		clock_t end = clock();
 		diff += (end - begin);
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " operations of LMM: " << double(diff) / (CLOCKS_PER_SEC * OPRUNS * TESTRUNS) << " sec" << endl;
}

void testOps2() {
 	PrivateKey<N, 2> pk;
	BridgeKey<N, 2> bk(pk);
	PublicKey<N, 2> pub(bk);
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

void testOps3() {
 	PrivateKey<N, 2> pk;
	BridgeKey<N, 2> bk(pk);
	PublicKey<N, 2> pub(bk);
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

void testClientRuns() {
	clock_t begin = clock();

	for (int i = 0; i < TESTRUNS; ++i) {
		// clock_t begin_i = clock();
		PrivateKey<N, 2> pk;

		BitMatrix<N> K = BitMatrix<N>::randomMatrix(N << 6);
		BridgeKey<N, 2> bk(pk);
		
		// clock_t end_i = clock();
		// cout << "Test Run #" << i << " time: " << double(end_i - begin_i) / (CLOCKS_PER_SEC) << " sec" << endl;
	}

 	clock_t end = clock();
 	cout << "Average time elapsed over " << TESTRUNS << " runs of private & bridge key generation: " << double(end - begin) / (CLOCKS_PER_SEC * TESTRUNS) << " sec" << endl;
}

void testPublicKeyRuns() {
	clock_t diff = 0;

	for (int i = 0; i < TESTRUNS; ++i) {
		PrivateKey<N, 2> pk;

		BitMatrix<N> K = BitMatrix<N>::randomMatrix(N << 6);
		BridgeKey<N, 2> bk(pk);

		clock_t begin_i = clock();
		PublicKey<N, 2> pub(bk);
		clock_t end_i = clock();

		diff += (end_i - begin_i);
	}

 	cout << "Average time elapsed over " << TESTRUNS << " runs of public key generation: " << double(diff) / (CLOCKS_PER_SEC * TESTRUNS) << " sec" << endl;
}

void testHash() {
	BitVector<N> t = BitVector<N>::randomVector(); //token
	BitVector<N> d = BitVector<N>::randomVector(); //document key
	BitVector<2*N> x = BitVector<2*N>::vCat(t, d);
	MultiQuadTuple<2*N, N> h = MultiQuadTuple<2*N, N>::randomMultiQuadTuple();
	BitVector<N> addr = h(x);
	PrivateKey<N, 2> pk;
	BitVector<2*N> encryptedAddress = pk.encrypt(addr);
	BridgeKey<N, 2> bk(pk);
	PublicKey<N, 2> pub(bk);
	EncryptedSearchPublicKey<N, 2> sk(pk); 
	
	BitVector<2*N> encryptedT = pk.encrypt(t);
	BitVector<2*N> encryptedD = pk.encrypt(d);
	clock_t begin = clock();
	BitVector<2*N> calculatedEncryptedAddress = sk.homomorphicHash(h, encryptedT, encryptedD);
	clock_t end = clock();
	cout << "Time for one search match computation: " << double(end - begin) / (CLOCKS_PER_SEC) << " sec" << endl;
	BitVector<N> dEAddr = pk.decrypt(calculatedEncryptedAddress); 
	//addr.print();
	//dEAddr.print();
	
}

int main(int argc, char **argv) {
	cout << "Speed tests with " << (N << 6) << " bit plaintext" << endl;
	testOps1();
	testOps2();
	testOps3();
	testClientRuns();
	testPublicKeyRuns();
	//testHash();
 	fclose(urandom);
	return 0;
}