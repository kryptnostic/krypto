//
//  KryptnosticClientGetter.h
//  krypto
//
//  Created by Robin Zhang on 8/19/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  Provides Embind bindings for all of the cryptographic keys and functions
//  necessary for client-side Kryptnostic search functionality
//

#ifndef krypto_KryptnosticClientGetter_cpp
#define krypto_KryptnosticClientGetter_cpp

#include "../KryptnosticClient.h"
#include <string>
#include <iostream>
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;
typedef unsigned char byte;

#define N 128
class KryptnosticClientGetter {

public:

/* Constructors */

	/*
     * Constructor: ()
     * Constructs a KryptnosticClientGetter from scratch
     * Used for first time registration
     */
	KryptnosticClientGetter() :
	_kc()
	{}

	/*
     * Constructor: (privateKey, searchPrivateKey)
     * Constructs a KryptnosticClientGetter given private and public keys
     * Used for future logins (not registeration)
     */
	KryptnosticClientGetter(std::string pk, std::string spk) :
	_kc(*reinterpret_cast<const PrivateKey<N>*>(pk.data()), *reinterpret_cast<const SearchPrivateKey<N>*>(spk.data()))
	{}

/* Registration */

	/*
	 * Function: getPrivateKey()
	 * Returns a serialized private key
	 */
	const val getPrivateKey() const{
		PrivateKey<N> pk = _kc.getPrivateKey();
		return val(memory_view<byte>(sizeof(PrivateKey<N>), (byte *) &pk));
	}

	/*
	 * Function: getSearchPrivateKey()
	 * Returns a serialized search private key
	 */
	const val getSearchPrivateKey() const{
		SearchPrivateKey<N> spk = _kc.getSearchPrivateKey();
		return val(memory_view<byte>(sizeof(SearchPrivateKey<N>), (byte *) &spk));
	}

	/*
	 * Function: getClientHashFunction()
	 * Returns a serialized concatenation of the three components
	 * of the ClientHashFunction
	 */
	const val getClientHashFunction() const{
		ClientHashFunction<N> newClientHashFunction = _kc.getClientHashFunction();
		return val(memory_view<byte>(sizeof(ClientHashFunction<N>), (byte *) &newClientHashFunction));
	}

/* Indexing */

	/*
	 * Function: getObjectIndexPair()
	 * Returns a serialized ObjectIndexPair
	 */
	const val getObjectIndexPair() const{
		const std::pair<BitVector<N>, BitMatrix<N> > objectIndexPair = _kc.getObjectIndexPair();
		return val(memory_view<byte>(sizeof(std::pair<BitVector<N>, BitMatrix<N> >), (byte *) &objectIndexPair));
	}

	/*
	 * Function: getObjectSearchPairFromObjectIndexPair(objectIndexPair)
	 * Returns a serialized pair of (FHE-encrypted ObjectSearchKey, ObjectAddressMatrix)
	 */
	const val getObjectSearchPairFromObjectIndexPair(std::string objectIndexPairStr) const{
		const std::pair<BitVector<N>, BitMatrix<N> > & objectIndexPair = *reinterpret_cast<const std::pair<BitVector<N>, BitMatrix<N> >* >(objectIndexPairStr.data());
		std::pair<BitVector<2*N>, BitMatrix<N> > objectSearchPair = _kc.getObjectSearchPairFromObjectIndexPair(objectIndexPair);
		return val(memory_view<byte>(sizeof(std::pair <BitVector<2*N>,BitMatrix<N> >), (byte *) &objectSearchPair));
	}

	/*
	 * Function: getMetadataAddress(objectAddressMatrix, objectSearchKey, token)
	 * Client-side address computation on raw object data and token
	 * Returns the address of the metadatum corresponding to an object and a token
	 */
	const val getMetadataAddress(std::string objectIndexPairStr, std::string tokenStr) const{
		const std::pair<BitVector<N>, BitMatrix<N> > & objectIndexPair = *reinterpret_cast<const std::pair<BitVector<N>, BitMatrix<N> >* >(objectIndexPairStr.data());
		const BitVector<N> & token = *reinterpret_cast<const BitVector<N>*>(tokenStr.data());

		const BitVector<N> metadatumAddress = _kc.getMetadataAddress(objectIndexPair, token);
		return val(memory_view<byte>(sizeof(BitVector<N>), (byte *) &metadatumAddress));
	}

/* Searching */

