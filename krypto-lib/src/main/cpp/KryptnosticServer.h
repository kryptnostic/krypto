//
//  KryptnosticServer.h
//  krypto
//
//  Created by Robin Zhang on 8/21/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ implementation of the server-side Kryptnostic
//  Performs the metadata address computation
//

#ifndef krypto_KryptnosticServer_h
#define krypto_KryptnosticServer_h

#include "ClientHashFunction.h"
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

#define N 128

class KryptnosticServer {

public:

/* Constructors */

	/*
     * Constructor
     * Constructs a Kryptnostic  given private and public keys
     */
	KryptnosticServer(const ClientHashFunction cHashFunction, const BitVector<2*N> eDocSearchKey, const BitMatrix<N, N> docConversionMatrix) :
	_cHashFunction(cHashFunction),
	_eDocSearchKey(eDocSearchKey),
	_docConversionMatrix(docConversionMatrix)
	{}

/* Registration */


	/*
	 * Function: getMetadataAddress
	 * Returns a serialized pair of (DocumentSearchKey, DocumentAddressFunction)
	 */
	const BitVector<N> getMetadataAddress(const BitVector<2*N> & eSearchToken) const{
		return _cHashFunction(eSearchToken, _eDocSearchKey);
	}


private:
	const ClientHashFunction<N> _cHashFunction;
	const BitVector<2*N> _eDocSearchKey;
	const BitMatrix<N, N> _docConversionMatrix;
};

#endif