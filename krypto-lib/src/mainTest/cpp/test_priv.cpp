#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/PrivateKey.h"
#include <string>
#include <ctime>
using namespace testing;

#define N 1

TEST(PrivKeyTests, testEncryptionAndDecryption){
	PrivateKey<1,2> pk;
	BitVector<1> m = BitVector<1>::randomVector();
	BitVector<2> c = pk.encrypt(m);
	BitVector<1> mt = pk.decrypt(c);
	ASSERT_TRUE(m.equals(mt));
}