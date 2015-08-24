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

	}


/* Registration */


	/*
	 * Function: getMetadataAddress
	 * Returns a serialized pair of (ObjectSearchKey, ObjectAddressFunction)
	 */
	const BitVector<N> getMetadataAddress(const BitVector<2*N> eObjectSearchKey, const BitMatrix<N, N> objectConversionMatrix) const{
		// return cHashFunction(eSearchToken,eObjSearchKey);
		return objectConversionMatrix * _tokenAddressFunction(eObjectSearchKey);
	}


private:
	MultiQuadTuple<2*N, N> _tokenAddressFunction;
};

#endif