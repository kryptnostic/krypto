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
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(crypto_module) {
	class_< KryptnosticClient >("KryptnosticClient")
		.constructor<>()
		.constructor<>(std::string pk, std::string spk)
		.function("getPrivateKey", &KryptnosticClient::getPrivateKey)
		.function("getSearchPrivateKey", &KryptnosticClient::getSearchPrivateKey)
		.function("getClientHashFunction", &KryptnosticClient::getClientHashFunction)
		.function("getMetadatumAddress", &KryptnosticClient::getMetadatumAddress)
		.function("getObjectAddressFunction", &KryptnosticClient::getObjectAddressFunction)
		.function("getObjectIndexPair", &KryptnosticClient::getObjectIndexPair)
		.function("getEncryptedSearchToken", &KryptnosticClient::getEncryptedSearchToken)
		.function("getObjectSharingPair", &KryptnosticClient::getObjectSharingPair)
		;
}
#endif