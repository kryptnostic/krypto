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
#define MON 7

TEST(SearchPrivateKeyTest, testIndexingAndSearch){
	SearchPrivateKey<N> sk;
	PrivateKey<N,MON> pk;

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

TEST(SearchPrivateKeyTest, testShare){
	SearchPrivateKey<N> sk_src;
	SearchPrivateKey<N> sk_dst;
	PrivateKey<N,MON> pk_src;
	PrivateKey<N,MON> pk_dst;

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

TEST(SearchPrivateKeyTest, testGetObjectIndexPairFromObjectSearchPair) {
	PrivateKey<N,MON> pk;
	SearchPrivateKey<N> spk;

	std::pair<BitVector<N>, BitMatrix<N>> objectIndexPair1 = spk.getObjectIndexPair();
	std::pair<BitVector<2*N>, BitMatrix<N>> objectSearchPair = spk.getObjectSearchPairFromObjectIndexPair(objectIndexPair1, pk);
	std::pair<BitVector<N>, BitMatrix<N>> objectIndexPair2 = spk.getObjectIndexPairFromObjectSearchPair(objectSearchPair, pk);

	ASSERT_TRUE(objectIndexPair2.first.equals(objectIndexPair1.first));
	ASSERT_TRUE(objectIndexPair2.second.equals(objectIndexPair1.second));
}
