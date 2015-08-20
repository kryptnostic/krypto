#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/PrivateKey.h"
#include <string>
#include <ctime>
using namespace testing;

#define N 64

TEST(PrivKeyTests, testEncryptionAndDecryption){
	PrivateKey<N> pk;
	BitVector<N> m = BitVector<N>::randomVector();
	BitVector<2*N> c = pk.encrypt(m);
	BitVector<N> mt = pk.decrypt(c);
	ASSERT_TRUE(m.equals(mt));
}