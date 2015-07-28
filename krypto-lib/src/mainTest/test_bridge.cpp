/*
 * test_bitmatrix.cpp
 *
 *  Created on: Mar 11, 2015
 *      Author: mtamayo
 */

#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/BitVector.h"
#include "../../main/cpp/BitMatrix.h"
#include "../../main/cpp/BridgeKey.h"
#include <string>
using namespace testing;

#define N 1

TEST(BitMatrixTest, testLMM){
	BitMatrix<2*N> Z = BridgeKey::get_LMM_Z();
}