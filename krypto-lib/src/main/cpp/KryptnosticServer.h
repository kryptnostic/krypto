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

template<unsigned int N = 128>
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
	KryptnosticServer(const ClientHashFunction<N> & cHashFunction, const BitVector<2*N> & eSearchToken) :
	_concealedF1(cHashFunction.concealedF1),
	_hashMatrixR(cHashFunction.hashMatrix.splitH2(1))
	{
		//set _tokenAddressFunction to partial eval of cHashFunction on eSearchToken
		_tokenAddressFunction = (cHashFunction.augmentedF2).template partialEval<N>(_concealedF1(eSearchToken));

		//add hashMatrix partial evaluation to consts of _tokenAddressFunction
		const BitVector<N> & hashMatrixPartialEval = cHashFunction.hashMatrix.splitH2(0) * eSearchToken;
		_tokenAddressFunction.xorConstants(hashMatrixPartialEval);
	}

/* Registration */

	/*
	 * Function: getMetadataAddress
	 * Returns a serialized pair of (ObjectSearchKey, ObjectAddressMatrix)
	 */
	const BitVector<N> getMetadataAddress(const std::pair <BitVector<2*N>, BitMatrix<N> > objectIndexPair) const{
		const BitVector<2*N> & eObjectSearchKey = objectIndexPair.first; //get from pair
		const BitMatrix<N> & objectConversionMatrix = objectIndexPair.second; //get from pair

		// return cHashFunction(eSearchToken, eObscuredObjectSearchKey);
		const BitVector<N> & fullEval = _tokenAddressFunction(_concealedF1(eObjectSearchKey)) ^ (_hashMatrixR * eObjectSearchKey);
		return objectConversionMatrix * fullEval;
	}


private:
	const BitMatrix<N, 2*N> _hashMatrixR;
	const MultiQuadTuple<2*N, N> _concealedF1;
	MultiQuadTuple<N, N> _tokenAddressFunction;
};

#endif