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
#include "SearchPrivateKey.h"
#include "ClientHashFunction.h"

using namespace emscripten;
typedef unsigned char byte;

#define N 128

class KryptnosticClient {

public:

/* Constructors */

	/*
     * Constructor
     * Constructs a Kryptnostic  from scratch
     * Used for first time registeration
     */
	KryptnosticClient() :
	_pk(),
	_spk()
	{

	}

	/*
     * Constructor
     * Constructs a Kryptnostic  given private and public keys
     * Used for future logins (not registeration)
     */
	KryptnosticClient(std::string pk, std::string spk) :
	_pk(*reinterpret_cast<const PrivateKey<N>*>(pk.data())),
	_spk(*reinterpret_cast<const SearchPrivateKey<N>*>(spk.data()))
	{

	}

/* Registration */

	/*
	 * Function: getPrivateKey
	 * Returns a serialized private key
	 */
	const val getPrivateKey() const{
		byte * pointer = (byte *) &_pk;
		return val(memory_view<byte>(sizeof(PrivateKey<N>), pointer));
	}

	/*
	 * Function: getSearchPrivateKey
	 * Returns a serialized search private key
	 */
	const val getSearchPrivateKey() const{
		byte * pointer = (byte *) &_spk;
		return val(memory_view<byte>(sizeof(SearchPrivateKey<N>), pointer));
	}

	/*
	 * Function: getClientHashFunction
	 * Returns a serialized concatenation of the three components
	 * of the ClientHashFunction
	 */
	const val getClientHashFunction() const{
		ClientHashFunction<N> newClientHashFunction = _spk.getClientHashFunction(_pk);
		byte * pointer = (byte *) &newClientHashFunction;
		return val(memory_view<byte>(sizeof(ClientHashFunction<N>), pointer));
	}

/* Indexing */

	const val getObjectSearchKey() const{
		const BitVector<N> & objectSearchKey = _spk.getObjectSearchKey();
		byte * pointer = (byte *) &objectSearchKey;
		return val(memory_view<byte>(sizeof(BitVector<N>), pointer));
	}


	const val getObjectAddressFunction() const{
		const BitMatrix<N> & objectAddressFunction = _spk.getObjectAddressFunction();
		byte * pointer = (byte *) &objectAddressFunction;
		return val(memory_view<byte>(sizeof(BitMatrix<N>), pointer));
	}

	/*
	 * Function: getObjectIndexPair
	 * Returns a serialized pair of (FHE-encrypted convertedObjectSearchKey, ObjectConversionMatrix(C_o * C_i^{-1}))
	 */
	const val getObjectIndexPair(std::string objectSearchKeyStr, std::string objectAddressFunctionStr) const{
		const BitVector<N> & objectSearchKey = *reinterpret_cast<const BitVector<N>*>(objectSearchKeyStr.data());
		const BitMatrix<N> & objectAddressFunction = *reinterpret_cast<const BitMatrix<N>*>(objectAddressFunctionStr.data());
		std::pair<BitVector<2*N>, BitMatrix<N> > objectIndexPair = _spk.getObjectIndexPair(objectSearchKey, objectAddressFunction, _pk);
		byte * pointer = (byte *) &objectIndexPair;
		return val(memory_view<byte>(sizeof(std::pair <BitVector<2*N>,BitMatrix<N> >), pointer));
	}

	/*
	 * Function: getMetadatumAddress
	 * Returns the address of the metadatum corresponding to an
	 * object and a token.
	 */
	const val getMetadatumAddress(std::string objectAddressFunctionStr, std::string tokenStr, std::string objectSearchKeyStr) const{
		const BitMatrix<N> & objectAddressFunction = *reinterpret_cast<const BitMatrix<N>*>(objectAddressFunctionStr.data());
		const BitVector<N> & token = *reinterpret_cast<const BitVector<N>*>(tokenStr.data());
		const BitVector<N> & objectSearchKey = *reinterpret_cast<const BitVector<N>*>(objectSearchKeyStr.data());

		BitVector<N> metadatumAddress = _spk.getMetadatumAddress(objectAddressFunction, token, objectSearchKey);
		unsigned char * pointer = (unsigned char *) &metadatumAddress;
		return val(memory_view<unsigned char>(sizeof(BitVector<N>), pointer));
	}

/* Searching */

