//
//  SearchPrivateKey.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 8/13/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ implementation of the SearchPrivateKey
//  which generates all of the necessary client-side parts
//  for indexing and searching objects
//

#ifndef krypto_SearchPrivateKey_h
#define krypto_SearchPrivateKey_h

#include "PrivateKey.h"
#include "UUID.h"
#include "ClientHashFunction.h"

//N should be a multiple of 64, otherwise BitVector would create
//the incorrect number of unsigned long longs
template<unsigned int N>
class SearchPrivateKey{
public:
	SearchPrivateKey() :
	_K(BitMatrix<N>::randomInvertibleMatrixDoubleH())
	{}

/* Getters */

	/*
     * Function: getK()
     * Returns the client-specific n x 2n matrix K_\Omega
     */
	const BitMatrix<N, 2*N> getK() const{
		return _K;
	}

	/*
	 * Function: getObjectSearchKey
	 * Returns a random object search key to be serialized
	 * (checking if this is unused is done by JavaScript frontend)
	 */
	const BitVector<N> getObjectSearchKey() const{
		return BitVector<N>::randomVector();
	}

	/*
	 * Function: getObjectAddressFunction
	 * Returns a random object address function L to be serialized
	 */
	const BitMatrix<N, 2*N> getObjectAddressFunction() const{
		return randomInvertibleMatrixDoubleH();
	}

	/*
	 * Function: getDocConversionMatrix
	 * Returns document conversion matrix given document address function and user-specific K
	 */
	const BitMatrix<N> getDocConversionMatrix(const BitMatrix<N, 2*N> & docAddressFunction,
		const BitMatrix<N, 2*N> & K) const{
		return docAddressFunction * K.pseudoinverse();
	}

	/*
	 * Function: getAddress
	 * Given a token and a object key, returns the address for the
	 * associated metadatum
	 */
	const BitVector<N> getAddress(const BitMatrix<N, 2*N> &addressMatrix, const BitVector<N> &token, const BitVector<N> & objectSearchKey) const{
		return addressMatrix * (BitVector<2*N>::vCat(token, objectSearchKey));
	}

	const ClientHashFunction<N> getClientHashFunction(const PrivateKey<N> & pk){
		ClientHashFunction<N> h;
		h.initialize(_K, pk);
		return h;
	}

private:
	BitMatrix<N, 2*N> _K; //user-specific K_\Omega

	/*
     * Function: randomInvertibleMatrixDoubleH()
     * by concatenating N x 2N invertible matrices
     */
	const BitMatrix<N, 2*N> randomInvertibleMatrixDoubleH() const{
		return BitMatrix<N, 2*N>::augH(
			BitMatrix<N>::randomInvertibleMatrix(), BitMatrix<N>::randomInvertibleMatrix());
	}
};

#endif/* defined(__krypto__SearchPrivateKey__) */