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
	//BitMatrix<N, 2*N> objectAddressFunction = sk.getObjectAddressFunction();
	//BitMatrix<N> objectConversionMatrix = sk.getObjectConversionMatrix(objectAddressFunction);
}

TEST(SearchKeyTest, testIndexing){
	SearchPrivateKey<N> sk;
	PrivateKey<N> pk;

	BitVector<N> token = BitVector<N>::randomVector();

	BitVector<N> objectSearchKey = sk.getObjectSearchKey();
	//std::pair<BitVector<2*N>, BitMatrix<N> > objectIndexPair = sk.getObjectIndexPair(pk);
}