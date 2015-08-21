//
//  KryptnosticEngine.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 8/13/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ implementation of the Kryptnostic Engine
//  Provides get functions for all of the cryptographic keys and functions
//  necessary for Kryptnostic search functionality
//

#ifndef krypto_KryptnosticEngine_h
#define krypto_KryptnosticEngine_h

#include "SearchPublicKey.h"
#include <string>
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

#define N 128

class KryptnosticEngine {

public:

/* Constructors */

	/*
     * Constructor
     * Constructs a Kryptnostic Engine from scratch
     */
	KryptnosticEngine() :
	_pk(),
	_spk(_pk),
	_spubk(_spk),
	vector(BitVector<1>::randomVector())
	{

	}

	/*
     * Constructor
     * Constructs a Kryptnostic Engine given private and public keys
     */
	KryptnosticEngine(const PrivateKey<N> pk, const SearchPrivateKey<N> spk, const SearchPublicKey<N> spubk) :
	_pk(pk),
	_spk(spk),
	_spubk(spubk),
	vector(BitVector<1>::randomVector())
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
	 * Function: getSearchPublicKey
	 * Returns a serialized search public key
	 */
	const val getSearchPublicKey() const{
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
	 * Function: getConversionMatrix
	 * Returns a serialized BitMatrix generated as the
	 * ConversionMatrix L_i K_\Omega^\cross for a document
	 */
	const val getConversionMatrix(const UUID & objectId) {
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

	/*
	 * Function: getDocumentIndexPair
	 * Returns a serialized pair of (FHE-encrypted DocumentSearchKey, ConversionMatrix)
	 */
	const val getDocumentIndexPair(const UUID & objectId) const{
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return val(memory_view<unsigned char>(sizeof(vector), pointer));
	}

	/*
	 * Function: getMetadataAddress
	 * Returns the serialized address from a given token and documentSearchKey
	 */
	const val getMetadataAddress(const string & token, const BitVector<N> & docKey) const{
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
	SearchPublicKey<N> _spubk;
	const BitVector<1> vector; //for testing purposes
};
#endif

	//const BridgeKey<N> _bk;
	//const PublicKey<N> _pubk;
	// const string _serialXor;
	// const string _serialAnd;
	// const string _serialLeftShift;


	// /*
 //     * Constructor
 //     * Constructs a fresh Kryptnostic Engine given all keys
 //     */
	// KryptnosticEngine(const PrivateKey<N> pk, const PublicKey<N> pubk, const string oldXor,
	// 	const string oldAnd, const string oldLeftShift) :
	// _pk(pk),
	// _bk(_pk),
	// _pubk(pubk),
	// _spk(_pk),
	// _serialXor(oldXor),
	// _serialAnd(oldAnd),
	// _serialLeftShift(oldLeftShift),
	// vector(BitVector<1>::randomVector())
	// {

	// }

	// /*
	//  * Function: getPublicKey
	//  * Returns a serialized public key
	//  */
	// const val getPublicKey() const{
	// 	unsigned char * pointer = (unsigned char *) &vector;
	// 	vector.print();
	// 	return val(memory_view<unsigned char>(sizeof(vector), pointer));
	// }

	// /*
	//  * Function: getXor
	//  * Returns a serialized function that performs bitwise Xor operations
	//  */
	// const val getXor() const{
	// 	unsigned char * pointer = (unsigned char *) &vector;
	// 	vector.print();
	// 	return val(memory_view<unsigned char>(sizeof(vector), pointer));
	// }

	// /*
	//  * Function: getAnd
	//  * Returns a serialized function that performs bitwise AND operations
	//  */
	// const val getAnd() const{
	// 	unsigned char * pointer = (unsigned char *) &vector;
	// 	vector.print();
	// 	return val(memory_view<unsigned char>(sizeof(vector), pointer));
	// }

	// /*
	//  * Function: getLeftShift
	//  * Returns a serialized function that performs left shift operations
	//  */
	// const val getLeftShift() const{
	// 	unsigned char * pointer = (unsigned char *) &vector;
	// 	vector.print();
	// 	return val(memory_view<unsigned char>(sizeof(vector), pointer));
	// }