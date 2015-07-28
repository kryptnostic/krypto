#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/FullyHomomorphicEncryption.h"
#include <string>
#include <ctime>
using namespace testing;

#define N 1
#define C 5 //runtime average over C times

TEST(FHETests, testEncryptionAndDecryption){
	ASSERT_TRUE(1 + 1 == 2);
	FullyHomomorphicEncryption<N> fhe;
	BitVector<N> m = BitVector<N>::randomVector();
	BitVector<N> r = BitVector<N>::randomVector();
	BitVector<2*N> c = fhe.encrypt(m, r);
	BitVector<N> mt = fhe.decrypt(c);
	ASSERT_TRUE(m.equals(mt));
	ASSERT_TRUE(mt.equals(m));
}

TEST(FHETests, testMultipleEncryptionAndDecryptions){
	ASSERT_TRUE(1 + 1 == 2);
	double total_time_E = 0, total_time_D = 0;
	for(int i = 0; i < C; ++i){
		FullyHomomorphicEncryption<N> fhe;
		BitVector<N> m = BitVector<N>::randomVector();
		BitVector<N> r = BitVector<N>::randomVector();
		clock_t begin_E = clock();
		BitVector<2*N> c = fhe.encrypt(m, r);
		clock_t end_E = clock();
		clock_t begin_D = clock();
		BitVector<N> mt = fhe.decrypt(c);
		clock_t end_D = clock();
		ASSERT_TRUE(m.equals(mt));
		ASSERT_TRUE(mt.equals(m));
		total_time_E += (end_E - begin_E)/static_cast<double>(CLOCKS_PER_SEC);
		total_time_D += (end_D - begin_D)/static_cast<double>(CLOCKS_PER_SEC);
	}
	double average_time_E = total_time_E/static_cast<double>(C);
	double average_time_D = total_time_D/static_cast<double>(C);
	std::cout << "Encryption [average]: " << average_time_E << std::endl;
    std::cout << "Decryption [average]: " << average_time_D << std::endl;	
}


TEST(FHETests, testMMult){	
	ASSERT_TRUE(1 + 1 == 2);
	BitVector<N> m = BitVector<N>::randomVector();
	BitVector<N> r = BitVector<N>::randomVector();
	FullyHomomorphicEncryption<N> fhe;
	BitVector<2*N> c = fhe.encrypt(m, r);
	BitMatrix<N> K = BitMatrix<N>::randomMatrix(N<<6); //the matrix to be multiplied
	BitMatrix<N> R = BitMatrix<N>::randomInvertibleMatrix();
	BitVector<2*N> H = fhe.MMult_Hom(K, c, R);
	BitVector<N> mt = fhe.decrypt(c);
	ASSERT_TRUE(m.equals(mt));	
	BitVector<N> rr = fhe.randUnary(R, c); 
	BitVector<2*N> Ht = fhe.encrypt(K*mt,rr);
	ASSERT_TRUE(H.equals(Ht));
	ASSERT_TRUE(Ht.equals(H));
}

