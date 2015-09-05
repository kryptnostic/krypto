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
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include "../SearchPrivateKey.h"
#include "../ClientHashFunction.h"
#include "../PrivateKey.h"

using namespace emscripten;
typedef unsigned char byte;

#define N 128
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
	KryptnosticClient(byte * pk, byte * spk) :
	_pk(*reinterpret_cast<const PrivateKey<N>*>(pk.data())),
	_spk(*reinterpret_cast<const SearchPrivateKey<N>*>(spk.data()))
	{}

/* Registration */

	/*
	 * Function: getPrivateKey()
	 * Returns a serialized private key
	 */
	const val getPrivateKey() const{
		return val(memory_view<byte>(sizeof(PrivateKey<N>), (byte *) &_pk));
	}

	/*
	 * Function: getSearchPrivateKey()
	 * Returns a serialized search private key
	 */
	const val getSearchPrivateKey() const{
		return val(memory_view<byte>(sizeof(SearchPrivateKey<N>), (byte *) &_spk));
	}

	/*
	 * Function: getClientHashFunction()
	 * Returns a serialized concatenation of the three components
	 * of the ClientHashFunction
	 */
	const val getClientHashFunction() const{
		ClientHashFunction<N> newClientHashFunction = _spk.getClientHashFunction(_pk);
		return val(memory_view<byte>(sizeof(ClientHashFunction<N>), (byte *) &newClientHashFunction));
	}

/* Indexing */

	/*
	 * Function: getObjectSearchKey()
	 * Returns a serialized ObjectSearchKey
	 */
	const val getObjectSearchKey() const{
		const BitVector<N> & objectSearchKey = _spk.getObjectSearchKey();
		return val(memory_view<byte>(sizeof(BitVector<N>), (byte *) &objectSearchKey));
	}

	/*
	 * Function: getObjectAddressMatrix()
	 * Returns a serialized ObjectAddressMatrix
	 */
	const val getObjectAddressMatrix() const{
		const BitMatrix<N> & objectAddressMatrix = _spk.getObjectAddressMatrix();
		return val(memory_view<byte>(sizeof(BitMatrix<N>), (byte *) &objectAddressMatrix));
	}

	/*
	 * Function: getObjectIndexPair(objectSearchKey, objectAddressMatrix)
	 * Returns a serialized pair of (FHE-encrypted ObjectSearchKey, ObjectAddressMatrix)
	 */
	const val getObjectIndexPair(byte * objectSearchKeyStr, byte * objectAddressMatrixStr) const{
		const BitVector<N> & objectSearchKey = *reinterpret_cast<const BitVector<N>*>(objectSearchKeyStr.data());
		const BitMatrix<N> & objectAddressMatrix = *reinterpret_cast<const BitMatrix<N>*>(objectAddressMatrixStr.data());
		std::pair<BitVector<2*N>, BitMatrix<N> > objectIndexPair = _spk.getObjectIndexPair(objectSearchKey, objectAddressMatrix, _pk);
		return val(memory_view<byte>(sizeof(std::pair <BitVector<2*N>,BitMatrix<N> >), (byte *) &objectIndexPair));
	}

	/*
	 * Function: getMetadatumAddress(objectAddressMatrix, objectSearchKey, token)
	 * Client-side address computation on raw object data and token
	 * Returns the address of the metadatum corresponding to an object and a token
	 */
	const val getMetadatumAddress(byte * objectAddressMatrixStr, byte * objectSearchKeyStr, byte * tokenStr) const{
		const BitMatrix<N> & objectAddressMatrix = *reinterpret_cast<const BitMatrix<N>*>(objectAddressMatrixStr.data());
		const BitVector<N> & token = *reinterpret_cast<const BitVector<N>*>(tokenStr.data());
		const BitVector<N> & objectSearchKey = *reinterpret_cast<const BitVector<N>*>(objectSearchKeyStr.data());

		const BitVector<N> metadatumAddress = _spk.getMetadatumAddress(objectAddressMatrix, token, objectSearchKey);
		return val(memory_view<byte>(sizeof(BitVector<N>), (byte *) &metadatumAddress));
	}

/* Searching */

	/*
	 * Function: getEncryptedSearchToken(search token)
	 * Returns a serialized FHE-encrypted search token
	 */
	const val getEncryptedSearchToken(byte * tokenStr) const{
		const BitVector<N> & token = *reinterpret_cast<const BitVector<N>*>(tokenStr.data());
		const BitVector<2*N> eToken = _pk.encrypt(token);
		return val(memory_view<byte>(sizeof(BitVector<2*N>), (byte *) &eToken));
	}

/* Sharing */

	/*
	 * Function: getObjectSharingPair(objectIndexPair)
	 * Returns a serialized pair of (FHE-encrypted objectSearchKey, objectConversionMatrix)
	 * Sent by a client to another to share a document
	 */
	const val getObjectSharingPair(byte * objectIndexPairStr) const{
		const std::pair< BitVector<2*N>, BitMatrix<N> > & objectIndexPair = *reinterpret_cast<const std::pair<BitVector<2*N>, BitMatrix<N> >* >(objectIndexPairStr.data());
		std::pair< BitVector<N>, BitMatrix<N> > objectSharingPair = _spk.getObjectSharingPair(objectIndexPair, _pk);
		return val(memory_view<byte>(sizeof(std::pair <BitVector<2*N>,BitMatrix<N> >), (byte *) &objectSharingPair));
	}

	/*
	 * Function: getObjectSharingPair(objectSharingPair)
	 * Returns a serialized pair of (FHE-encrypted objectSearchKey, objectConversionMatrix)
	 * Performed after the client receives a SharingPair from another client
	 * Assume the two inputs are RSA-decrypted before passing in to C++
	 */
	const val getObjectIndexPairFromSharing(byte * objectSharingPairStr) const{
		const std::pair< BitVector<N>, BitMatrix<N> > objectSharingPair = *reinterpret_cast<const std::pair<BitVector<N>, BitMatrix<N> >* >(objectSharingPairStr.data());
		std::pair< BitVector<2*N>, BitMatrix<N> > objectUploadPair = _spk.getObjectIndexPairFromSharing(objectSharingPair, _pk);
		return val(memory_view<byte>(sizeof(std::pair <BitVector<2*N>,BitMatrix<N> >), (byte *) &objectUploadPair));
	}

private:
	const PrivateKey<N> _pk;
	const SearchPrivateKey<N> _spk;
};
#endif