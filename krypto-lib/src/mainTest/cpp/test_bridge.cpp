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

// TEST(BridgeKeyTest, testLMM){
// 	PrivateKey<N, 2> pk;
// 	BridgeKey<N, 2> bk(pk, BitMatrix<N>::squareIdentityMatrix());
// 	BitMatrix<2*N> Z = bk.get_LMM_Z();
// }