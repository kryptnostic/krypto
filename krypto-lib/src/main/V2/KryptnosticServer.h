//
//  KryptnosticServer.h
//  krypto
//
//  Created by Robin Zhang on 8/21/15.
//	Modified by njhlai
//  Copyright (c) 2016 Kryptnostic. All rights reserved.
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
     * Constructor: (ClientHashFunction, FHE-encrypted search token)
     * Constructs a KryptnosticServer
     * Calculates the client's hash function partially evaluated
     * on the search token without the encrypted ObjectSearchKey
     */
	KryptnosticServer(const ClientHashFunction<N> & cHashFunction, const BitVector<2*N> & eSearchToken) :
	_hashMatrixR(cHashFunction.hashMatrix.splitH2(1)),
	_augmentedK(cHashFunction.augmentedK),
	_concealedMatrix(cHashFunction.concealedMatrix.splitV2(1).splitH2(1))
	{
		const BitVector<2*N> & hashMatrixPartialEval = _augmentedK.rightInverse() * (cHashFunction.hashMatrix.splitH2(0) * eSearchToken);
		
		_tokenAddressFunction = cHashFunction.augmentedF.partialEval(cHashFunction.concealedMatrix.splitV2(0).splitH2(0) * eSearchToken);
		_tokenAddressFunction.xorConstant(hashMatrixPartialEval);
	}

/* Registration */

	/*
	 * Function: getMetadataAddress(objectSearchPair)
	 * Server-side address computation on encrypted ObjectSearchKey and ObjectConversionMatrix
	 * Returns the address of metadatum corresponding to a token-object pair
	 */
	const BitVector<N> getMetadataAddress(const std::pair <BitVector<2*N>, BitMatrix<N> > &objectSearchPair) const{
		const BitVector<2*N> & eObjectSearchKey = objectSearchPair.first; //get from pair
		const BitMatrix<N> & objectConversionMatrix = objectSearchPair.second; //get from pair

		// return cHashFunction(eSearchToken, eObscuredObjectSearchKey);
		const BitVector<N> & fullEval = _augmentedK * _tokenAddressFunction(_concealedMatrix * eObjectSearchKey) ^ (_hashMatrixR * eObjectSearchKey);
		return objectConversionMatrix * fullEval;
	}

	~KryptnosticServer() {
		// freeAllNext must be called to properly delete ConstantHeaderChain
		_tokenAddressFunction.getMMC().freeAllNext();
	}

private:
	const BitMatrix<N,2*N> _hashMatrixR;
	const BitMatrix<N,2*N> _augmentedK;
	const BitMatrix<N,2*N> _concealedMatrix;
	ConstantChainHeader<N,2*N> _tokenAddressFunction;
};

#endif