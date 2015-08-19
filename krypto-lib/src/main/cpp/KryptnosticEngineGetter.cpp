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
		.function("getPrivateKey", &KryptnosticEngine::getPrivateKey)
		.function("getPublicKey", &KryptnosticEngine::getPublicKey)
		.function("getServerSearchFunction", &KryptnosticEngine::getServerSearchFunction)
		.function("getXor", &KryptnosticEngine::getXor)
		.function("getAnd", &KryptnosticEngine::getAnd)
		.function("getLeftShift", &KryptnosticEngine::getLeftShift)
		.function("getDocKey", &KryptnosticEngine::getDocKey)
		.function("setDocKey", &KryptnosticEngine::setDocKey)
		.function("getHashedToken", &KryptnosticEngine::getHashedToken)
		.function("getEncryptedSearchTerm", &KryptnosticEngine::getEncryptedSearchTerm)
		;
}
#endif

	// EMSCRIPTEN_BINDINGS(KryptnosticEngine) {
	// 	emscripten::value_object<unsigned char>("blob");
	// }