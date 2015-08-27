//
//  KryptnosticClientGetter.h
//  krypto
//
//  Created by Robin Zhang on 8/19/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  Provides Embind bindings for all of the cryptographic keys and functions
//  necessary for client-side Kryptnostic search functionality
//

#ifndef krypto_KryptnosticClientGetter_cpp
#define krypto_KryptnosticClientGetter_cpp

#include "KryptnosticClient.h"
#include <string>
#include <iostream>
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(crypto_module) {
	class_< KryptnosticClient >("KryptnosticClient")
		.constructor<>()
		.constructor<std::string, std::string>()
		.function("getPrivateKey", &KryptnosticClient::getPrivateKey)
		.function("getSearchPrivateKey", &KryptnosticClient::getSearchPrivateKey)
		.function("getClientHashFunction", &KryptnosticClient::getClientHashFunction)
		.function("getObjectSearchKey", &KryptnosticClient::getObjectSearchKey)
		.function("getObjectAddressFunction", &KryptnosticClient::getObjectAddressFunction)
		.function("getObjectIndexPair", &KryptnosticClient::getObjectIndexPair)
		.function("getMetadatumAddress", &KryptnosticClient::getMetadatumAddress)
		.function("getEncryptedSearchToken", &KryptnosticClient::getEncryptedSearchToken)
		.function("getMetadatumAddressFromPair", &KryptnosticClient::getMetadatumAddressFromPair)
		.function("getObjectSharingPair", &KryptnosticClient::getObjectSharingPair)
		.function("getObjectUploadPair", &KryptnosticClient::getObjectUploadPair)
		;
}
#endif