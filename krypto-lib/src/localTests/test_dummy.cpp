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
#define TESTRUNS 1

int main(int argc, char **argv) {

	cout << "hello";

	// for (int i = 0; i < TESTRUNS; ++i) {
		PrivateKey<N, 2> pk;

		BitMatrix<N> K = BitMatrix<N>::randomInvertibleMatrix();

		BridgeKey<N, 2> bk(pk, K);
		//PublicKey<N, 2> pub(bk);

		BitVector<N> x = BitVector<N>::randomVector();
		BitVector<N> y = BitVector<N>::randomVector();
		// BitVector<2*N> encryptedX = pk.encrypt(x);
		// BitVector<2*N> encryptedY = pk.encrypt(y);
	// }

	cout << "bye";

	return 0;
}