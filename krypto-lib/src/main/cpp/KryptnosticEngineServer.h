//
//  KryptnosticEngineServer.h
//  krypto
//
//  Created by Robin Zhang on 8/21/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ implementation of the server-side Kryptnostic Engine
//  Performs the metadata address computation
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
	KryptnosticEngineServer(const ClientHashFunction cHashFunction, const BitVector<2*N> eDocSearchKey, const BitMatrix<N, N> docConversionMatrix) :
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
	const BitVector<N> getMetadataAddress(const BitVector<2*N> eSearchToken) const{
		BitVector<N> hashMatrixOutput = _cHashFunction.hashMatrix * BitVector<4*N>::vCat(eSearchToken, _eDocSearchKey);
		BitVector<2*N> augmentedOutputF1 = BitVector<2*N>::vCat(_cHashFunction.concealedF1(eSearchToken), _cHashFunction.concealedF1(_eDocSearchKey));
		BitVector<N> functionalOutput = _cHashFunction.augmentedF2(augmentedOutputF1);
		return hashMatrixOutput ^ functionalOutput;
	}


private:
	const ClientHashFunction _cHashFunction;
	const BitVector<N> _eDocSearchKey;
	const BitMatrix<N, N> _docConversionMatrix
};

#endif