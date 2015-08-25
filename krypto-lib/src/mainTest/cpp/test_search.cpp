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

TEST(SearchKeyTest, testCallsWithoutPrivateKey){
	SearchPrivateKey<N> sk;
	BitVector<N> objectSearchKey = sk.getObjectSearchKey();
	BitMatrix<N, 2*N> objectAddressFunction = sk.getObjectAddressFunction();
	BitMatrix<N> objectConversionMatrix = sk.getObjectConversionMatrix(objectAddressFunction);
}

TEST(SearhKeyTest, testClientHashFunction){
	SearchPrivateKey<N> sk;
	PrivateKey<N> pk;
	ClientHashFunction<N> chf = sk.getClientHashFunction(pk);
	BitVector<N> searchToken = BitVector<N>::randomVector();
	BitVector<2*N> eSearchToken = pk.encrypt(searchToken);
	BitVector<N> objectSearchKey = sk.getObjectSearchKey();
	BitVector<2*N> eObjectSearchKey = pk.encrypt(objectSearchKey);
	BitVector<N> metadatumAddress = chf(eSearchToken, eObjectSearchKey);
}