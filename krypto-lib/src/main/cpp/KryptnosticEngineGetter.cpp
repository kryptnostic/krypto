//
//  KryptnosticEngineClient.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 8/19/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ implementation of the Kryptnostic Engine
//  Provides get functions for all of the cryptographic keys and functions
//  necessary for Kryptnostic search functionality
//

#ifndef krypto_KryptnosticEngineClientGetter_cpp
#define krypto_KryptnosticEngineClientGetter_cpp

#include "KryptnosticEngineClient.h"
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(crypto_module) {
	class_< KryptnosticEngineClient >("KryptnosticEngineClient")
		.constructor<>()
		//.constructor<>(PrivateKey<N> pk, SearchPrivateKey<N> spk, SearchPublicKey spubk)
		.function("getPrivateKey", &KryptnosticEngineClient::getPrivateKey)
		.function("getSearchPrivateKey", &KryptnosticEngineClient::getSearchPrivateKey)
		.function("getClientHashFunction", &KryptnosticEngineClient::getClientHashFunction)
		.function("getDocumentAddressFunction", &KryptnosticEngineClient::getDocumentAddressFunction)
		.function("getDocumentConversionMatrix", &KryptnosticEngineClient::getDocumentConversionMatrix)
		.function("getDocumentIndexPair", &KryptnosticEngineClient::getDocumentIndexPair)
		.function("getEncryptedSearchToken", &KryptnosticEngineClient::getEncryptedSearchToken)
		.function("getDocumentSharingPair", &KryptnosticEngineClient::getDocumentSharingPair)
		.function("getDocumentSearchKey", &KryptnosticEngineClient::getDocumentSearchKey)
		.function("setDocumentSearchKey", &KryptnosticEngineClient::setDocumentSearchKey)
		;
}
#endif