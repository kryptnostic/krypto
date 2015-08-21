//
//  KryptnosticEngineClient.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 8/13/15.
//  Copyright (c) 20645 Kryptnostic. All rights reserved.
//
//  C++ implementation of the client-side Kryptnostic Engine
//  Provides get functions for all of the cryptographic keys and functions
//  necessary for Kryptnostic search functionality
//

#ifndef krypto_KryptnosticEngineClient_h
#define krypto_KryptnosticEngineClient_h

#include "SearchPrivateKey.h"
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
	_spk(_pk),
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
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

	/*
	 * Function: getSearchPrivateKey
	 * Returns a serialized search private key
	 */
	const val getSearchPrivateKey() const{
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

	/*
	 * Function: getClientHashFunction
	 * Returns a serialized concatenation of the three components
	 * of the ClientHashFunction
	 */
	const val getClientHashFunction() {
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

/* Indexing */

	/*
	 * Function: getDocumentSearchKey
	 * Returns a serialized random unused document key
	 * and inserts the document key into a stored hash set
	 * Returns existing key if object has an existing key
	 */
	const val getDocumentSearchKey(const UUID & objectId) {
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

	/*
	 * Function: getDocumentAddressFunction
	 * Returns a serialized BitMatrix generated as the
	 * DocumentAddressFunction L_i for a document
	 */
	const val getDocumentAddressFunction(const UUID & objectId) {
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

	/*
	 * Function: getDocumentConversionMatrix
	 * Returns a serialized BitMatrix generated as the
	 * DocumentConversionMatrix L_i K_\Omega^\cross for a document
	 */
	const val getDocumentConversionMatrix(const UUID & objectId) {
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

	/*
	 * Function: getDocumentIndexPair
	 * Returns a serialized pair of (FHE-encrypted DocumentSearchKey, DocumentConversionMatrix)
	 */
	const val getDocumentIndexPair(const UUID & objectId) const{
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
	 * Function: getDocumentSharingPair
	 * Returns a serialized pair of (DocumentSearchKey, DocumentAddressFunction)
	 */
	const val getDocumentSharingPair(const UUID & objectId) const{
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

/* Setters */

	/*
	 * Function: setDocumentSearchKey
	 * Sets the DocumentSearchKey of a given object to a given document key
	 * Returns whether the operation was valid and successful
	 */
	const bool setDocumentSearchKey(const UUID & objectId, const BitVector<N> & docKey) {
		return _spk.setDocKey(objectId, docKey);
	}

private:
	const PrivateKey<N> _pk;
	SearchPrivateKey<N> _spk;
	const BitVector<64> vector; //for testing purposes
};
#endif