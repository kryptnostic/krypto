/*
 * test_bitmatrix.cpp
 *
 *  Created on: Mar 11, 2015
 *      Author: mtamayo
 */

#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/BitVector.h"
#include "../../main/cpp/BitMatrix.h"
#include <string>
using namespace testing;

TEST(BitMatrixTests, testRandomMatrix) {
	//BitMatrix<1> m = BitMatrix<1>::randomMatrix(64);
    //BitVector<1> v = BitVector<1>::randomVector();
    
    sadfas
	ASSERT_TRUE( 1+1 == 2 );
    ASSERT_TRUE( 1+2 == 3);
	/*
	printf("Generated random matrix.");
	{
		const char * unformatted_msg = "Row count is incorrect: %d";
		size_t bufLen = strlen(unformatted_msg) + 32;
		char trace_msg[bufLen];
		snprintf(trace_msg, bufLen, unformatted_msg, m.rowCount());
		SCOPED_TRACE(trace_msg);

		ASSERT_TRUE(m.rowCount() == 64);
	}

	for (int i = 0; i < m.rowCount(); ++i) {
		SCOPED_TRACE("Unexpected zero vector.");
		ASSERT_FALSE(m[i].isZero());
	}
	*/
}

