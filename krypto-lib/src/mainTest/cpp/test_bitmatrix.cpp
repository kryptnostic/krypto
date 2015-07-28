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

TEST(BitMatrixTest, testIdentity){
	BitMatrix<N> I = BitMatrix<N>::squareIdentityMatrix();
	ASSERT_TRUE(I.isIdentity());
	BitMatrix<N> O = BitMatrix<N>::squareZeroMatrix();
	ASSERT_FALSE(O.isIdentity());
}

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
	BitMatrix<N> A = BitMatrix<N>::randomInvertibleMatrix();
	A.set(0,0), A.set(1,1), A.set((N<<6)-1,(N<<6)-1);
	ASSERT_TRUE(A.get(0,0) && A.get(1,1) && A.get((N<<6)-1,(N<<6)-1));
}

/*
TEST(BitMatrixTests, testEquality){
	BitMatrix<N> S = BitMatrix<N>::randomInvertibleMatrix();
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
	BitMatrix<N> M = BitMatrix<N>::randomInvertibleMatrix();
	BitVector<N> x = M.solve(v);
	BitVector<N> mx = M*x;
	ASSERT_TRUE(mx.equals(v));

	BitMatrix<N> R = BitMatrix<N>::randomMatrix(N << 6);
	//BitMatrix<N> Q = M.solveM(R);
}

TEST(BitMatrixTests, testInv) {
	ASSERT_TRUE(1+5 == 6);
	BitMatrix<N> M = BitMatrix<N>::randomInvertibleMatrix();
	BitMatrix<N> Mi = M.inv();
	BitMatrix<N> Il = M*Mi;
	BitMatrix<N> Ir = Mi*M;
	ASSERT_TRUE(Il.isIdentity());
	ASSERT_TRUE(Ir.isIdentity());
	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<N> mix = Mi*x;
	ASSERT_TRUE(mix.equals(M.solve(x)));

	BitMatrix<N> Q = BitMatrix<N>::randomInvertibleMatrix();
	BitMatrix<N> Qi = Q.inv();
	BitMatrix<N> Jl = Q*Qi;
	BitMatrix<N> Jr = Qi*Q;
	ASSERT_TRUE(Jl.isIdentity());
	ASSERT_TRUE(Jr.isIdentity());

/*
	BitMatrix<N> X = BitMatrix<N>::randomInvertibleMatrix();
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
	BitMatrix<N> I1 = I.split_h_2(0); 
	BitMatrix<N> I2 = I.split_h_2(1);
	BitMatrix<N> I11 = I1.split_v_2(0);
	BitMatrix<N> I12 = I1.split_v_2(1);
	BitMatrix<N> I21 = I2.split_v_2(0);
	BitMatrix<N> I22 = I2.split_v_2(1);
	ASSERT_TRUE(I11.isIdentity());
	ASSERT_FALSE(I12.isIdentity());
	ASSERT_FALSE(I21.isIdentity());
	ASSERT_TRUE(I22.isIdentity());

	BitMatrix<3*N> J = BitMatrix<3*N>::squareIdentityMatrix();
	BitMatrix<N> J1 = J.split_h_3(0);
	BitMatrix<N> J11 = J1.split_v_3(0);
	ASSERT_TRUE(J11.isIdentity());
}

TEST(BitMatrixTest, testAug){
	BitMatrix<N> I = BitMatrix<N>::squareIdentityMatrix();
	BitMatrix<N> O = BitMatrix<N>::squareZeroMatrix();
	BitMatrix<2*N> IO = BitMatrix<2*N>::aug_h(I, O);
	BitMatrix<2*N> OI = BitMatrix<2*N>::aug_h(O, I);
	BitMatrix<2*N> IOOI = BitMatrix<2*N>::aug_v(IO, OI);
	ASSERT_TRUE(IOOI.isIdentity());
}

TEST(BitMatrixTests, testTranspose){
	BitMatrix<N> I = BitMatrix<N>::squareIdentityMatrix();
	ASSERT_TRUE(I.isIdentity());
	BitMatrix<N> It = I.T<N>();
	ASSERT_TRUE(It.isIdentity());

	BitMatrix<N> S = BitMatrix<N>::squareRandomMatrix();
	BitMatrix<N> St = S.T<N>();
	BitMatrix<N> Stt = St.T<N>(); 
	//ASSERT_TRUE(S.equals(Stt));
	//ASSERT_TRUE(Stt.equals(S));

	BitMatrix<N> R = BitMatrix<N>::randomMatrix(N << 7);
	BitMatrix<(N<<1)> Rt = R.T<(N<<1)>();
	BitMatrix<N> Rtt = Rt.T<N>();
	//ASSERT_TRUE(R.equals(Rtt));
	//ASSERT_TRUE(Rtt.equals(R));
}