TEST(FHETests, testMultipleMMults){
	ASSERT_TRUE(1 + 1 == 2);
	double total_time = 0;
	for(int i = 0; i < C; ++i){
		BitVector<N> m = BitVector<N>::randomVector();
		BitVector<N> r = BitVector<N>::randomVector();
		FullyHomomorphicEncryption<N> fhe;
		BitVector<2*N> c = fhe.encrypt(m, r);
		BitMatrix<N> K = BitMatrix<N>::randomMatrix(N << 6); //the matrix to be multiplied
		BitMatrix<N> R = BitMatrix<N>::randomInvertibleMatrix();
		clock_t begin = clock();
		BitVector<2*N> H = fhe.MMult_Hom(K, c, R);
		clock_t end = clock();
		BitVector<N> mt = fhe.decrypt(c);
		ASSERT_TRUE(m.equals(mt));	
		BitVector<N> rr = fhe.randUnary(R, c); 
		BitVector<2*N> Ht = fhe.encrypt(K*mt,rr);
		ASSERT_TRUE(H.equals(Ht));
		ASSERT_TRUE(Ht.equals(H));
		total_time += (end - begin)/static_cast<double>(CLOCKS_PER_SEC);
	}
	double average_time = total_time/static_cast<double>(C);
	std::cout << "MMult_Hom [average]: " << average_time << endl;
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
	BitMatrix<N> Rx = BitMatrix<N>::randomInvertibleMatrix();
	BitMatrix<N> Ry = BitMatrix<N>::randomInvertibleMatrix();
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

TEST(FHETests, testMultipleXors){
	ASSERT_TRUE(1 + 1 == 2);
	double total_time = 0;
	for(int i = 0; i < C; ++i){
		BitVector<N> mx = BitVector<N>::randomVector();
		BitVector<N> my = BitVector<N>::randomVector();
		BitVector<N> rx = BitVector<N>::randomVector();
		BitVector<N> ry = BitVector<N>::randomVector();
		FullyHomomorphicEncryption<N> fhe;
		BitVector<2*N> cx = fhe.encrypt(mx, rx);
		BitVector<2*N> cy = fhe.encrypt(my, ry);
		BitMatrix<N> Rx = BitMatrix<N>::randomInvertibleMatrix();
		BitMatrix<N> Ry = BitMatrix<N>::randomInvertibleMatrix();
		clock_t begin = clock();
		BitVector<2*N> H = fhe.Xor_Hom(cx, cy, Rx, Ry);
		clock_t end = clock();
		BitVector<N> mxt = fhe.decrypt(cx);
		BitVector<N> myt = fhe.decrypt(cy);
		ASSERT_TRUE(mx.equals(mxt));
		ASSERT_TRUE(my.equals(myt));
		BitVector<N> rr = fhe.randBinary(Rx, Ry, cx, cy); 
		BitVector<2*N> Ht = fhe.encrypt(mxt ^ myt, rr);
		ASSERT_TRUE(H.equals(Ht));
		ASSERT_TRUE(Ht.equals(H));
		total_time += (end - begin)/static_cast<double>(CLOCKS_PER_SEC);
	}
	double average_time = total_time/static_cast<double>(C);
	std::cout << "Xor_Hom [average]: " << average_time << std::endl;
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
	BitMatrix<N> Rx = BitMatrix<N>::randomInvertibleMatrix();
	BitMatrix<N> Ry = BitMatrix<N>::randomInvertibleMatrix();
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

TEST(FHETests, testMultipleAnds){
	ASSERT_TRUE(1 + 1 == 2);
	double total_time = 0;
	for(int i = 0; i < C; ++i){
		BitVector<N> mx = BitVector<N>::randomVector();
		BitVector<N> my = BitVector<N>::randomVector();
		BitVector<N> rx = BitVector<N>::randomVector();
		BitVector<N> ry = BitVector<N>::randomVector();
		FullyHomomorphicEncryption<N> fhe;
		BitVector<2*N> cx = fhe.encrypt(mx, rx);
		BitVector<2*N> cy = fhe.encrypt(my, ry);
		BitMatrix<N> Rx = BitMatrix<N>::randomInvertibleMatrix();
		BitMatrix<N> Ry = BitMatrix<N>::randomInvertibleMatrix();
		clock_t begin = clock();
		BitVector<2*N> H = fhe.And_Hom(cx, cy, Rx, Ry);
		clock_t end = clock();
		BitVector<N> mxt = fhe.decrypt(cx);
		BitVector<N> myt = fhe.decrypt(cy);
		ASSERT_TRUE(mx.equals(mxt));
		ASSERT_TRUE(my.equals(myt));
		BitVector<N> rr = fhe.randBinary(Rx, Ry, cx, cy); 
		BitVector<2*N> Ht = fhe.encrypt(mxt & myt, rr);
		ASSERT_TRUE(H.equals(Ht));
		ASSERT_TRUE(Ht.equals(H));
		total_time += (end - begin)/static_cast<double>(CLOCKS_PER_SEC);
	}
	double average_time = total_time/static_cast<double>(C);
	std::cout << "And_Hom [average]: " << average_time << endl;	
}