#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/FullyHomomorphicEncryption.h"
#include <string>
using namespace testing;

#define N 2

TEST(FHETests, testEncryptionAndDecryption){
	ASSERT_TRUE(1 + 1 == 2);
	BitVector<N> m = BitVector<N>::randomVector();
	BitVector<N> r = BitVector<N>::randomVector();
	FullyHomomorphicEncryption<N> fhe;
	BitVector<2*N> c = fhe.encrypt(m, r);
	BitVector<N> mt = fhe.decrypt(c);
	ASSERT_TRUE(m.equals(mt));
	ASSERT_TRUE(mt.equals(m));
}


TEST(FHETests, testMMult){	
	ASSERT_TRUE(1 + 1 == 2);
	BitVector<N> m = BitVector<N>::randomVector();
	BitVector<N> r = BitVector<N>::randomVector();
	FullyHomomorphicEncryption<N> fhe;
	BitVector<2*N> c = fhe.encrypt(m, r);
	BitMatrix<N> K = BitMatrix<N>::randomMatrix(N<<6); //the matrix to be multiplied
	BitMatrix<N> R = BitMatrix<N>::randomInvertibleMatrix(N<<6);
	BitVector<2*N> H = fhe.MMult_Hom(K, c, R);
	BitVector<N> mt = fhe.decrypt(c);
	ASSERT_TRUE(m.equals(mt));	
	BitVector<N> rr = fhe.randUnary(R, c); 
	BitVector<2*N> Ht = fhe.encrypt(K*mt,rr);
	ASSERT_TRUE(H.equals(Ht));
	ASSERT_TRUE(Ht.equals(H));
}

TEST(FHETests, testXor){
	ASSERT_TRUE(1 + 1 == 2);
	BitVector<N> mx = BitVector<N>::randomVector();
	BitVector<N> my = BitVector<N>::randomVector();
	BitVector<N> rx = BitVector<N>::randomVector();
	BitVector<N> ry = BitVector<N>::randomVector();
	FullyHomomorphicEncryption<N> fhe;
	BitVector<2*N> cx = fhe.encrypt(mx, rx);
	BitVector<2*N> cy = fhe.encrypt(my, ry);
	BitMatrix<N> Rx = BitMatrix<N>::randomInvertibleMatrix(N<<6);
	BitMatrix<N> Ry = BitMatrix<N>::randomInvertibleMatrix(N<<6);
	BitVector<2*N> H = fhe.Xor_Hom(cx, cy, Rx, Ry);
	BitVector<N> mxt = fhe.decrypt(cx);
	BitVector<N> myt = fhe.decrypt(cy);
	ASSERT_TRUE(mx.equals(mxt));
	ASSERT_TRUE(my.equals(myt));
	BitVector<N> rr = fhe.randBinary(Rx, Ry, cx, cy); 
	BitVector<2*N> Ht = fhe.encrypt(mxt ^ myt, rr);
	ASSERT_TRUE(H.equals(Ht));
	ASSERT_TRUE(Ht.equals(H));
}

TEST(FHETests, testAnd){
	ASSERT_TRUE(1 + 1 == 2);
	BitVector<N> mx = BitVector<N>::randomVector();
	BitVector<N> my = BitVector<N>::randomVector();
	BitVector<N> rx = BitVector<N>::randomVector();
	BitVector<N> ry = BitVector<N>::randomVector();
	FullyHomomorphicEncryption<N> fhe;
	BitVector<2*N> cx = fhe.encrypt(mx, rx);
	BitVector<2*N> cy = fhe.encrypt(my, ry);
	BitMatrix<N> Rx = BitMatrix<N>::randomInvertibleMatrix(N<<6);
	BitMatrix<N> Ry = BitMatrix<N>::randomInvertibleMatrix(N<<6);
	BitVector<2*N> H = fhe.And_Hom(cx, cy, Rx, Ry);
	BitVector<N> mxt = fhe.decrypt(cx);
	BitVector<N> myt = fhe.decrypt(cy);
	ASSERT_TRUE(mx.equals(mxt));
	ASSERT_TRUE(my.equals(myt));
	BitVector<N> rr = fhe.randBinary(Rx, Ry, cx, cy); 
	BitVector<2*N> Ht = fhe.encrypt(mxt & myt, rr);
	ASSERT_TRUE(H.equals(Ht));
	ASSERT_TRUE(Ht.equals(H));	
}