/*
 * test_bridge.cpp
 *
 *  Created on: July 28, 2015
 *      Author: robinz16
 */

#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/PublicKey.h"
#include <chrono>

using namespace testing;

#define N 64
#define MON 7

TEST(PublicKeyTest, testLMM){
	ASSERT_TRUE(1+1 == 2);
	PrivateKey<N,MON> pk;
	BitMatrix<N> K = BitMatrix<N>::randomMatrix();

	auto start = std::chrono::high_resolution_clock::now();
	BridgeKey<N,MON> bk(pk);
	PublicKey<N,MON> pub(bk);
	auto stop = std::chrono::high_resolution_clock::now();
	auto total = std::chrono::duration<double,micro>(stop - start).count();
	std::cout << "Time used to generate PubKey and BridgeKey from PrivateKey: " << total << " \xC2\xB5s" << std::endl;


	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<N> y = BitVector<N>::randomVector();
	BitVector<2*N> encryptedX = pk.encrypt(x);
	BitVector<2*N> encryptedY = pk.encrypt(y);

	BitMatrix<2*N, 4*N> Z = bk.getLMMZ(K);

	begin = clock();
	BitVector<2*N> encryptedLMM = pub.homomorphicLMM(Z, encryptedX);
	end = clock();
	std::cout << "Time used to compute encryptedLMM: " << total << " \xC2\xB5s" << std::endl;
	BitVector<N> unencryptedLMM = pk.decrypt(encryptedLMM);
	BitVector<N> expectedLMM = K * x;

	ASSERT_TRUE(expectedLMM.equals(unencryptedLMM));
}

TEST(PublicKeyTest, testXOR){
	PrivateKey<N,MON> pk;
	BridgeKey<N,MON> bk(pk);
	PublicKey<N,MON> pub(bk);

	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<N> y = BitVector<N>::randomVector();
	BitVector<2*N> encryptedX = pk.encrypt(x);
	BitVector<2*N> encryptedY = pk.encrypt(y);

	auto start = std::chrono::high_resolution_clock::now();
	BitVector<2*N> encryptedXOR = pub.homomorphicXOR(encryptedX, encryptedY);
	auto stop = std::chrono::high_resolution_clock::now();
	auto total = std::chrono::duration<double,micro>(stop - start).count();
	std::cout << "Time used to compute encryptedXOR: " << total << " \xC2\xB5s" << std::endl;

	BitVector<N> unencryptedXOR = pk.decrypt(encryptedXOR);
	BitVector<N> expectedXOR = x ^ y;

	ASSERT_TRUE(expectedXOR.equals(unencryptedXOR));
}

TEST(PublicKeyTest, testAND){
	PrivateKey<N,MON> pk;
	BridgeKey<N,MON> bk(pk);
	PublicKey<N,MON> pub(bk);

	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<N> y = BitVector<N>::randomVector();
	BitVector<2*N> encryptedX = pk.encrypt(x);
	BitVector<2*N> encryptedY = pk.encrypt(y);

	auto start = std::chrono::high_resolution_clock::now();
	BitVector<2*N> encryptedAND = pub.homomorphicAND(encryptedX, encryptedY);
	auto stop = std::chrono::high_resolution_clock::now();
	auto total = std::chrono::duration<double,micro>(stop - start).count();
	std::cout << "Time used to compute encryptedAND: " << total << " \xC2\xB5s" << std::endl;

	BitVector<N> unencryptedAND = pk.decrypt(encryptedAND);
	BitVector<N> expectedAND = x & y;

	ASSERT_TRUE(expectedAND.equals(unencryptedAND));
}

