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

TEST(PublicKeyTest, testPublicKey){
	PrivateKey<N, 2> pk;
	BridgeKey<N, 2> bk(pk, BitMatrix<N>::squareIdentityMatrix());
	PublicKey<N, 2> pub(bk);

	// BitVector<N> x; //zero vector
	// BitVector<2*N> encrypted = pk.encrypt(x);
	// BitVector<2*N> encryptedLMM = pub.homomorphicLMM(encrypted);
	// BitVector<N> unencryptedLMM = pk.decrypt(encryptedLMM); //should be zero
}