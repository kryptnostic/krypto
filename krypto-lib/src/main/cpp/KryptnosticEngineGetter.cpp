//
//  KryptnosticEngine.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 8/13/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ implementation of the Kryptnostic Engine
//  Provides get functions for all of the cryptographic keys and functions
//  necessary for Kryptnostic search functionality
//

#ifndef krypto_KryptnosticEngineGetter_cpp
#define krypto_KryptnosticEngineGetter_cpp

#include "KryptnosticEngine.h"
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(crypto_module) {
	class_< KryptnosticEngine >("KryptnosticEngine")
		.constructor<>()
		//.constructor<>(PrivateKey<N> pk, SearchPrivateKey<N> spk, SearchPublicKey spubk)
		.function("getPrivateKey", &KryptnosticEngine::getPrivateKey)
		.function("getSearchPrivateKey", &KryptnosticEngine::getSearchPrivateKey)
		.function("getSearchPublicKey", &KryptnosticEngine::getSearchPublicKey)
		.function("getClientHashFunction", &KryptnosticEngine::getClientHashFunction)
		.function("getDocumentAddressFunction", &KryptnosticEngine::getDocumentAddressFunction)
		.function("getConversionMatrix", &KryptnosticEngine::getConversionMatrix)
		.function("getDocumentIndexPair", &KryptnosticEngine::getDocumentIndexPair)
		.function("getMetadataAddress", &KryptnosticEngine::getMetadataAddress)
		.function("getEncryptedSearchToken", &KryptnosticEngine::getEncryptedSearchToken)
		.function("getDocumentSharingPair", &KryptnosticEngine::getDocumentSharingPair)
		.function("getDocumentSearchKey", &KryptnosticEngine::getDocumentSearchKey)
		.function("setDocumentSearchKey", &KryptnosticEngine::setDocumentSearchKey)
		;
}
#endif

	// EMSCRIPTEN_BINDINGS(KryptnosticEngine) {
	// 	emscripten::value_object<unsigned char>("blob");
	// }