	/*
	 * Function: getEncryptedSearchToken
	 * Returns a serialized FHE-encrypted search token
	 */
	const val getEncryptedSearchToken(std::string tokenStr) const{
		const BitVector<N> & token = *reinterpret_cast<const BitVector<N>*>(tokenStr.data());
		const BitVector<2*N> eToken = _pk.encrypt(token);
		byte * pointer = (byte *) &eToken;
		return val(memory_view<byte>(sizeof(BitVector<2*N>), pointer));
	}

	/*
	 * Function: getMetadatumAddressFromPair
	 * Given a token and a object key, returns the address for the
	 * associated metadatum (everything seialized)
	 */
	const val getMetadatumAddressFromPair(std::string tokenStr, std::string objectIndexPairStr) const{
		const BitVector<N> & token = *reinterpret_cast<const BitVector<N>*>(tokenStr.data());
		const std::pair< BitVector<2*N>, BitMatrix<N> > objectIndexPair = *reinterpret_cast<const std::pair<BitVector<2*N>, BitMatrix<N> >* >(objectIndexPairStr.data());
		BitVector<N> metadatumAddress = _spk.getMetadatumAddressFromPair(token, objectIndexPair, _pk);
		byte *pointer = (byte *) &metadatumAddress;
		return val(memory_view<byte>(sizeof(BitVector<N>), pointer));
	}

/* Sharing */

	/*
	 * Function: getObjectSharingPair
	 * Returns a serialized pair of (FHE-encrypted ObjectSearchKey, ObjectConversionMatrix(L_iK_\Omega^\dagger))
	 * This is sent by a client to another to share a document
	 */
	//const val getObjectSharingPair(std::string objectAddressFunctionStr, std::string objectSearchKeyStr) const{
	const val getObjectSharingPair(std::string objectIndexPairStr) const{
		const std::pair< BitVector<2*N>, BitMatrix<N> > objectIndexPair = *reinterpret_cast<const std::pair<BitVector<2*N>, BitMatrix<N> >* >(objectIndexPairStr.data());
		std::pair< BitVector<N>, BitMatrix<N> > objectSharingPair = _spk.getObjectSharingPair(objectIndexPair, _pk);
		byte * pointer = (byte *) &objectSharingPair;
		return val(memory_view<byte>(sizeof(std::pair <BitVector<2*N>,BitMatrix<N> >), pointer));
	}

	/*
	 * Function: getObjectSharingPair
	 * Returns a serialized pair of (FHE-encrypted ObjectSearchKey, ObjectConversionMatrix(L_iK_\Omega^\dagger))
	 * This is done after the client receive a document shared by another client
	 * Assume the two inputs are RSA-decrypted before passing in to C++
	 */
	//const val getObjectSharingPair(std::string objectAddressFunctionStr, std::string objectSearchKeyStr) const{
	const val getObjectUploadPair(std::string objectSharingPairStr) const{
		const std::pair< BitVector<N>, BitMatrix<N> > objectSharingPair = *reinterpret_cast<const std::pair<BitVector<N>, BitMatrix<N> >* >(objectSharingPairStr.data());
		std::pair< BitVector<2*N>, BitMatrix<N> > objectUploadPair = _spk.getObjectUploadPair(objectSharingPair, _pk);
		byte * pointer = (byte *) &objectUploadPair;
		return val(memory_view<byte>(sizeof(std::pair <BitVector<2*N>,BitMatrix<N> >), pointer));
	}

private:
	const PrivateKey<N> _pk;
	const SearchPrivateKey<N> _spk;
};
#endif