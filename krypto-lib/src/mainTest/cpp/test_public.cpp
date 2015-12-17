/*
 * test_bridge.cpp
 *
 *  Created on: July 28, 2015
 *      Author: robinz16
 */

#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/PublicKey.h"
#include <time.h>

using namespace testing;

#define N 64

TEST(PublicKeyTest, testLMM){
	ASSERT_TRUE(1+1 == 2);
	PrivateKey<N> pk;
	BitMatrix<N> K = BitMatrix<N>::randomMatrix();

	clock_t begin = clock();
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);
	clock_t end = clock();
	std::cout << "Time used to generate PubKey from PrivKey and BridgeKey: " << double(end - begin) / CLOCKS_PER_SEC << "sec" << endl;


	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<N> y = BitVector<N>::randomVector();
	BitVector<2*N> encryptedX = pk.encrypt(x);
	BitVector<2*N> encryptedY = pk.encrypt(y);

	BitMatrix<2*N, 4*N> Z = bk.getLMMZ(K);

	begin = clock();
	BitVector<2*N> encryptedLMM = pub.homomorphicLMM(Z, encryptedX);
	end = clock();
	std::cout << "Time used to compute encryptedLMM: " << double(end - begin) / CLOCKS_PER_SEC << " sec" << endl;
	BitVector<N> unencryptedLMM = pk.decrypt(encryptedLMM);
	BitVector<N> expectedLMM = K * x;

	ASSERT_TRUE(expectedLMM.equals(unencryptedLMM));
}

TEST(PublicKeyTest, testXOR){
	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);

	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<N> y = BitVector<N>::randomVector();
	BitVector<2*N> encryptedX = pk.encrypt(x);
	BitVector<2*N> encryptedY = pk.encrypt(y);

	clock_t begin = clock();
	BitVector<2*N> encryptedXOR = pub.homomorphicXOR(encryptedX, encryptedY);
	clock_t end = clock();
	std::cout << "Time used to compute encryptedXOR: " << double(end - begin) / CLOCKS_PER_SEC << " sec" << endl;

	BitVector<N> unencryptedXOR = pk.decrypt(encryptedXOR);
	BitVector<N> expectedXOR = x ^ y;

	ASSERT_TRUE(expectedXOR.equals(unencryptedXOR));
}

TEST(PublicKeyTest, testAND){
	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);

	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<N> y = BitVector<N>::randomVector();
	BitVector<2*N> encryptedX = pk.encrypt(x);
	BitVector<2*N> encryptedY = pk.encrypt(y);

	/*
	clock_t begin = clock();
	BitVector<2*N> encryptedAND = pub.homomorphicAND(encryptedX, encryptedY);
	clock_t end = clock();
	std::cout << "Time used to compute encryptedAND: " << double(end - begin) / CLOCKS_PER_SEC << " sec" << endl;
	BitVector<N> unencryptedAND = pk.decrypt(encryptedAND);
	BitVector<N> expectedAND = x & y;

	ASSERT_TRUE(expectedAND.equals(unencryptedAND));
	*/
}

TEST(PublicKeyTest, testLEFTSHIFT){
	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);

	BitVector<N> x = BitVector<N>::randomVector();
	x.clear(0); //make sure the leftmost bit is zeroed (this restriction can be removed later)
	BitVector<2*N> encryptedX = pk.encrypt(x);

	clock_t begin = clock();
	BitVector<2*N> encryptedLS = pub.homomorphicLEFTSHIFT(encryptedX);
	clock_t end = clock();
	std::cout << "Time used to compute encrypted left shift: " << double(end - begin) / CLOCKS_PER_SEC << "sec" << endl;

	BitVector<N> unecryptedLS = pk.decrypt(encryptedLS);

	ASSERT_TRUE(unecryptedLS.equals(x.leftShift(1)));
}

TEST(PublicKeyTest, testRIGHTSHIFT){
	PrivateKey<N> pk;
	BridgeKey<N> bk(pk);
	PublicKey<N> pub(bk);

	BitVector<N> x = BitVector<N>::randomVector();
	x.clear(N-1); //make sure the rightmost bit is zeroed (this restriction can be removed later)
	BitVector<2*N> encryptedX = pk.encrypt(x);

	clock_t begin = clock();
	BitVector<2*N> encryptedRS = pub.homomorphicRIGHTSHIFT(encryptedX);
	clock_t end = clock();
	std::cout << "Time used to compute encrypted right shift: " << double(end - begin) / CLOCKS_PER_SEC << "sec" << endl;

	BitVector<N> unecryptedRS = pk.decrypt(encryptedRS);

	ASSERT_TRUE(unecryptedRS.equals(x.rightShift(1)));
}