/*
 * test_server.cpp
 *
 *  Created on: August 27, 2015
 *      Author: howpenghui
 */

#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/SearchPrivateKey.h"
#include "../../main/cpp/KryptnosticServer.h"

using namespace testing;

TEST(KryptnosticServerTest, testInit){
	SearchPrivateKey<> spk;
	PrivateKey<> pk;
	ClientHashFunction<> chf = spk.getClientHashFunction(pk);
	BitVector<> token = BitVector<>::randomVector();
	KryptnosticServer<> ks(chf, pk.encrypt(token));
}