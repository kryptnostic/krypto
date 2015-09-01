#include <iostream>
#include "../main/cpp/PublicKey.h"

#define N 64

int main(){
	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);

	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<N> y = BitVector<N>::randomVector();
	const BitVector<2*N> & eX = pk.encrypt(x);
	const BitVector<2*N> & eY = pk.encrypt(y);

	BitVector<2*N> z = pub.homomorphicAND(eX, eY);

	return 0;
}