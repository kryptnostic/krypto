#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/SearchPublicKey.h"

using namespace testing;

#define N 1

TEST(PublicKeyTest, testHash){
	BitVector<N> t = BitVector<N>::randomVector(); //token
	BitVector<N> d = BitVector<N>::randomVector(); //document key
	BitVector<2*N> x = BitVector<2*N>::vCat(t, d);
	MultiQuadTuple<2*N, N> h = MultiQuadTuple<2*N, N>::randomMultiQuadTuple();
	BitVector<N> addr = h(x);
	PrivateKey<N, 2> pk;
	BitVector<2*N> encryptedAddress = pk.encrypt(addr);
	
	SearchPrivateKey<N, 2> rk(pk, h);
	BridgeKey<N, 2> bk(pk);
	SearchPublicKey<N, 2> sk(rk, bk); 
	
	BitVector<2*N> encryptedT = pk.encrypt(t);
	BitVector<2*N> encryptedD = pk.encrypt(d);
	BitVector<2*N> calculatedEncryptedAddress = sk.homomorphicHash(encryptedT, encryptedD);
	BitVector<N> dEAddr = pk.decrypt(calculatedEncryptedAddress);

	ASSERT_TRUE(addr.equals(dEAddr));
}