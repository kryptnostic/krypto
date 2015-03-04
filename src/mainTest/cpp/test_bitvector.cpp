#include "../../../../contrib/gtest/gtest.h"
#include "../../main/cpp/BitVector.h"

using namespace testing;

TEST(BitVectorTests, test_random) {
	BitVector<1> bv = BitVector<1>::randomVector();
	ASSERT_TRUE(bv.length() == 64);
}

TEST(BitVectorTests, test_clear) {
	BitVector<1> bv = BitVector<1>::randomVector();
	bv.zero();
	for (int i = 0; i < bv.length(); i++) {
		std::cout << bv.get(i) << ", ";
		ASSERT_FALSE(bv.get(i));
	}
}

TEST(BitVectorTests, test_equals) {
	BitVector<1> bv = BitVector<1>::randomVector();
	BitVector<1> bv2 = BitVector<1>::randomVector();

	bv.print();
	bv2.print();

	ASSERT_TRUE(bv == bv);

	bv.zero();
	bv2.zero();

	ASSERT_TRUE(bv == bv2);

	BitVector<1> other;
	other.set(1);
	ASSERT_TRUE(!(bv == other));
}
