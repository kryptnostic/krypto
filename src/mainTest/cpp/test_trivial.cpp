#include "../../../contrib/gtest/gtest.h"

using namespace testing;

TEST(OperatorTests, test_minus) {
	ASSERT_TRUE(2 + 0 == 2);
	ASSERT_TRUE(0 - -2 == 2);
	ASSERT_TRUE(2 - 2 == 0);
}
