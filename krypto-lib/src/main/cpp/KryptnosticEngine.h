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

#ifndef krypto_KryptnosticEnginer_h
#define krypto_KryptnosticEngine_h

#include "PublicKey.h"
#include "SearchPublicKey.h"
#include <string>


template<unsigned int N, unsigned int L>
class KryptnosticEngine {

public:

/* Constructors */

	/*
     * Constructor
     * Constructs a Kryptnostic Engine from scratch
     */
	KryptnosticEngine(const string serverGlobal) {

	}

	/*
     * Constructor
     * Constructs a Kryptnostic Engine given private and public keys
     */
	KryptnosticEngine(const PrivateKey pk, const PublicKey bk, const string serverGlobal) {
		
	}

	/*
     * Constructor
     * Constructs a fresh Kryptnostic Engine given all keys
     */
	KryptnosticEngine(const PrivateKey pk, const PublicKey bk, const string serverGlobal) {
		
	}

/* Keys */

	/*
	 * Function: getPrivateKey
	 * Returns a serialized private key
	 */
	const string getPrivateKey() const{
		
	}

	/*
	 * Function: getPublicKey
	 * Returns a serialized public key
	 */
	const string getPublicKey() const{
		
	}

	/*
	 * Function: getServerSearchFunction
	 * Returns a serialized homomorphic hash function
	 */
	const string getServerSearchFunction() const{
		
	}

	/*
	 * Function: getXor
	 * Returns a serialized function that performs bitwise Xor operations
	 */
	const string getXor() const{
		
	}

	/*
	 * Function: getAnd
	 * Returns a serialized function that performs bitwise AND operations
	 */
	const string getAnd() const{
		
	}

	/*
	 * Function: getLeftShift
	 * Returns a serialized function that performs left shift operations
	 */
	const string getLeftShift() const{
		
	}

	/*
	 * Function: getDocKey
	 * Returns a serialized document key for a given object
	 */
	const string getDocKey(string objectId) const{
		
	}

/* Transformers */

	/*
	 * Function: getHashedToken
	 * Returns the serialized result from hashing a given token and a given document key
	 */
	const string getHashedToken(string token, string docKey) const{
		
	}

	/*
	 * Function: getEncryptedSearchTerm
	 * Returns a serialized encrypted search term
	 */
	const string getEncryptedSearchTerm(string objectId) const{
		
	}

/* Generators */

	/*
	 * Function: generateDocKey
	 * Returns a serialized random unused document key
	 * and inserts the document key into a stored hash set
	 * Returns NULL if object has an existing key
	 */
	const string generateDocKey(string objectId) const{
		
	}

/* Setters */

	/*
	 * Function: setDocKey
	 * Sets the document key of a given object to a given document key
	 * Returns whether the operation was valid and successful
	 */
	const bool setDocKey(string objectId, string docKey) const{
		
	}

private:
	// const PrivateKey<N, L> _pk;
	// const BridgeKey<N, L> _bk;
	// const PublicKey<N, L> _pubk;

};

#endif