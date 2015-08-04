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
#define TESTRUNS 20

int main(int argc, char **argv) {
	clock_t begin = clock();

	for (int i = 0; i < TESTRUNS; ++i) {
		PrivateKey<N, 2> pk;

		BitMatrix<N> K = BitMatrix<N>::randomMatrix(N << 6);
		BridgeKey<N, 2> bk(pk, K);
		PublicKey<N, 2> pub(bk);

		BitVector<N> x = BitVector<N>::randomVector();
		BitVector<N> y = BitVector<N>::randomVector();
		BitVector<2*N> encryptedX = pk.encrypt(x);
		BitVector<2*N> encryptedY = pk.encrypt(y);

		BitVector<2*N> encryptedLMM = pub.homomorphicLMM(encryptedX);
		BitVector<N> unencryptedLMM = pk.decrypt(encryptedLMM);

		BitVector<2*N> encryptedXOR = pub.homomorphicXOR(encryptedX, encryptedY);
		BitVector<N> unencryptedXOR = pk.decrypt(encryptedXOR);

		BitVector<2*N> encryptedAND = pub.homomorphicAND(encryptedX, encryptedY);
		BitVector<N> unencryptedAND = pk.decrypt(encryptedAND);
	}

 	clock_t end = clock();
 	cout << "Average time elapsed over " << TESTRUNS << " runs: " << double(end - begin) / (CLOCKS_PER_SEC * TESTRUNS) << " sec" << endl;

 	fclose(urandom);
	return 0;
}