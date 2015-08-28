/*
 * test_client.cpp
 * mock test of javascript integration done on C++
 *
 *  Created on: August 27, 2015
 *      Author: howpenghui
 */

#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/KryptnosticClient.h"

using namespace testing;

TEST(KryptnosticClientTest, testRegistration){
	KryptnosticClient kc;
	//TODO: index a document
}

TEST(KryptnosticClientTest, testLogin){
	KryptnosticClient kc_reg;
	//retrieve private key
	//retrieve search private key
	//make a constructor of KC out of the retrieved keys
}