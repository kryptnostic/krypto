/*
 * test_bridge.cpp
 *
 *  Created on: July 28, 2015
 *      Author: robinz16
 */

#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/BitVector.h"
#include "../../main/cpp/BitMatrix.h"
#include "../../main/cpp/PrivateKey.h"
#include "../../main/cpp/BridgeKey.h"
#include "../../main/cpp/PublicKey.h"
#include <string>
using namespace testing;

#define N 1

TEST(PublicKeyTest, testLMM){
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
	BitVector<N> expectedLMM = K.template operator*<N>(x);

	ASSERT_TRUE(expectedLMM.equals(unencryptedLMM));
}

TEST(PublicKeyTest, testXOR){
	PrivateKey<N, 2> pk;
	BitMatrix<N> K = BitMatrix<N>::randomMatrix(N << 6);
	BridgeKey<N, 2> bk(pk, K);
	PublicKey<N, 2> pub(bk);

	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<N> y = BitVector<N>::randomVector();
	BitVector<2*N> encryptedX = pk.encrypt(x);
	BitVector<2*N> encryptedY = pk.encrypt(y);

	BitVector<2*N> encryptedXOR = pub.homomorphicXOR(encryptedX, encryptedY);
	BitVector<N> unencryptedXOR = pk.decrypt(encryptedXOR); 
	BitVector<N> expectedXOR = x ^ y;

	ASSERT_TRUE(expectedXOR.equals(unencryptedXOR));
}

TEST(PublicKeyTest, testAND){
	PrivateKey<N, 2> pk;
	BitMatrix<N> K = BitMatrix<N>::randomMatrix(N << 6);
	BridgeKey<N, 2> bk(pk, K);
	PublicKey<N, 2> pub(bk);

	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<N> y = BitVector<N>::randomVector();
	BitVector<2*N> encryptedX = pk.encrypt(x);
	BitVector<2*N> encryptedY = pk.encrypt(y);
	
	BitVector<2*N> encryptedAND = pub.homomorphicAND(encryptedX, encryptedY);
	BitVector<N> unencryptedAND = pk.decrypt(encryptedAND); 
	BitVector<N> expectedAND = x & y;

	ASSERT_TRUE(expectedAND.equals(unencryptedAND));	
}