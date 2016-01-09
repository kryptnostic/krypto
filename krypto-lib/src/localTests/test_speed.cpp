#include "../main/cpp/MultiQuadTuple.h"
#include "../main/cpp/BridgeKey.h"
#include "../main/cpp/PublicKey.h"
#include <iostream>
#include <chrono>

using namespace std;

#define N 64
#define DEBUG false
#define OPRUNS 1
#define TESTRUNS 100

void testLMM() {
 	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);
	auto diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		BitVector<N> x = BitVector<N>::randomVector();
		BitVector<2*N> encryptedX = pk.encrypt(x);

		BitMatrix<N> K = BitMatrix<N>::randomMatrix();


		for (int i = 0; i < OPRUNS; ++i) {
			BitMatrix<2*N, 4*N> Z = bk.getLMMZ(K);
			auto begin = chrono::steady_clock::now();
			BitVector<2*N> encryptedLMM = pub.homomorphicLMM(Z, encryptedX);
 			auto end = chrono::steady_clock::now();
 			diff += (end - begin).count();	
		}
 	}
 	cout<< "Average time elapsed over " << OPRUNS * TESTRUNS << " operations of LMM: " << diff/double(OPRUNS * TESTRUNS) << " nanos" << endl;
}

void testXOR() {
 	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);
	auto diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		BitVector<N> x = BitVector<N>::randomVector();
		BitVector<N> y = BitVector<N>::randomVector();
		BitVector<2*N> encryptedX = pk.encrypt(x);
		BitVector<2*N> encryptedY = pk.encrypt(y);

		auto begin = chrono::steady_clock::now();

		for (int i = 0; i < OPRUNS; ++i) {
			BitVector<2*N> encryptedXOR = pub.homomorphicXOR(encryptedX, encryptedY);
		}

 		auto end = chrono::steady_clock::now();
 		diff += (end - begin).count();
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " operations of XOR: " << diff/double(OPRUNS * TESTRUNS) << " nanos" << endl;
}

void testAND() {
 	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);
	auto diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		BitVector<N> x = BitVector<N>::randomVector();
		BitVector<N> y = BitVector<N>::randomVector();
		BitVector<2*N> encryptedX = pk.encrypt(x);
		BitVector<2*N> encryptedY = pk.encrypt(y);

		auto begin = chrono::steady_clock::now();

		for (int i = 0; i < OPRUNS; ++i) {
			BitVector<2*N> encryptedAND = pub.homomorphicAND(encryptedX, encryptedY);
		}

 		auto end = chrono::steady_clock::now();
 		diff += (end - begin).count();
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " operations of AND: " << diff/double(OPRUNS * TESTRUNS) << " nanos" << endl;
}

void testSHIFT() {
 	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);
	auto diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		BitVector<N> x = BitVector<N>::randomSmallVector();
		BitVector<2*N> encryptedX = pk.encrypt(x);

		auto begin = chrono::steady_clock::now();

		for (int i = 0; i < OPRUNS; ++i) {
			BitVector<2*N> encryptedSHIFT = pub.homomorphicLEFTSHIFT(encryptedX);
		}

 		auto end = chrono::steady_clock::now();
 		diff += (end - begin).count();
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " operations of SHIFT: " << diff/double(OPRUNS * TESTRUNS) << " nanos" << endl;
}

void testADD() {
 	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);
	auto diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		BitVector<N> x = BitVector<N>::randomVector();
		BitVector<N> y = BitVector<N>::randomVector();
		BitVector<2*N> encryptedX = pk.encrypt(x);
		BitVector<2*N> encryptedY = pk.encrypt(y);

		auto begin = chrono::steady_clock::now();

		for (int i = 0; i < OPRUNS; ++i) {
			BitVector<2*N> encryptedADD = pub.homomorphicADD(encryptedX, encryptedY);
		}

 		auto end = chrono::steady_clock::now();
 		diff += (end - begin).count()/double(OPRUNS * TESTRUNS);
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " operations of ADD: " << diff << " nanos" << endl;
}

void testMULT() {
 	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);
	auto diff = 0;

	for (int run = 0; run < TESTRUNS; ++run) {
		BitVector<N> x = BitVector<N>::randomVectorLeadingZeroes(N/2);
		BitVector<N> y = BitVector<N>::randomVectorLeadingZeroes(N/2);
		BitVector<2*N> encryptedX = pk.encrypt(x);
		BitVector<2*N> encryptedY = pk.encrypt(y);

		auto begin = chrono::steady_clock::now();

		for (int i = 0; i < OPRUNS; ++i) {
			BitVector<2*N> encryptedMULT = pub.homomorphicMULT(encryptedX, encryptedY);
		}

 		auto end = chrono::steady_clock::now();
 		diff += (end - begin).count()/double(OPRUNS * TESTRUNS);
 	}
 	cout << "Average time elapsed over " << OPRUNS * TESTRUNS << " operations of MULT: " << diff << " nanos" << endl;
}

void testClientRuns() {
	auto diff = 0;
	auto begin = chrono::steady_clock::now();

	for (int i = 0; i < TESTRUNS; ++i) {
		PrivateKey<N> pk;

		BitMatrix<N> K = BitMatrix<N>::randomMatrix();
		BridgeKey<N> bk(pk);
	}

 	auto end = chrono::steady_clock::now();
 	diff += (end - begin).count();
 	cout << "Average time elapsed over " << TESTRUNS << " runs of private & bridge key generation: " << diff/double(TESTRUNS) << " nanos" << endl;
}

void testPublicKeyRuns() {
	auto diff = 0;

	for (int i = 0; i < TESTRUNS; ++i) {
		PrivateKey<N> pk;

		BitMatrix<N> K = BitMatrix<N>::randomMatrix();
		BridgeKey<N> bk(pk);

		auto begin_i = chrono::steady_clock::now();
		PublicKey<N> pub(bk);
		auto end_i = chrono::steady_clock::now();

		diff += (end_i - begin_i).count()/double(TESTRUNS);
	}

 	cout << "Average time elapsed over " << TESTRUNS << " runs of public key generation: " << diff << " nanos" << endl;
}

int main(int argc, char **argv) {
	// cout << "Speed tests with " << N << " bit plaintext" << endl;
	testLMM();
	testXOR();
	testAND();
	testSHIFT();
	testADD();
	testMULT();
	testClientRuns();
	testPublicKeyRuns();
 	//fclose(urandom);
	return 0;
}

