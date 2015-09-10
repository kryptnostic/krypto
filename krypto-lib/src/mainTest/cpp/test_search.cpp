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

TEST(SearchKeyTest, testIndexingAndSearch){
	SearchPrivateKey<N> sk;
	PrivateKey<N> pk;

	BitVector<N> objectSearchKey = sk.getObjectSearchKey();
	BitMatrix<N> objectAddressMatrix = sk.getObjectAddressMatrix();

	std::pair<BitVector<2*N>, BitMatrix<N> > objectSearchPair = sk.getObjectSearchPair(objectSearchKey, objectAddressMatrix, pk);

	BitVector<N> token = BitVector<N>::randomVector();

	BitVector<N> metadataAddress = sk.getMetadataAddress(objectAddressMatrix, objectSearchKey, token);

	BitVector<2*N> eToken = pk.encrypt(token);
	BitVector<2*N> eObjectSearchKey = pk.encrypt(objectSearchKey);

	ClientHashFunction<N> chf = sk.getClientHashFunction(pk);

	BitVector<N> calculatedAddress = sk.getObjectConversionMatrix(objectAddressMatrix) * chf(eToken, eObjectSearchKey);

	ASSERT_TRUE(metadataAddress.equals(calculatedAddress));
}

TEST(SearchKeyTest, testShare){
	SearchPrivateKey<N> sk_src;
	SearchPrivateKey<N> sk_dst;
	PrivateKey<N> pk_src;
	PrivateKey<N> pk_dst;

	//source client indexes a new document
	BitVector<N> objectSearchKey = sk_src.getObjectSearchKey();
	BitMatrix<N> objectAddressMatrix = sk_src.getObjectAddressMatrix();
	std::pair<BitVector<2*N>, BitMatrix<N> > sourceObjectSearchPair = sk_src.getObjectSearchPair(objectSearchKey, objectAddressMatrix, pk_src);

	BitVector<N> token = BitVector<N>::randomVector();
	BitVector<N> expectedAddress = sk_src.getMetadataAddress(objectAddressMatrix, objectSearchKey, token);

	//source client prepares the sharing pair
	std::pair<BitVector<N>, BitMatrix<N> > objectSharePair = sk_src.getObjectSharePair(sourceObjectSearchPair, pk_src);

	//destination client receives the sharing pair and prepares the upload pair
	std::pair<BitVector<2*N>, BitMatrix<N> > destinationObjectSearchPair = sk_dst.getObjectSearchPairFromObjectSharePair(objectSharePair, pk_dst);
	BitVector<N> calculatedAddress = sk_dst.getMetadataAddressFromPair(token, destinationObjectSearchPair, pk_dst);

	ASSERT_TRUE(expectedAddress.equals(calculatedAddress));
}