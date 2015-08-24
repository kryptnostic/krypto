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
		//.constructor<>(PrivateKey<N> pk, SearchPrivateKey<N> spk, SearchPublicKey spubk)
		.function("getPrivateKey", &KryptnosticClient::getPrivateKey)
		.function("getSearchPrivateKey", &KryptnosticClient::getSearchPrivateKey)
		.function("getClientHashFunction", &KryptnosticClient::getClientHashFunction)
		.function("getDocumentAddressFunction", &KryptnosticClient::getDocumentAddressFunction)
		.function("getDocumentConversionMatrix", &KryptnosticClient::getDocumentConversionMatrix)
		.function("getDocumentIndexPair", &KryptnosticClient::getDocumentIndexPair)
		.function("getEncryptedSearchToken", &KryptnosticClient::getEncryptedSearchToken)
		.function("getDocumentSharingPair", &KryptnosticClient::getDocumentSharingPair)
		.function("getDocumentSearchKey", &KryptnosticClient::getDocumentSearchKey)
		.function("setDocumentSearchKey", &KryptnosticClient::setDocumentSearchKey)
		;
}
#endif