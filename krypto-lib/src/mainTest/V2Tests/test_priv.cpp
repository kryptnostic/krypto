#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/PrivateKey.h"
#include <string>
#include <chrono>

using namespace testing;

#define N 64
#define MON 7

TEST(PrivKeyTests, testEncryptionAndDecryption){
	PrivateKey<N,MON> pk;
	BitVector<N> m = BitVector<N>::randomVector();

	auto start = std::chrono::high_resolution_clock::now();
	BitVector<2*N> c = pk.encrypt(m);
	auto stop = std::chrono::high_resolution_clock::now();
	auto total = std::chrono::duration<double,nano>(stop - start).count();

	std::cout << "Total time for encryption: " << total <<  " ns" << std::endl;

	start = std::chrono::high_resolution_clock::now();
	BitVector<N> mt = pk.decrypt(c);
	stop = std::chrono::high_resolution_clock::now();
	total = std::chrono::duration<double,nano>(stop-start).count();

	std::cout << "Total time for decryption: " << total << " ns" << std::endl;

	ASSERT_TRUE(m.equals(mt));
}