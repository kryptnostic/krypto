//
//  SearchPrivateKey.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 8/13/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ implementation of the SearchPrivateKey
//  which generates all of the necessary client-side parts
//  for indexing and searching documents
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
	SearchPrivateKey(const PrivateKey<N> & pk) :
	_pk(pk),
	_K(generateK()),
	_C(BitMatrix<N>::randomInvertibleMatrix())
	{ }

/* Getters */

	/*
     * Function: generateK()
     * Returns the random client-specific n x 2n matrix K_\Omega
     */
	const BitMatrix<N, 2*N> getK() const{
		return _K;
	}

	/*
	 * Function: getDocKey
	 * Returns a serialized random unused document key
	 * and inserts the document key into a stored hash set
	 * Returns existing key if object has an existing key
	 */
	const BitVector<N> getDocKey( const UUID & objectId) {
		BitVector<N> docKey = generateDocKey(objectId);
		if (!docKey.isZero()) { //objectId already used
			while (docKeySet.count(docKey) == 1) docKey = generateDocKey(objectId); //generated new key
			docToKeyMap[objectId] = docKey;
			docKeySet.insert(docKey);
		} else docKey = docToKeyMap[objectId];
		return docKey;
	}

	/*
	 * Function: setDocKey
	 * Sets the document key of a given object to a given document key
	 * Returns whether the operation was valid and successful
	 */
	const bool setDocKey(const UUID & objectId, const BitVector<N> & docKey) {
		if (docToKeyMap.count(objectId) != 0) {
			docToKeyMap[objectId] = docKey;
			return true;
		}
		return false;
	}

	/*
	 * Function: getDocAddressFunction
	 * Returns a serialized random unused document address function L
	 * and inserts the document address function into a stored hash set
	 * Returns existing address function if object has an existing key
	 */
	const BitMatrix<N, 2*N> getDocAddressFunction(const UUID & objectId) {
		BitMatrix<N, 2*N> addressMatrix;
		if (docKeySet.count(objectId) == 0) { //objectId already used
			addressMatrix = generateK();
			while (addressMatrix.docAddressFunctionSet.count(addressMatrix) == 1) addressMatrix = generateK(); //generated new matrix
			docToAddressFunctionMap[objectId] = addressMatrix;
			docAddressFunctionSet.insert(addressMatrix);
		} else addressMatrix = docToAddressFunctionMap[objectId];
		return addressMatrix;
	}

	/*
	 * Function: getAddress
	 * Given a token and a document key, returns the address for the
	 * associated metadatum
	 */
	const BitVector<N> getAddress(const BitVector<N> & token, const UUID & objectId) const{
		BitVector<N> docKey = docToKeyMap[objectId];
		BitMatrix<N, 2*N> addressMatrix = docToAddressFunctionMap[objectId];

		return addressMatrix * (BitVector<2*N>::vCat(token, objectId));
	}

private:
	PrivateKey<N> _pk;
	BitMatrix<N, 2*N> _K;
	BitMatrix<N> _C;

	unordered_set<BitVector<N> > docKeySet;
	unordered_set<BitMatrix<N, 2*N> > docAddressFunctionSet;
	unordered_map<UUID, BitVector<N> > docToKeyMap;
	unordered_map<UUID, BitMatrix<N, 2*N> > docToAddressFunctionMap;


	/*
     * Function: generateK()
     * Returns a random client-specific n x 2n matrix K_\Omega
     * with 0 bottom left and top right blocks
     * Assumes N is even (also it should be a multiple of 128)
     */
	const BitMatrix<N, 2*N> generateK() const{
		BitMatrix<N> K1 = BitMatrix<N>::randomInvertibleMatrix();
		BitMatrix<N> K2 = BitMatrix<N>::randomInvertibleMatrix();

		return BitMatrix<N, 2*N>::augH(K1, K2);
	}

	/*
     * Function: generateHashMatrix()
     * Returns the matrix portion of the hash function
     * Applied to x concatenated with y
     */
	const BitMatrix<N, 4*N> generateHashMatrix() const{
		BitMatrix<2*N> Mi = _pk.getM().inv();
		BitMatrix<N, 2*N> Mi1 = Mi.splitV(0);
		BitMatrix<N, 2*N> Mi2 = Mi.splitV(1);

		BitMatrix<N, 2*N> left = _pk.getB().inv() * Mi1;
		BitMatrix<N, 2*N> right = _pk.getA().inv() * Mi2;
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
	const MultiQuadTuple<N, 2*N> generateAugmentedF2() const{
		MultiQuadTuple<N, N> f2 = _pk.getf().get(1);
		MultiQuadTuple<N, N> topBot = (f2 * _C);
		MultiQuadTuple<N, 2*N> augmentedDecrypt = MultiQuadTuple<N, 2*N>::augV(topBot, topBot);
		return augmentedDecrypt.rMult(_K);
	}

	/*
     * Function: generateConcealedF1()
     * Returns the f1 C portion of the hash function
     * Applied to x and y separately
     */
	const MultiQuadTuple<2*N, N> generateConcealedF1() const{
		MultiQuadTuple<N, N> f1 = _pk.getf().get(0);
		BitMatrix<N, 2*N> Mi2 = _pk.getM().inv().splitV(1);
		BitMatrix<N, 2*N> inner = _pk.getA().inv() * Mi2;
		return (f1 * inner).rMult(_C.inv());
	}


	/*
	 * Function: generateDocKey
	 * Returns a serialized random unused document key
	 * Returns 0 if object has an existing key
	 */
	const BitVector<N> generateDocKey(const UUID & objectId) const{
		BitVector<N> docKey = BitVector<N>::randomVector();
		if (docToKeyMap.count(objectId) == 0) {
	        docKey = BitVector<N>::randomVector();
		}
		return docKey;
	}
};

#endif/* defined(__krypto__SearchPrivateKey__) */