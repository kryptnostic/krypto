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
#include "UUID.h"
#include <string>
#include <unordered_map>
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

#define N 2
#define L 2

using namespace emscripten;

//template<unsigned int N, unsigned int L>
class KryptnosticEngine {

public:

/* Constructors */

	/*
     * Constructor
     * Constructs a Kryptnostic Engine from scratch
     */
	KryptnosticEngine() :
	_pk(),
	_bk(_pk),
	_pubk(_bk),
	vector(BitVector<1>::randomVector())
	{

	}

	/*
     * Constructor
     * Constructs a Kryptnostic Engine given private and public keys
     */
	KryptnosticEngine(const PrivateKey<N, L> pk, const PublicKey<N, L> pubk) :
	_pk(pk),
	_bk(_pk),
	_pubk(pubk),
	vector(BitVector<1>::randomVector())
	{
		
	}

	/*
     * Constructor
     * Constructs a fresh Kryptnostic Engine given all keys
     */
	KryptnosticEngine(const PrivateKey<N, L> pk, const PublicKey<N, L> pubk, const string oldXor,
		const string oldAnd, const string oldLeftShift) :
	_pk(pk),
	_bk(_pk),
	_pubk(pubk),
	_serialXor(oldXor),
	_serialAnd(oldAnd),
	_serialLeftShift(oldLeftShift),
	vector(BitVector<1>::randomVector())
	{
		
	}

/* Keys */

	/*
	 * Function: getPrivateKey
	 * Returns a serialized private key
	 */
	const memory_view<unsigned char> getPrivateKey() const{
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return memory_view<unsigned char>(sizeof(vector), pointer);
	}

	/*
	 * Function: getPublicKey
	 * Returns a serialized public key
	 */
	const memory_view<unsigned char> getPublicKey() const{
		unsigned char * pointer = (unsigned char *) &vector;
		vector.print();
		return memory_view<unsigned char>(sizeof(vector), pointer);
	}

	/*
	 * Function: getServerSearchFunction
	 * Returns a serialized homomorphic hash function
	 */
	const string getServerSearchFunction() const{
		return "";
	}

	/*
	 * Function: getXor
	 * Returns a serialized function that performs bitwise Xor operations
	 */
	const string getXor() const{
		return _serialXor;
	}

	/*
	 * Function: getAnd
	 * Returns a serialized function that performs bitwise AND operations
	 */
	const string getAnd() const{
		return _serialAnd;
	}

	/*
	 * Function: getLeftShift
	 * Returns a serialized function that performs left shift operations
	 */
	const string getLeftShift() const{
		return _serialLeftShift;
	}

	/*
	 * Function: getDocKey
	 * Returns a serialized random unused document key
	 * and inserts the document key into a stored hash set
	 * Returns existing key if object has an existing key
	 */
	const UUID getDocKey(const UUID & objectId) {
		UUID docKey = generateDocKey(objectId);
		if (!docKey.isZero()) { //objectId already used
			while (docKeySet.count(docKey) == 1) docKey = generateDocKey(objectId); //generated new key

			docToKeyMap[objectId] = docKey;
			docKeySet.insert(docKey);
		} else docKey = docToKeyMap[objectId];
		return docKey;
	}

/* Transformers */

	/*
	 * Function: getHashedToken
	 * Returns the serialized result from hashing a given token and a given document key
	 */
	const string getHashedToken(const string & token, const UUID & docKey) const{
		return "";
	}

	/*
	 * Function: getEncryptedSearchTerm
	 * Returns a serialized encrypted search term
	 */
	const string getEncryptedSearchTerm(const UUID & objectId) const{
		return "";
	}

/* Setters */

	/*
	 * Function: setDocKey
	 * Sets the document key of a given object to a given document key
	 * Returns whether the operation was valid and successful
	 */
	const bool setDocKey(const UUID & objectId, const UUID & docKey) {
		if (docToKeyMap.count(objectId) != 0) {
			docToKeyMap[objectId] = docKey;
			return true;
		}
		return false;
	}

private:
	const PrivateKey<N, L> _pk;
	const BridgeKey<N, L> _bk;
	const PublicKey<N, L> _pubk;
	const string _serialXor;
	const string _serialAnd;
	const string _serialLeftShift;
	const BitVector<1> vector; //for testing purposes
	unordered_set<UUID> docKeySet;
	unordered_map<UUID, UUID> docToKeyMap;

/* Generators */

	/*
	 * Function: generateDocKey
	 * Returns a serialized random unused document key
	 * Returns 0 if object has an existing key
	 */
	const UUID generateDocKey(const UUID & objectId) const{
		UUID docKey;
		if (docToKeyMap.count(objectId) == 0) {
	        docKey.randomize();
		}
		return docKey;
	}

};
#endif

EMSCRIPTEN_BINDINGS(crypto_module) {
	class_< KryptnosticEngine >("KryptnosticEngine")
		.constructor<>()
		.function("getPrivateKey", &KryptnosticEngine::getPrivateKey)
		.function("getPublicKey", &KryptnosticEngine::getPublicKey)
		.function("getServerSearchFunction", &KryptnosticEngine::getServerSearchFunction)
		.function("getXor", &KryptnosticEngine::getXor)
		.function("getAnd", &KryptnosticEngine::getAnd)
		.function("getLeftShift", &KryptnosticEngine::getLeftShift)
		.function("getDocKey", &KryptnosticEngine::getDocKey)
		.function("setDocKey", &KryptnosticEngine::setDocKey)
		.function("getHashedToken", &KryptnosticEngine::getHashedToken)
		.function("getEncryptedSearchTerm", &KryptnosticEngine::getEncryptedSearchTerm)
		;
}

	// EMSCRIPTEN_BINDINGS(KryptnosticEngine) {
	// 	emscripten::value_object<unsigned char>("blob");
	// }