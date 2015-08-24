//
//  KryptnosticEngineClient.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 8/13/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ implementation of the client-side Kryptnostic Engine
//  Provides get functions for all of the cryptographic keys and functions
//  necessary for Kryptnostic search functionality
//

#ifndef krypto_KryptnosticEngineClient_h
#define krypto_KryptnosticEngineClient_h

#include "SearchPrivateKey.h"
#include "ClientHashFunction.h"
#include <string>
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

#define N 128

class KryptnosticEngineClient {

public:

/* Constructors */

	/*
     * Constructor
     * Constructs a Kryptnostic Engine from scratch
     */
	KryptnosticEngineClient() :
	_pk(),
	_spk(),
	vector(BitVector<64>::randomVector())
	{

	}

	/*
     * Constructor
     * Constructs a Kryptnostic Engine given private and public keys
     */
	KryptnosticEngineClient(const PrivateKey<N> pk, const SearchPrivateKey<N> spk) :
	_pk(pk),
	_spk(spk),
	vector(BitVector<64>::randomVector())
	{

	}

/* Registration */

	/*
	 * Function: getPrivateKey
	 * Returns a serialized private key
	 */
	const val getPrivateKey() const{
		unsigned char * pointer = (unsigned char *) &_pk;
		return val(memory_view<unsigned char>(sizeof(_pk), pointer));
	}

	/*
	 * Function: getSearchPrivateKey
	 * Returns a serialized search private key
	 */
	const val getSearchPrivateKey() const{
		unsigned char * pointer = (unsigned char *) &_spk;
		return val(memory_view<unsigned char>(sizeof(_spk), pointer));
	}

	/*
	 * Function: getClientHashFunction
	 * Returns a serialized concatenation of the three components
	 * of the ClientHashFunction
	 */
	const val getClientHashFunction() {
		//ClientHashFunction chf = {_spk.generateHashMatrix(), _spk.generateAugmentedF2(), _spk.generateConcealedF1()};
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

/* Indexing */

	/*
	 * Function: getObjectSearchKey
	 * Returns a serialized random unused document key
	 * and inserts the document key into a stored hash set
	 * Returns existing key if object has an existing key
	 */
	const val getObjectSearchKey(const UUID & objectId) {
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

	/*
	 * Function: getObjectAddressFunction
	 * Returns a serialized BitMatrix generated as the
	 * ObjectAddressFunction L_i for a document
	 */
	const val getObjectAddressFunction(const UUID & objectId) {
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

	/*
	 * Function: getObjectConversionMatrix
	 * Returns a serialized BitMatrix generated as the
	 * ObjectConversionMatrix L_i K_\Omega^\cross for a document
	 */
	const val getObjectConversionMatrix(const UUID & objectId) {
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

	/*
	 * Function: getObjectIndexPair
	 * Returns a serialized pair of (FHE-encrypted ObjectSearchKey, ObjectConversionMatrix)
	 */
	const val getObjectIndexPair(const UUID & objectId) const{
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

/* Searching */

	/*
	 * Function: getEncryptedSearchToken
	 * Returns a serialized FHE-encrypted search token
	 */
	const val getEncryptedSearchToken(const string & word) const{
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

/* Sharing */

	/*
	 * Function: getObjectSharingPair
	 * Returns a serialized pair of (ObjectSearchKey, ObjectAddressFunction)
	 */
	const val getObjectSharingPair(const UUID & objectId) const{
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

/* Setters */

	/*
	 * Function: setObjectSearchKey
	 * Sets the ObjectSearchKey of a given object to a given document key
	 * Returns whether the operation was valid and successful
	 */
	const bool setObjectSearchKey(const UUID & objectId, const BitVector<N> & docKey) {
		return _spk.setObjectSearchKey(objectId, docKey);
	}

private:
	PrivateKey<N> _pk;
	SearchPrivateKey<N> _spk;
	const BitVector<64> vector; //for testing purposes
};
#endif