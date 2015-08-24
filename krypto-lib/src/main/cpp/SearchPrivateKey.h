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
	 * Function: getObjectSearchKey
	 * Returns a serialized random unused object key
	 * and inserts the object key into a stored hash set
	 * Returns existing key if object has an existing key
	 */
	const BitVector<N> getObjectSearchKey( const UUID & objectId ) {
		BitVector<N> objectKey = generateObjectKey(objectId);
		if (!objectKey.isZero()) { //objectId already used
			while (objectKeySet.count(objectKey) == 1) objectKey = generateObjectKey(objectId); //generated new key
			objectToKeyMap[objectId] = objectKey;
			objectKeySet.insert(objectKey);
		} else objectKey = objectToKeyMap[objectId];
		return objectKey;
	}

	/*
	 * Function: setObjectSearchKey
	 * Sets the object key of a given object to a given object key
	 * Returns whether the operation was valid and successful
	 */
	const bool setObjectSearchKey(const UUID & objectId, const BitVector<N> & objectKey) {
		if (objectToKeyMap.count(objectId) != 0) {
			objectToKeyMap[objectId] = objectKey;
			return true;
		}
		return false;
	}

	/*
	 * Function: getObjectAddressFunction
	 * Returns a serialized random unused object address function L
	 * and inserts the object address function into a stored hash set
	 * Returns existing address function if object has an existing key
	 */
	const BitMatrix<N, 2*N> getObjectAddressFunction(const UUID & objectId) {
		BitMatrix<N, 2*N> addressMatrix;
		if (objectKeySet.count(objectId) == 0) { //objectId already used
			addressMatrix = generateK();
			while (addressMatrix.objectAddressFunctionSet.count(addressMatrix) == 1) addressMatrix = generateK(); //generated new matrix
			objectToAddressFunctionMap[objectId] = addressMatrix;
			objectAddressFunctionSet.insert(addressMatrix);
		} else addressMatrix = objectToAddressFunctionMap[objectId];
		return addressMatrix;
	}

	/*
	 * Function: getAddress
	 * Given a token and a object key, returns the address for the
	 * associated metadatum
	 */
	const BitVector<N> getAddress(const BitVector<N> & token, const UUID & objectId) const{
		BitVector<N> objectKey = objectToKeyMap[objectId];
		BitMatrix<N, 2*N> addressMatrix = objectToAddressFunctionMap[objectId];

		return addressMatrix * (BitVector<2*N>::vCat(token, objectId));
	}

private:
	PrivateKey<N> _pk;
	BitMatrix<N, 2*N> _K;
	BitMatrix<N> _C;

	unordered_set<BitVector<N> > objectKeySet;
	unordered_set<BitMatrix<N, 2*N> > objectAddressFunctionSet;
	unordered_map<UUID, BitVector<N> > objectToKeyMap;
	unordered_map<UUID, BitMatrix<N, 2*N> > objectToAddressFunctionMap;


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


	/*
	 * Function: generateObjectKey
	 * Returns a serialized random unused object key
	 * Returns 0 if object has an existing key
	 */
	const BitVector<N> generateObjectSearchKey(const UUID & objectId) const{
		BitVector<N> objectKey = BitVector<N>::randomVector();
		if (objectToKeyMap.count(objectId) == 0) {
	        objectKey = BitVector<N>::randomVector();
		}
		return objectKey;
	}
};

#endif/* defined(__krypto__SearchPrivateKey__) */