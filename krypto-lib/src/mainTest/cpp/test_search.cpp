/*
 * test_search.cpp
 *
 *  Created on: August 24, 2015
 *      Author: howpenghui
 */

#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/SearchPrivateKey.h"

using namespace testing;

#define N 128

TEST(SearchKeyTest, testIndexingAndSearch){
	SearchPrivateKey<N> sk;
	PrivateKey<N> pk;

	BitVector<N> objectSearchKey = sk.getObjectSearchKey();
	BitMatrix<N> objectAddressFunction = sk.getObjectAddressMatrix();

	std::pair<BitVector<2*N>, BitMatrix<N> > objectIndexPair = sk.getObjectIndexPair(objectSearchKey, objectAddressFunction, pk);

	BitVector<N> token = BitVector<N>::randomVector();

	BitVector<N> metadatumAddress = sk.getMetadatumAddress(objectAddressFunction, objectSearchKey, token);

	BitVector<2*N> eToken = pk.encrypt(token);
	BitVector<2*N> eObjectSearchKey = pk.encrypt(objectSearchKey);

	ClientHashFunction<N> chf = sk.getClientHashFunction(pk);

	BitVector<N> calculatedAddress = sk.getObjectConversionMatrix(objectAddressFunction) * chf(eToken, eObjectSearchKey);

	ASSERT_TRUE(metadatumAddress.equals(calculatedAddress));
}

TEST(SearchKeyTest, testSharing){
	SearchPrivateKey<N> sk_src;
	SearchPrivateKey<N> sk_dst;
	PrivateKey<N> pk_src;
	PrivateKey<N> pk_dst;

	//source client indexes a new document
	BitVector<N> objectSearchKey = sk_src.getObjectSearchKey();
	BitMatrix<N> objectAddressFunction = sk_src.getObjectAddressMatrix();
	std::pair<BitVector<2*N>, BitMatrix<N> > sourceObjectIndexPair = sk_src.getObjectIndexPair(objectSearchKey, objectAddressFunction, pk_src);

	BitVector<N> token = BitVector<N>::randomVector();
	BitVector<N> expectedAddress = sk_src.getMetadatumAddress(objectAddressFunction, objectSearchKey, token);

	//source client prepares the sharing pair
	std::pair<BitVector<N>, BitMatrix<N> > objectSharingPair = sk_src.getObjectSharingPair(sourceObjectIndexPair, pk_src);

	//destination client receives the sharing pair and prepares the upload pair
	std::pair<BitVector<2*N>, BitMatrix<N> > destinationObjectIndexPair = sk_dst.getObjectIndexPairFromSharing(objectSharingPair, pk_dst);
	BitVector<N> calculatedAddress = sk_dst.getMetadatumAddressFromPair(token, destinationObjectIndexPair, pk_dst);

	ASSERT_TRUE(expectedAddress.equals(calculatedAddress));
}