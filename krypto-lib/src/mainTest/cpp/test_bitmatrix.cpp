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

/*
TEST(BitMatrixTests, testEquality){
	BitMatrix<N> S = BitMatrix<N>::randomInvertibleMatrix(N<<6);
	BitMatrix<N> Z = BitMatrix<N>::squareZeroMatrix();
	BitMatrix<N> I = BitMatrix<N>::squareIdentityMatrix();
	ASSERT_TRUE(S.equals(S));
	ASSERT_TRUE(Z.equals(Z));
	ASSERT_TRUE(I.equals(I));
	BitMatrix<N> Z_C = BitMatrix<N>::squareZeroMatrix();
	BitMatrix<N> I_C = BitMatrix<N>::squareIdentityMatrix(); 
	ASSERT_TRUE(Z.equals(Z_C));
	ASSERT_TRUE(Z_C.equals(Z));
	ASSERT_TRUE(I.equals(I_C));
	ASSERT_TRUE(I_C.equals(I));
	ASSERT_FALSE(I.equals(Z));
	ASSERT_FALSE(S.equals(Z));
	ASSERT_FALSE(Z.equals(I));
	ASSERT_FALSE(Z.equals(S));
}
*/

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

/*
	BitMatrix<N> X = BitMatrix<N>::randomInvertibleMatrix(N << 6);
	BitMatrix<N> R1 = Mi * X;
	BitMatrix<N> R2 = Mi.solve(X);
	BitMatrix<N> r1 = R1[2];
	BitMatrix<N> r2 = R2[2];
	ASSERT_TRUE(r1.equals(r2));
*/
}

TEST(BitMatrixTests, testIdentity){
	BitMatrix<N> I = BitMatrix<N>::squareIdentityMatrix();
	ASSERT_TRUE(I.rref().isIdentity());
}

TEST(BitMatrixTests, testSplit) {
	BitMatrix<2*N> I = BitMatrix<2*N>::squareIdentityMatrix();
	BitMatrix<2*N> Il = I.split_v(1, 2);
	BitMatrix<N> Ir = Il.split_h(1, 2);
	ASSERT_TRUE(Ir.isIdentity());
}

// TEST(BitMatrixTests, testAugH) {
// 	BitMatrix<N> I = BitMatrix<N>::squareIdentityMatrix();
// 	BitMatrix<N> Z = BitMatrix<N>::squareIdentityMatrix();
// 	BitMatrix<2*N> IZ = BitMatrix<2N>::aug_h(I, Z);
// 	vector<BitVector<2*N>> IZ_rows();
// 	BitMatrix <2*N> IZ_manual(IZ_rows);
// 	ASSERT_TRUE(true);
// }