/*
 * test_bridge.cpp
 *
 *  Created on: July 28, 2015
 *      Author: robinz16
 */

#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/BitVector.h"
#include "../../main/cpp/BitMatrix.h"
#include "../../main/cpp/BridgeKey.h"
#include <string>
using namespace testing;

#define N 1

TEST(BridgeKeyTest, testLMM){
	BitMatrix<2*N> Z = BridgeKey::get_LMM_Z();
}