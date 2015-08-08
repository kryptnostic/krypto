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
	testOps();
	testRuns();
	testHash();
 	fclose(urandom);
	return 0;
}