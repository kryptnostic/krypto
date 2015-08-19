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

#define N 64

TEST(BitMatrixTest, testProvidedConstructors) {
    BitMatrix<N> m = BitMatrix<N>::randomMatrix();
    BitMatrix<N> m2 = m;

    if( m[ 10 ][ 25 ] ) {
        m[ 10 ].clear( 25 );
    } else {
        m[ 10 ].set( 25 );
    }

    ASSERT_TRUE( m.operator!=( m2 ) );
}

TEST(BitMatrixTest, testIdentity){
	BitMatrix<N> I = BitMatrix<N>::identityMatrix();
	ASSERT_TRUE(I.isIdentity());
	BitMatrix<N> O = BitMatrix<N>::zeroMatrix(); //square zero matrix
	ASSERT_FALSE(O.isIdentity());
}

TEST(BitMatrixTests, testRandomMatrix) {
	BitMatrix<N> m = BitMatrix<N>::randomMatrix(); //square random matrix

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

TEST(BitMatrixTests, testEqual){
	BitMatrix<N> I = BitMatrix<N>::identityMatrix();
	ASSERT_TRUE(I.equals(I));
	BitMatrix<N> J = BitMatrix<N>::identityMatrix();
	ASSERT_TRUE(I.equals(J));
	ASSERT_TRUE(J.equals(I));
	BitMatrix<N,N> R = BitMatrix<N,N>::randomInvertibleMatrix();
	ASSERT_TRUE(R.equals(R));
	BitMatrix<2*N,N> S = BitMatrix<2*N,N>::randomMatrix();
	ASSERT_TRUE(S.equals(S));
}

TEST(BitMatrixTests, testAssignment){
	BitMatrix<N> A = BitMatrix<N>::randomInvertibleMatrix();
    A.set(0,0);
    A.set(1,1);
    A.set(N-1,N-1);
	ASSERT_TRUE(A.get(0,0) && A.get(1,1) && A.get(N-1,N-1));
}

TEST(BitMatrixTests, testSolve) {
	ASSERT_TRUE( 1+5 == 6 );
	BitVector<N> v = BitVector<N>::randomVector();
	BitMatrix<N> M = BitMatrix<N>::randomInvertibleMatrix();
	BitVector<N> x = M.solve(v);
	BitVector<N> mx = M.template operator*<N>(x);
	ASSERT_TRUE(mx.equals(v));
}

TEST(BitMatrixTests, testInv) {
	ASSERT_TRUE(1+5 == 6);
	BitMatrix<N> M = BitMatrix<N>::randomInvertibleMatrix();
	BitMatrix<N> Mi = M.inv();
	BitMatrix<N> Il = M*Mi;
	BitMatrix<N> Ir = Mi*M;
	ASSERT_TRUE(Il.isIdentity());
	ASSERT_TRUE(Ir.isIdentity());
}

TEST(BitMatrixTests, testSplit) {
	BitMatrix<2*N> I = BitMatrix<2*N>::identityMatrix();
	BitMatrix<2*N,N> I1 = I.splitH2(0);
	BitMatrix<2*N,N> I2 = I.splitH2(1);
	BitMatrix<N> I11 = I1.splitV2(0);
	BitMatrix<N> I21 = I1.splitV2(1);
	BitMatrix<N> I12 = I2.splitV2(0);
	BitMatrix<N> I22 = I2.splitV2(1);
	ASSERT_TRUE(I11.isIdentity());
	ASSERT_FALSE(I12.isIdentity());
	ASSERT_FALSE(I21.isIdentity());
	ASSERT_TRUE(I22.isIdentity());

    //TODO: Need a more through test here.
	BitMatrix<3*N> J = BitMatrix<3*N>::identityMatrix();
	BitMatrix<3*N,N> J1 = J.splitH3(0);
	BitMatrix<N,N> J11 = J1.splitV3(0);
	ASSERT_TRUE(J11.isIdentity());
}

TEST(BitMatrixTest, testAug){
	BitMatrix<N> I = BitMatrix<N>::identityMatrix();
	BitMatrix<N> O = BitMatrix<N>::zeroMatrix();
	BitMatrix<N,2*N> IO = BitMatrix<N,2*N>::augH<N,N>(I, O);
	BitMatrix<N,2*N> OI = BitMatrix<N,2*N>::augH<N,N>(O, I);
	BitMatrix<2*N,2*N> IOOI = BitMatrix<2*N,2*N>::augV(IO, OI);
	ASSERT_TRUE(IOOI.isIdentity());
}

TEST(BitMatrixTests, testTranspose){
	BitMatrix<N> I = BitMatrix<N>::identityMatrix();
	ASSERT_TRUE(I.isIdentity());
	BitMatrix<N,N> It = I.transpose();
	ASSERT_TRUE(It.isIdentity());

	BitMatrix<N> S = BitMatrix<N>::randomMatrix(); //square random matrix
	BitMatrix<N,N> St = S.transpose();
	BitMatrix<N,N> Stt = St.transpose();
	ASSERT_TRUE(S.equals(Stt));
	ASSERT_TRUE(Stt.equals(S));

	BitMatrix<N,2*N> R = BitMatrix<N,2*N>::randomMatrix();
	BitMatrix<2*N,N> Rt = R.transpose();
	BitMatrix<N,2*N> Rtt = Rt.transpose();
	ASSERT_TRUE(R.equals(Rtt));
	ASSERT_TRUE(Rtt.equals(R));
}