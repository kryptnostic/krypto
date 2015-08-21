//
//  KryptnosticEngineServer.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 8/13/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ implementation of the client-side Kryptnostic Engine
//  Provides get functions for all of the cryptographic keys and functions
//  necessary for Kryptnostic search functionality
//

#ifndef krypto_KryptnosticEngineServer_h
#define krypto_KryptnosticEngineServer_h

#include "ClientHashFunction.h"
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

#define N 128

class KryptnosticEngineServer {

public:

/* Constructors */

	/*
     * Constructor
     * Constructs a Kryptnostic Engine given private and public keys
     */
	KryptnosticEngineServer(const ClientHashFunction cHashFunction, const BitVector<N> eDocSearchKey, const BitMatrix<N, N> docConversionMatrix) :
	_cHashFunction(cHashFunction),
	_eDocSearchKey(eDocSearchKey),
	_docConversionMatrix(docConversionMatrix)
	{

	}

/* Registration */

	
	/*
	 * Function: getMetadataAddress
	 * Returns a serialized pair of (DocumentSearchKey, DocumentAddressFunction)
	 */
	const BitVector<N> getMetadataAddress(const BitVector<N> eSearchToken) const{

		return 0;
	}


private:
	const ClientHashFunction _cHashFunction;
	const BitVector<N> _eDocSearchKey;
	const BitMatrix<N, N> _docConversionMatrix
};

#endif