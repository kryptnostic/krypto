//
//	PublicKey.h
//	krypto
//
//	Created by njhlai
//	Copyright (c) 2016 Kryptnostic. All rights reserved.
//
//	Implementation of public key generation
//	Accessed by servers
//

#ifndef krypto_PublicKey_h
#define krypto_PublicKey_h 

#include "BridgeKey.h"

#define DEBUG false

/*
 * Template for BridgeKey
 */
class PublicKey{
public:
	/*
	 * Constructor
	 */
	PublicKey(const BridgeKey & bk) :
	_ADD(bk.getADD()),
	_MULT(bk.getMULT())
	{}

	/*
	 * Function: computes e(d(x) + d(y)) given ciphertext x, y homomorphically
	 * Return: e(d(x) + d(y))
	 */
	const vec_ZZ_p homomorphicADD(const vec_ZZ_p & x, const vec_ZZ_p & y) const {
		return _ADD(x, y);
	}

	/*
	 * Function: computes e(d(x) * d(y)) given ciphertext x, y homomorphically
	 * Return: e(d(x) * d(y))
	 */
	const vec_ZZ_p homomorphicMULT(const vec_ZZ_p & x, const vec_ZZ_p & y) const {
		return _MULT(x, y);
	}

private:
	const typename BridgeKey::H_ADD _ADD; // structure for homomorphic ADD
	const typename BridgeKey::H_MULT _MULT; // structure for homomorphic MULT
};

#endif