	/*
	 * Function: getEncryptedSearchToken(search token)
	 * Returns a serialized FHE-encrypted search token
	 */
	const val getEncryptedSearchToken(std::string tokenStr) const{
		const BitVector<N> & token = *reinterpret_cast<const BitVector<N>*>(tokenStr.data());
		const BitVector<2*N> eToken = _kc.getEncryptedSearchToken(token);
		return val(memory_view<byte>(sizeof(BitVector<2*N>), (byte *) &eToken));
	}

/* Sharing */

	/*
	 * Function: getObjectSharePairFromObjectSearchPair(objectSearchPair)
	 * Returns a serialized pair of (FHE-encrypted objectSearchKey, objectConversionMatrix)
	 * Sent by a client to another to share a document
	 */
	const val getObjectSharePairFromObjectSearchPair(std::string objectSearchPairStr) const{
		const std::pair< BitVector<2*N>, BitMatrix<N> > & objectSearchPair = *reinterpret_cast<const std::pair<BitVector<2*N>, BitMatrix<N> >* >(objectSearchPairStr.data());
		std::pair< BitVector<N>, BitMatrix<N> > objectSharePair = _kc.getObjectSharePairFromObjectSearchPair(objectSearchPair);
		return val(memory_view<byte>(sizeof(std::pair <BitVector<2*N>,BitMatrix<N> >), (byte *) &objectSharePair));
	}

	/*
	 * Function: getObjectSearchPairFromObjectSharePair(objectSharePair)
	 * Returns a serialized pair of (FHE-encrypted objectSearchKey, objectConversionMatrix)
	 * Performed after the client receives a SharePair from another client
	 * Assume the two inputs are RSA-decrypted before passing in to C++
	 */
	const val getObjectSearchPairFromObjectSharePair(std::string objectSharePairStr) const{
		const std::pair< BitVector<N>, BitMatrix<N> > objectSharePair = *reinterpret_cast<const std::pair<BitVector<N>, BitMatrix<N> >* >(objectSharePairStr.data());
		std::pair< BitVector<2*N>, BitMatrix<N> > objectUploadPair = _kc.getObjectSearchPairFromObjectSharePair(objectSharePair);
		return val(memory_view<byte>(sizeof(std::pair <BitVector<2*N>, BitMatrix<N> >), (byte *) &objectUploadPair));
	}

private:
	const KryptnosticClient<N> _kc;
};

EMSCRIPTEN_BINDINGS(crypto_module) {
	class_< KryptnosticClientGetter >("KryptnosticClient")
		.constructor<>()
		.constructor<std::string, std::string>()
		.function("getPrivateKey", &KryptnosticClientGetter::getPrivateKey)
		.function("getSearchPrivateKey", &KryptnosticClientGetter::getSearchPrivateKey)
		.function("getClientHashFunction", &KryptnosticClientGetter::getClientHashFunction)
		.function("getObjectIndexPair", &KryptnosticClientGetter::getObjectIndexPair)
		.function("getObjectSearchPairFromObjectIndexPair", &KryptnosticClientGetter::getObjectSearchPairFromObjectIndexPair)
		.function("getMetadataAddress", &KryptnosticClientGetter::getMetadataAddress)
		.function("getEncryptedSearchToken", &KryptnosticClientGetter::getEncryptedSearchToken)
		.function("getObjectSharePairFromObjectSearchPair", &KryptnosticClientGetter::getObjectSharePairFromObjectSearchPair)
		.function("getObjectSearchPairFromObjectSharePair", &KryptnosticClientGetter::getObjectSearchPairFromObjectSharePair)
		;
}
#endif