TEST(PublicKeyTest, testLEFTSHIFT){
	PrivateKey<N,MON> pk;
	BridgeKey<N,MON> bk(pk);
	PublicKey<N,MON> pub(bk);

	BitVector<N> x = BitVector<N>::randomVector();
	x.clear(0); //make sure the leftmost bit is zeroed (this restriction can be removed later)
	BitVector<2*N> encryptedX = pk.encrypt(x);

	auto start = std::chrono::high_resolution_clock::now();
	BitVector<2*N> encryptedLS = pub.homomorphicLEFTSHIFT(encryptedX);
	auto stop = std::chrono::high_resolution_clock::now();
	auto total = std::chrono::duration<double,micro>(stop - start).count();
	std::cout << "Time used to compute encrypted left shift: " << total << " \xC2\xB5s" << std::endl;

	BitVector<N> unecryptedLS = pk.decrypt(encryptedLS);

	ASSERT_TRUE(unecryptedLS.equals(x.leftShift(1)));
}

TEST(PublicKeyTest, testRIGHTSHIFT){
	PrivateKey<N,MON> pk;
	BridgeKey<N,MON> bk(pk);
	PublicKey<N,MON> pub(bk);

	BitVector<N> x = BitVector<N>::randomVector();
	x.clear(N-1); //make sure the rightmost bit is zeroed (this restriction can be removed later)
	BitVector<2*N> encryptedX = pk.encrypt(x);

	auto start = std::chrono::high_resolution_clock::now();
	BitVector<2*N> encryptedRS = pub.homomorphicRIGHTSHIFT(encryptedX);
	auto stop = std::chrono::high_resolution_clock::now();
	auto total = std::chrono::duration<double,micro>(stop - start).count();
	std::cout << "Time used to compute encrypted right shift: " << total << " \xC2\xB5s" << std::endl;

	BitVector<N> unecryptedRS = pk.decrypt(encryptedRS);

	ASSERT_TRUE(unecryptedRS.equals(x.rightShift(1)));
}

TEST(PublicKeyTest, testADD){
	PrivateKey<N,MON> pk;
	BridgeKey<N,MON> bk(pk);
	PublicKey<N,MON> pub(bk);
	BitVector<N> x = BitVector<N>::randomVectorLeadingZeroes(2);
	BitVector<N> y = BitVector<N>::randomVectorLeadingZeroes(2);
	BitVector<2*N> encryptedX = pk.encrypt(x);
	BitVector<2*N> encryptedY = pk.encrypt(y);

	auto start = std:::chrono::high_resolution_clock::now();
	BitVector<2*N> encryptedSum = pub.homomorphicADD(encryptedX, encryptedY);
	auto stop = std::chrono::high_resolution_clock::now();
	auto total = std::chrono::duration<double,milli>(stop - start).count();
	std::cout << "Time used to computed encryptedADD: " << total << " ms" << std::endl;

	BitVector<N> actualSum = pk.decrypt(encryptedSum);
	BitVector<N> expectedSum = x + y;

	ASSERT_TRUE(actualSum.equals(expectedSum)); 
}

TEST(PublicKeyTest, testMULT){
	PrivateKey<N,MON> pk;
	BridgeKey<N,MON> bk(pk);
	PublicKey<N,MON> pub(bk);
	BitVector<N> x = BitVector<N>::randomVectorLeadingZeroes(N/2);
	BitVector<N> y = BitVector<N>::randomVectorLeadingZeroes(N/2);
	BitVector<2*N> encryptedX = pk.encrypt(x);
	BitVector<2*N> encryptedY = pk.encrypt(y);

	auto start = std::chrono::high_resolution_clock::now();
	BitVector<2*N> encryptedProd = pub.homomorphicMULT(encryptedX, encryptedY);
	auto stop = std::chrono::high_resolution_clock::now();
	auto total = std::chrono::duration<double>(stop - start).count();
	std::cout << "Time used to compute encryptedMULT: " << total << " s" << std::endl;

	BitVector<N> actualProd = pk.decrypt(encryptedProd);
	BitVector<N> expectedProd = x * y;

	ASSERT_TRUE(actualProd.equals(expectedProd));
}