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
     * Constructs a KryptnosticServer given a
     * client's hash function and an FHE-encrypted search token
     * Calculates the client's hash function evaluated
     * on the search token without the encrypted ObjectSearchKey
     */
	KryptnosticServer(const ClientHashFunction<N> & cHashFunction, const BitVector<2*N> & eSearchToken) {
		//set _tokenAddressFunction to partial eval of cHashFunction on eSearchToken

		//store right half of HashMatrix as a private variable
		_concealedF1 = cHashFunction.concealedF1;
		BitVector<N> inner = _concealedF1(eSearchToken);

		BitVector<N> hashMatrixPartialEval = cHashFunction.hashMatrix.splitH2(0) * eSearchToken; //add to consts of _tokenAddressFunction
		//_tokenAddressFunction = (cHashFunction.augmentedF2).partialEval<N>(inner);
	}


/* Registration */


	/*
	 * Function: getMetadataAddress
	 * Returns a serialized pair of (ObjectSearchKey, ObjectAddressFunction)
	 */
	const BitVector<N> getMetadataAddress(const BitVector<2*N> eObjectSearchKey, const BitMatrix<N, N> objectConversionMatrix) const{
		// return cHashFunction(eSearchToken,eObjSearchKey);
		BitVector<N> inner = _concealedF1(eObjectSearchKey);
		return objectConversionMatrix * _tokenAddressFunction(inner);
	}


private:
	MultiQuadTuple<2*N, N> _concealedF1;
	MultiQuadTuple<N, N> _tokenAddressFunction;
};

#endif