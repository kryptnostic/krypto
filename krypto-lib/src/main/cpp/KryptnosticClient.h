//
//  KryptnosticClient.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 8/13/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ implementation of the client-side Kryptnostic
//  Provides get functions for all of the cryptographic keys and functions
//  necessary for Kryptnostic search functionality
//

#ifndef krypto_KryptnosticClient_h
#define krypto_KryptnosticClient_h

#include <utility> //for std::pair
#include <iostream>
#include <string>
#include "SearchPrivateKey.h"
#include "ClientHashFunction.h"
#include "PrivateKey.h"

template<unsigned int N>
class KryptnosticClient {

public:

/* Constructors */

	/*
     * Constructor: ()
     * Constructs a KryptnosticClient from scratch
     * Used for first time registration
     */
	KryptnosticClient() :
	_pk(),
	_spk()
	{}

	/*
     * Constructor: (privateKey, searchPrivateKey)
     * Constructs a KryptnosticClient given private and public keys
     * Used for future logins (not registeration)
     */
	KryptnosticClient(PrivateKey<N> pk, SearchPrivateKey<N> spk) :
	_pk(pk),
	_spk(spk)
	{}

/* Registration */

	/*
	 * Function: getPrivateKey()
	 * Returns a serialized private key
	 */
	const PrivateKey<N> getPrivateKey() const{
		return _pk;
	}

	/*
	 * Function: getSearchPrivateKey()
	 * Returns a serialized search private key
	 */
	const SearchPrivateKey<N> getSearchPrivateKey() const{
		return _spk;
	}

	/*
	 * Function: getClientHashFunction()
	 * Returns a serialized concatenation of the three components
	 * of the ClientHashFunction
	 */
	const ClientHashFunction<N> getClientHashFunction() const{
		return _spk.getClientHashFunction(_pk);
	}

/* Indexing */

	/*
	 * Function: getObjectSearchKey()
	 * Returns a serialized ObjectSearchKey
	 * 2.2(2)
	 */
	const BitVector<N> getObjectSearchKey() const{
		return _spk.getObjectSearchKey();
	}

	/*
	 * Function: getObjectAddressMatrix()
	 * Returns a serialized ObjectAddressMatrix
	 * 2.2(3)
	 */
	const BitMatrix<N> getObjectAddressMatrix() const{
		return _spk.getObjectAddressMatrix();
	}


	/*
	 * Function: getObjectSearchPairFromIndexPair(objectIndexPair)
	 * Returns a serialized pair of (FHE-encrypted ObjectSearchKey, ObjectConversionMatrix)
	 * This returns the stuff that you upload to the server to be able to search your own content
	 * 2.2(5)
	 */
	const std::pair <BitVector<2*N>,BitMatrix<N> > getObjectSearchPairFromIndexPair(std::pair<BitVector<N>, BitMatrix<N> > objectIndexPair) const{
		return _spk.getObjectSearchPairFromIndexPair(objectIndexPair.first, objectIndexPair.second, _pk);
	}

	/*
	 * DELETE_LATER
	 * Function: getObjectSearchPair(objectSearchKey, objectAddressMatrix)
	 * Returns a serialized pair of (FHE-encrypted ObjectSearchKey, ObjectConversionMatrix)
	 * This returns the stuff that you upload to the server to be able to search your own content
	 * 2.2(5)
	 */
	const std::pair <BitVector<2*N>,BitMatrix<N> > getObjectSearchPair(BitVector<N> objectSearchKey, BitMatrix<N> objectAddressMatrix) const{
		return _spk.getObjectSearchPair(objectSearchKey, objectAddressMatrix, _pk);
	}

	/*
	 * Function: getMetadataAddress(objectAddressMatrix, objectSearchKey, token)
	 * Client-side address computation on raw object data and token
	 * Returns the address of the metadatum corresponding to an object and a token
	 */
	const BitVector<N> getMetadataAddress(const BitMatrix<N> & objectAddressMatrix, const BitVector<N> & objectSearchKey, const BitVector<N> & token) const{
		return _spk.getMetadataAddress(objectAddressMatrix, objectSearchKey, token);
	}

/* Searching */

	/*
	 * Function: getEncryptedSearchToken(search token)
	 * Returns a serialized FHE-encrypted search token
	 */
	const BitVector<2*N> getEncryptedSearchToken(const BitVector<N> & token) const{
		return _pk.encrypt(token);
	}

/* Sharing */

	/*
	 * Function: getObjectSharePairFromObjectSearchPair(objectSearchPair)
	 * Returns a serialized pair of (objectSearchKey, objectConversionMatrix)
	 * Sent by a client to share their stuff with others
	 */
	const std::pair< BitVector<N>, BitMatrix<N> > getObjectSharePairFromObjectSearchPair(std::pair< BitVector<2*N>, BitMatrix<N> > objectSearchPair) const{
		return _spk.getObjectSharePair(objectSearchPair, _pk);
	}

	/*
	 * Function: getObjectSearchPairFromObjectSharePair(objectSharePair)
	 * Returns a serialized pair of (FHE-encrypted objectSearchKey, objectConversionMatrix)
	 * Performed after the client receives a SharePair from another client
	 * Assume the two inputs are RSA-decrypted before passing in to C++
	 * 2.4(3)
	 */
	const std::pair< BitVector<2*N>, BitMatrix<N> > getObjectSearchPairFromObjectSharePair(std::pair< BitVector<N>, BitMatrix<N> > objectSharePair) const{
		return _spk.getObjectSearchPairFromObjectSharePair(objectSharePair, _pk);
	}

private:
	const PrivateKey<N> _pk;
	const SearchPrivateKey<N> _spk;
};
#endif