/*
 * test_search.cpp
 *
 *  Created on: August 24, 2015
 *      Author: howpenghui
 */

#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/SearchPrivateKey.h"

using namespace testing;

#define N 64

TEST(SearchKeyTest, testClientHashFunctionCall){
	SearchPrivateKey<N> sk;
	PrivateKey<N> pk;
	ClientHashFunction<N> chf = sk.getClientHashFunction(pk);
}

TEST(SearchKeyTest, testIndexingAndSearch){
	SearchPrivateKey<N> sk;
	PrivateKey<N> pk;

	BitVector<N> objectSearchKey = sk.getObjectSearchKey();
	BitMatrix<N> objectAddressFunction = sk.getObjectAddressFunction();

	std::pair<BitVector<2*N>, BitMatrix<N> > objectIndexPair = sk.getObjectIndexPair(objectSearchKey, objectAddressFunction, pk);

	BitVector<N> token = BitVector<N>::randomVector();

	BitVector<N> metadatumAddress = sk.getMetadatumAddress(objectAddressFunction, token, objectSearchKey);

	BitVector<2*N> eToken = pk.encrypt(token);
	BitVector<2*N> eObjectSearchKey = pk.encrypt(objectSearchKey);

	BitMatrix<N> objectConversionMatrix = sk.getObjectConversionMatrix(objectAddressFunction);
	ClientHashFunction<N> chf = sk.getClientHashFunction(pk);

	BitVector<N> calculatedAddress = objectConversionMatrix * chf(eToken, eObjectSearchKey);

	metadatumAddress.print();
	calculatedAddress.print();

	//TODO: assert
}

TEST(SearchKeyTest, testSharing){
	SearchPrivateKey<N> sk_src;
	SearchPrivateKey<N> sk_dst;
	PrivateKey<N> pk_src;
	PrivateKey<N> pk_dst;

	//TODO: test sharing
}