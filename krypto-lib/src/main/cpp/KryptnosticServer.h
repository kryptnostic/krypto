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
	KryptnosticServer(const ClientHashFunction cHashFunction, const BitVector<2*N> eSearchToken) {

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
	MultiQuadTuple<2*N, N> _tokenAddressFunction;
};

#endif