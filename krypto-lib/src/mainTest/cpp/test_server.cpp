/*
 * test_server.cpp
 *
 *  Created on: August 27, 2015
 *      Author: howpenghui
 */

#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/SearchPrivateKey.h"
#include "../../main/cpp/KryptnosticServer.h"

using namespace testing;

#define N 128

TEST(KryptnosticServerTest, testInit){
	SearchPrivateKey<> sk;
	PrivateKey<> pk;

	ClientHashFunction<> chf = sk.getClientHashFunction(pk);
	BitVector<> objectSearchKey = sk.getObjectSearchKey();
	BitMatrix<> objectAddressMatrix = sk.getObjectAddressMatrix();

	std::pair<BitVector<>, BitMatrix<> > objectIndexPair = sk.getObjectIndexPair();

	std::pair<BitVector<2*N>, BitMatrix<N> > objectSearchPair = sk.getObjectSearchPairFromObjectIndexPair(objectIndexPair, pk);

	BitVector<> token = BitVector<>::randomVector();
	BitVector<> expectedAddress = sk.getMetadataAddress(objectIndexPair, token);

	BitVector<2*N> eToken = pk.encrypt(token);
	KryptnosticServer<> ks(chf, eToken);
	BitVector<> actualAddress = ks.getMetadataAddress(objectSearchPair);

	ASSERT_TRUE(expectedAddress.equals(actualAddress));
}