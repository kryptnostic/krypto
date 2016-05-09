#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/MonomialMatrix.h"
#include <string>
#include <chrono>

using namespace testing;

#define big 256
#define mid 128
#define sma 64
#define mon 7

TEST(ConstantChainHeaderTests, testPartialEval) {
	ConstantChainHeader<2*sma,sma> f = ConstantChainHeader<2*sma,sma>::random();

	BitVector<sma> x = BitVector<sma>::randomVector();
	BitVector<sma> y = BitVector<sma>::randomVector();
	BitVector<2*sma> z = BitVector<2*sma>::vCat(x, y);

	ConstantChainHeader<sma,sma> g = f.partialEval(x);

	ASSERT_TRUE(g(y) == f(z));
}

TEST(SecretPolynomialTests, testLeftCompose) {
	BitVector<sma> v = BitVector<sma>::randomVector();
	SecretPolynomial<sma,sma,mon> cs = SecretPolynomial<sma,sma,mon>::random();

	BitMatrix<sma> m = BitMatrix<sma>::randomMatrix();

	std::cout << "Starting left compose" << std::endl;

	auto start = std::chrono::high_resolution_clock::now();
	SecretPolynomial<sma,sma,mon> csm = cs * m;
	auto stop = std::chrono::high_resolution_clock::now();
	auto total = std::chrono::duration<double,nano>(stop - start).count();

	std::cout << "Total time: " << total << " ns" << std::endl;

	BitVector<sma> mv = m * v;
	BitVector<sma> csm_v = csm(v);
	BitVector<sma> cs_mv = cs(mv);

	ASSERT_TRUE(csm_v.equals(cs_mv));
}

TEST(SecretPolynomialTests, testLeftComposeDiffDims) {
	BitMatrix<sma,mid> m = BitMatrix<sma,mid>::randomMatrix();
	SecretPolynomial<sma,sma,mon> cs = SecretPolynomial<sma,sma,mon>::random();
	SecretPolynomial<mid,sma,mon> csm = cs * m;

	BitVector<mid> v = BitVector<mid>::randomVector();
	BitVector<sma> mv = m*v;

	BitVector<sma> csm_v = csm(v);
	BitVector<sma> cs_mv = cs(mv);

	ASSERT_TRUE(csm_v.equals(cs_mv));
}

TEST(SecretPolynomialTests, testRightCompose) {
	BitVector<sma> v = BitVector<sma>::randomVector();
	SecretPolynomial<sma,sma,mon> cs = SecretPolynomial<sma,sma,mon>::random();

	BitMatrix<sma> m = BitMatrix<sma>::generateSparseInvertibleMatrix(1);

	std::cout << "Starting right compose" << std::endl;

	auto start = std::chrono::high_resolution_clock::now();
	SecretPolynomial<sma,sma,mon> mcs = cs.permute(m);
	auto stop = std::chrono::high_resolution_clock::now();
	auto total = std::chrono::duration<double,nano>(stop - start).count();

	std::cout << "Total time: " << total << " ns" << std::endl;

	BitVector<sma> m_csv = m * cs(v);
	BitVector<sma> mcs_v = mcs(v);

	ASSERT_TRUE(m_csv.equals(mcs_v));
}

TEST(SecretPolynomialTests, testStackCols) {
	BitVector<sma> v = BitVector<sma>::randomVector();
	SecretPolynomial<sma,sma,mon> cs1 = SecretPolynomial<sma,sma,mon>::random();
	SecretPolynomial<sma,sma,mon> cs2 = SecretPolynomial<sma,sma,mon>::random();
	SecretPolynomial<sma,sma,mon> cs3 = SecretPolynomial<sma,sma,mon>::random();

	SecretPolynomial<sma,3*sma,mon> cs = SecretPolynomial<sma,3*sma,mon>::stackCols(cs1, cs2, c3);

	BitVector<3*sma> cs_v = cs(v);
	BitVector<3*sma> aug = BitVector<3*sma>::vCat(cs1(v), cs2(v), cs3(v));

	ASSERT_TRUE(cs_v.equals(aug));
}

TEST(SecretPolynomialTests, testStackRows) {
	BitVector<3*sma> v = BitVector<sma>::randomVector();
	SecretPolynomial<sma,sma,mon> cs1 = SecretPolynomial<sma,sma,mon>::random();
	SecretPolynomial<sma,sma,mon> cs2 = SecretPolynomial<sma,sma,mon>::random();
	SecretPolynomial<sma,sma,mon> cs3 = SecretPolynomial<sma,sma,mon>::random();

	SecretPolynomial<3*sma,sma,mon> cs = SecretPolynomial<3*sma,sma,mon>::stackRows(cs1, cs2, cs3);

	BitVector<sma> cs_v = cs(v);
	BitVector<sma> aug = cs1(v.proj3(0)) ^ cs2(v.proj3(1)) ^ cs3(v.proj3(2));

	ASSERT_TRUE(cs_v.equals(aug));
}