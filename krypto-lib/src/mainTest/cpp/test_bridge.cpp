/*
 * test_bridge.cpp
 *
 *  Created on: July 28, 2015
 *      Author: robinz16
 */

#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/BitVector.h"
#include "../../main/cpp/BitMatrix.h"
#include "../../main/cpp/PrivateKey.h"
#include "../../main/cpp/BridgeKey.h"
#include <string>
using namespace testing;

#define N 1

TEST(BridgeKeyTest, testLMM){
	PrivateKey<N, 2*N> pk;
	BridgeKey<N, 2*N> bk(pk, BitMatrix<N>::squareIdentityMatrix());

	MultiQuadTuple<2*N, 2*N> u_g1 = bk.get_UNARY_g1();
	MultiQuadTuple<2*N, 2*N> u_g2 = bk.get_UNARY_g2();

	BitMatrix<4*N> Z = bk.get_LMM_Z();
}

TEST(BridgeKeyTest, testXOR){
	PrivateKey<N, 2*N> pk;
	BridgeKey<N, 2*N> bk(pk, BitMatrix<N>::squareIdentityMatrix());

	MultiQuadTuple<2*N, 3*N> b_gx1 = bk.get_BINARY_gx1();
	MultiQuadTuple<2*N, 3*N> b_gy1 = bk.get_BINARY_gy1();
	MultiQuadTuple<3*N, 3*N> b_g2 = bk.get_BINARY_g2();

	BitMatrix<2*N> Xx = bk.get_XOR_Xx();
	BitMatrix<2*N> Xy = bk.get_XOR_Xy();
	BitMatrix<3*N> Y = bk.get_XOR_Y();
}

TEST(BridgeKeyTest, testAND){
	PrivateKey<N, 2*N> pk;
	BridgeKey<N, 2*N> bk(pk, BitMatrix<N>::squareIdentityMatrix());

	BitMatrix<2*N> Z1 = bk.get_AND_Z1();
	BitMatrix<2*N> Z2 = bk.get_AND_Z2();
	BitMatrix<N> z = bk.get_AND_z();
}