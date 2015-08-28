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
	 * Function: getObjectSearchKey
	 * Returns a random object search key to be serialized
	 * R_o^{-1}R_id_i
	 */
	const BitVector<N> getObjectSearchKey() const{
		return BitVector<N>::randomVector();
	}

	//documentC
	const BitMatrix<N> getObjectAddressFunction() const{
		return BitMatrix<N>::randomInvertibleMatrix();
	}

	/*
	 * Function: getObjectConversionMatrix
	 * Returns object(document) conversion matrix given object address function
	 * K_doc * K_user^{-1}
	 */
	const BitMatrix<N> getObjectConversionMatrix(const BitMatrix<N> & objectAddressFunction) const{
		return objectAddressFunction * _K.inv();
	}

	/*
	 * Function: getMetadatumAddress
	 * Given a token and a object key, returns the address for the
	 * associated metadatum
	 */
	const BitVector<N> getMetadatumAddress(const BitMatrix<N> & objectAddressFunction, const BitVector<N> &token, const BitVector<N> & objectSearchKey) const{
		return objectAddressFunction * (token ^ (_R * objectSearchKey));
	}

	/*
	 * Function: getMetadatumAddressFromPair
	 * Given a token and a object key, returns the address for the
	 * associated metadatum
	 */
	const BitVector<N> getMetadatumAddressFromPair(const BitVector<N> &token, const std::pair<BitVector<2*N>, BitMatrix<N> > & objectIndexPair, const PrivateKey<N> & pk) const{
		return objectIndexPair.second * _K * (token ^ (_R * pk.decrypt(objectIndexPair.first)));
	}

	/*
	 * Function: getMetadatumAddress
	 * Given a object index pair, returns the address for the
	 * associated metadatum
	 */
	const ClientHashFunction<N> getClientHashFunction(const PrivateKey<N> & pk) const{
		ClientHashFunction<N> h;
		h.initialize(BitMatrix<N>::randomInvertibleMatrix(), _K * BitMatrix<N, 2*N>::augH(BitMatrix<N>::identityMatrix(), _R), pk);
		return h;
	}

	//to upload to the server during indexing
	//uploaded = {E(R_o^{-1}R_id_i), C_i * C_o^{-1}}
	const std::pair<BitVector<2*N>, BitMatrix<N> > getObjectIndexPair(const BitVector<N> & objectSearchKey, const BitMatrix<N> & objectAddressFunction, const PrivateKey<N> & pk) const{
		return std::make_pair(pk.encrypt(objectSearchKey), getObjectConversionMatrix(objectAddressFunction));
	}

	//uploaded = {E(R_o^{-1}R_id_i), C_i * C_o^{-1}}
	const std::pair<BitVector<N>, BitMatrix<N> > getObjectSharingPair(const std::pair<BitVector<2*N>, BitMatrix<N> > & uploaded, const PrivateKey<N> & pk) const{
		return std::make_pair(_R * pk.decrypt(uploaded.first), uploaded.second * _K);
	}


	//to be uploaded after getting the sharing pair
	const std::pair<BitVector<2*N>, BitMatrix<N> > getObjectUploadPair(const std::pair<BitVector<N>, BitMatrix<N> > & shared, const PrivateKey<N> & pk) const{
		return std::make_pair(pk.encrypt(_R.solve(shared.first)), getObjectConversionMatrix(shared.second));
	}

private:
	BitMatrix<N> _K; //front protection
	BitMatrix<N> _R; //back protection
};

#endif/* defined(__krypto__SearchPrivateKey__) */