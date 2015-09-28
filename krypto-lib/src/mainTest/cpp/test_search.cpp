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

	std::pair<BitVector<N>, BitMatrix<N> > objectIndexPair = sk.getObjectIndexPair();

	std::pair<BitVector<2*N>, BitMatrix<N> > objectSearchPair = sk.getObjectSearchPairFromObjectIndexPair(objectIndexPair, pk);

	BitVector<N> token = BitVector<N>::randomVector();

	BitVector<N> metadataAddress = sk.getMetadataAddress(objectIndexPair, token);

	BitVector<2*N> eToken = pk.encrypt(token);
	BitVector<2*N> eObjectSearchKey = pk.encrypt(objectIndexPair.first);

	ClientHashFunction<N> chf = sk.getClientHashFunction(pk);

	BitVector<N> calculatedAddress = sk.getObjectConversionMatrix(objectIndexPair.second) * chf(eToken, eObjectSearchKey);

	ASSERT_TRUE(metadataAddress.equals(calculatedAddress));
}

TEST(SearchKeyTest, testShare){
	SearchPrivateKey<N> sk_src;
	SearchPrivateKey<N> sk_dst;
	PrivateKey<N> pk_src;
	PrivateKey<N> pk_dst;

	//source client indexes a new document
	std::pair<BitVector<N>, BitMatrix<N> > objectIndexPair = sk_src.getObjectIndexPair();

	std::pair<BitVector<2*N>, BitMatrix<N> > sourceObjectSearchPair = sk_src.getObjectSearchPairFromObjectIndexPair(objectIndexPair, pk_src);

	BitVector<N> token = BitVector<N>::randomVector();
	BitVector<N> expectedAddress = sk_src.getMetadataAddress(objectIndexPair, token);

	//source client prepares the sharing pair
	std::pair<BitVector<N>, BitMatrix<N> > objectSharePair = sk_src.getObjectSharePairFromObjectSearchPair(sourceObjectSearchPair, pk_src);

	//destination client receives the sharing pair and prepares the upload pair
	std::pair<BitVector<2*N>, BitMatrix<N> > destinationObjectSearchPair = sk_dst.getObjectSearchPairFromObjectSharePair(objectSharePair, pk_dst);
	BitVector<N> calculatedAddress = sk_dst.getMetadataAddressFromPair(token, destinationObjectSearchPair, pk_dst);

	ASSERT_TRUE(expectedAddress.equals(calculatedAddress));
}