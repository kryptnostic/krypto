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

#include <utility>
#include "PrivateKey.h"
#include "ClientHashFunction.h"

//N should be a multiple of 64, otherwise BitVector would create
//the incorrect number of unsigned long longs
template<unsigned int N = 128>
class SearchPrivateKey{
public:
	SearchPrivateKey() :
	_K(BitMatrix<N>::randomInvertibleMatrix()),
	_R(BitMatrix<N>::randomInvertibleMatrix())
	{}

/* Getters */

	/*
	 * Function: getObjectSearchKey()
	 * Returns a random object search key to be serialized
	 * R_o^{-1}R_id_i
	 */
	const BitVector<N> getObjectSearchKey() const{
		return BitVector<N>::randomVector();
	}

	/*
	 * Function: getObjectAddressMatrix()
	 * Returns a random object address matrix L_i
	 */
	const BitMatrix<N> getObjectAddressMatrix() const{
		return BitMatrix<N>::randomInvertibleMatrix();
	}

	/*
	 * Function: getObjectConversionMatrix(objectAddressMatrix)
	 * Returns object(document) conversion matrix given object address function
	 * K_doc * K_user^{-1}
	 */
	const BitMatrix<N> getObjectConversionMatrix(const BitMatrix<N> & objectAddressMatrix) const{
		return objectAddressMatrix * _K.inv();
	}

	/*
	 * Function: getMetadatumAddress(objectAddressMatrix, objectSearchKey, token)
	 * Returns the address for metadatum given raw unencrypted data
	 */
	const BitVector<N> getMetadatumAddress(const BitMatrix<N> & objectAddressMatrix, const BitVector<N> & objectSearchKey, const BitVector<N> &token) const{
		return objectAddressMatrix * (token ^ (_R * objectSearchKey));
	}

	/*
	 * Function: getMetadatumAddressFromPair
	 * Test function for computing metdataum address given server-side objects
	 */
	const BitVector<N> getMetadatumAddressFromPair(const BitVector<N> &token, const std::pair<BitVector<2*N>, BitMatrix<N> > & objectIndexPair, const PrivateKey<N> & pk) const{
		return objectIndexPair.second * _K * (token ^ (_R * pk.decrypt(objectIndexPair.first)));
	}

	/*
	 * Function: getClientHashFunction(pk)
	 * Given a private key, generates a client hash function with a random matrix C
	 */
	const ClientHashFunction<N> getClientHashFunction(const PrivateKey<N> & pk) const{
		ClientHashFunction<N> h;
		h.initialize(BitMatrix<N>::randomInvertibleMatrix(), _K * BitMatrix<N, 2*N>::augH(BitMatrix<N>::identityMatrix(), _R), pk);
		return h;
	}

	/*
	 * Function: getObjectIndexPair(objectSearchKey, objectAddressMatrix, pk)
	 * Generates the (encrypted object search key, object conversion matrix) pair to be stored on server during indexing
	 * Uploaded = {E(R_o^{-1}R_id_i), C_i * C_o^{-1}}
	 */
	const std::pair<BitVector<2*N>, BitMatrix<N> > getObjectIndexPair(const BitVector<N> & objectSearchKey, const BitMatrix<N> & objectAddressMatrix, const PrivateKey<N> & pk) const{
		return std::make_pair(pk.encrypt(objectSearchKey), getObjectConversionMatrix(objectAddressMatrix));
	}

	/*
	 * Function: getObjectSharingPair(uploaded, pk)
	 * Generates the pair to be shared with another client
	 * Uploaded = {R_id_i, C_i * C_o^{-1}}
	 */
	const std::pair<BitVector<N>, BitMatrix<N> > getObjectSharingPair(const std::pair<BitVector<2*N>, BitMatrix<N> > & uploaded, const PrivateKey<N> & pk) const{
		return std::make_pair(_R * pk.decrypt(uploaded.first), uploaded.second * _K);
	}

	/*
	 * Function: getObjectIndexPairFromSharing(shared, pk)
	 * Generates the (encrypted object search key, object conversion matrix) pair to be stored on server during indexing from sharing
	 * Uploaded = {E(R_o^{-1}R_id_i), C_i * C_o^{-1}}
	 */
	const std::pair<BitVector<2*N>, BitMatrix<N> > getObjectIndexPairFromSharing(const std::pair<BitVector<N>, BitMatrix<N> > & shared, const PrivateKey<N> & pk) const{
		return std::make_pair(pk.encrypt(_R.solve(shared.first)), getObjectConversionMatrix(shared.second));
	}

private:
	BitMatrix<N> _K; //front protection
	BitMatrix<N> _R; //back protection
};

#endif/* defined(__krypto__SearchPrivateKey__) */