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
#include <unordered_map>
#include <unordered_set>

//N should be a multiple of 64, otherwise BitVector would create
//the incorrect number of unsigned long longs
template<unsigned int N>
class SearchPrivateKey{
public:
	SearchPrivateKey() :
	_K(BitMatrix<N>::randomInvertibleMatrixDoubleH()),
	_C(BitMatrix<N>::randomInvertibleMatrix())
	{ }

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
	const BitMatrix<N, 2*N> getObjectAddressFunction(const UUID & objectId) const{
		return randomInvertibleMatrixDoubleH();
	}

	/*
	 * Function: getDocConversionMatrix
	 * Returns document conversion matrix given document address function and user-specific K
	 */
	const BitMatrix<N> getDocConversionMatrix(const BitMatrix<N, 2*N> & docAddressFunction,
		const BitMatrix<N, 2*N> & userK) const{
		BitMatrix<2*N, N> K_ginv = userK.transpose(); //TODO: implement generalized inverse
		return docAddressFunction * K_ginv;
	}

	/*
	 * Function: getAddress
	 * Given a token and a object key, returns the address for the
	 * associated metadatum
	 */
	const BitVector<N> getAddress(const BitMatrix<N, 2*N> &addressMatrix, const BitVector<N> &token, const BitVector<N> & objectSearchKey) const{
		return addressMatrix * (BitVector<2*N>::vCat(token, objectSearchKey));
	}

private:
	BitMatrix<N, 2*N> _K; //user-specific K_\Omega
	BitMatrix<N> _C; //to conceal individual function piece

	/*
     * Function: randomInvertibleMatrixDoubleH()
     * by concatenating N x 2N invertible matrices
     */
	const BitMatrix<N, 2*N> randomInvertibleMatrixDoubleH() const{
		return BitMatrix<N, 2*N>::augH(
			BitMatrix<N>::randomInvertibleMatrix(), BitMatrix<N>::randomInvertibleMatrix());
	}

/* Components of Hash */

	/*
     * Function: generateHashMatrix()
     * Returns the matrix portion of the hash function
     * Applied to x concatenated with y
     */
	const BitMatrix<N, 4*N> generateHashMatrix(const PrivateKey<N> & pk) const{
		BitMatrix<2*N> Mi = pk.getM().inv();
		BitMatrix<N, 2*N> Mi1 = Mi.splitV(0);
		BitMatrix<N, 2*N> Mi2 = Mi.splitV(1);

		BitMatrix<N, 2*N> left = pk.getB().inv() * Mi1;
		BitMatrix<N, 2*N> right = pk.getA().inv() * Mi2;
		BitMatrix<N, 2*N> decryptMatrix = left ^ right;

		BitMatrix<N, 2*N> zero = BitMatrix<N, 2*N>::zeroMatrix();
		BitMatrix<N, 4*N> top = BitMatrix<N, 4*N>::augH(decryptMatrix, zero);
		BitMatrix<N, 4*N> bot = BitMatrix<N, 4*N>::augH(zero, decryptMatrix);
		return _K * (BitMatrix<2*N, 4*N>::augV(top, bot));
	}

	/*
     * Function: generateAugmentedF2()
     * Returns the K (f2 C || f2 C) portion of the hash function
     * Applied to concealedF1(x) concatenated with concealedF1(y)
     */
	const MultiQuadTuple<N, 2*N> generateAugmentedF2(const PrivateKey<N> & pk) const{
		MultiQuadTuple<N, N> f2 = pk.getf().get(1);
		MultiQuadTuple<N, N> topBot = (f2 * _C);
		MultiQuadTuple<N, 2*N> augmentedDecrypt = MultiQuadTuple<N, 2*N>::augV(topBot, topBot);
		return augmentedDecrypt.rMult(_K);
	}

	/*
     * Function: generateConcealedF1()
     * Returns the f1 C portion of the hash function
     * Applied to x and y separately
     */
	const MultiQuadTuple<N, 2*N> generateConcealedF1(const PrivateKey<N> & pk) const{
		MultiQuadTuple<N, N> f1 = pk.getf().get(0);
		BitMatrix<N, 2*N> Mi2 = pk.getM().inv().splitV(1);
		BitMatrix<N, 2*N> inner = pk.getA().inv() * Mi2;
		return (f1 * inner).rMult(_C.inv());
	}
};

#endif/* defined(__krypto__SearchPrivateKey__) */