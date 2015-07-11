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

#define N 1

TEST(BitMatrixTests, testRandomMatrix) {
	BitMatrix<1> m = BitMatrix<1>::randomMatrix(64);
    BitVector<1> v = BitVector<1>::randomVector();
    
	ASSERT_TRUE( 1+1 == 2 );
    ASSERT_TRUE( 1+2 == 3);
	
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
	
}

TEST(BitMatrixTests, testAssignment){
	BitMatrix<N> A = BitMatrix<N>::randomInvertibleMatrix(N<<6);
	A.set(0,0), A.set(1,1), A.set((N<<6)-1,(N<<6)-1);
	ASSERT_TRUE(A.get(0,0) && A.get(1,1) && A.get((N<<6)-1,(N<<6)-1));
}

TEST(BitMatrixTests, testSolve) {
 ASSERT_TRUE( 1+5 == 6 );
	BitVector<N> v = BitVector<N>::randomVector();
	BitMatrix<N> M = BitMatrix<N>::randomInvertibleMatrix(N << 6);
	BitVector<N> x = M.solve(v);
	BitVector<N> mx = M*x;
	ASSERT_TRUE(mx.equals(v));
}

TEST(BitMatrixTests, testInv) {
	ASSERT_TRUE(1+5 == 6);
	BitMatrix<N> M = BitMatrix<N>::randomInvertibleMatrix(N << 6);
	BitMatrix<N> Mi = M.inv();
	BitMatrix<N> Il = M*Mi;
	BitMatrix<N> Ir = Mi*M;
	ASSERT_TRUE(Il.isIdentity());
	ASSERT_TRUE(Ir.isIdentity());
	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<N> mix = Mi*x;
	ASSERT_TRUE(mix.equals(M.solve(x)));
}

TEST(BitMatrixTests, testIdentity){
	BitMatrix<N> I = BitMatrix<N>::squareIdentityMatrix();
	ASSERT_TRUE(I.rref().isIdentity());
}