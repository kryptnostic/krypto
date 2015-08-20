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

#include "PublicKey.h"
#include "SearchPublicKey.h"
#include "UUID.h"
#include <string>
#include <unordered_map>


template<unsigned int N>
class KryptnosticEngine {

public:

/* Constructors */

	/*
     * Constructor
     * Constructs a Kryptnostic Engine from scratch
     */
	KryptnosticEngine(const string serverGlobal) :
	_serverGlobal(serverGlobal)
	{

	}

	/*
     * Constructor
     * Constructs a Kryptnostic Engine given private and public keys
     */
	KryptnosticEngine(const string serverGlobal, const PrivateKey<N> pk, const PublicKey<N> bk) :
	_serverGlobal(serverGlobal),
	_pk(pk),
	_bk(bk)
	{

	}

	/*
     * Constructor
     * Constructs a fresh Kryptnostic Engine given all keys
     */
	KryptnosticEngine(const string serverGlobal, const PrivateKey<N> pk, const PublicKey<N> bk, const string oldXor,
		const string oldAnd, const string oldLeftShift) :
	_serverGlobal(serverGlobal),
	_pk(pk),
	_bk(bk),
	_serialXor(oldXor),
	_serialAnd(oldAnd),
	_serialLeftShift(oldLeftShift)
	{

	}

/* Keys */

	/*
	 * Function: getPrivateKey
	 * Returns a serialized private key
	 */
	const string getPrivateKey() const{
		return "";
	}

	/*
	 * Function: getPublicKey
	 * Returns a serialized public key
	 */
	const string getPublicKey() const{
		return "";
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
	 * Returns 0 if object has an existing key
	 */
	const UUID getDocKey(const UUID & objectId) const{
		UUID docKey = generateDocKey(objectId);
		if (!docKey.isZero()) { //objectId already used
			while (docKeySet.count(docKey) == 1) docKey = generateDocKey(objectId); //generated new key

			docToKeyMap.insert(pair<UUID, UUID>(objectId, docKey));
			docKeySet.insert(docKey);
		}
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
	const bool setDocKey(const UUID & objectId, const UUID & docKey) const{
		if (docToKeyMap.count(objectId) != 0) {
			docToKeyMap[objectId] = docKey;
			return true;
		}
		return false;
	}

private:
	const string _serverGlobal;
	const PrivateKey<N> _pk;
	const BridgeKey<N> _bk;
	const PublicKey<N> _pubk;
	const string _serialXor;
	const string _serialAnd;
	const string _serialLeftShift;
	const unordered_set<UUID> docKeySet;
	const unordered_map<UUID, UUID> docToKeyMap;

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