/**
 * JavaScriptGetter.cpp
 * Created by Peng Hui How, 8/11/2015.
 * Copyright (c) 2015 Kryptnostic. All rights reserved.
 * A class that allows kryptnostic-js to retrieve keys generated by C++ code.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include "../cpp/PrivateKey.h"
#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

#define N 1
#define L 2

/* Translation from JavaScript String */

	/*
	 * Function: getBitVector(str)
	 * Returns a BitVector object from a base-64-encoded string
	 * TODO: use a base-64-encoded string or any string format that communicates to js
	 */
	const BitVector<N> getBitVector(string str){
		//unimplemented!
		return BitVector<N>::zeroVector(); 
	}

	/*
	 * Function: getBitMatrix(str)
	 * Returns a BitMatrix object from a base-64-encoded string
	 * TODO: use a base-64-encoded string or any string format that communicates to js
	 */
	const BitMatrix<N> getBitMatrix(string str){
		//unimplemented!
		return BitMatrix<N>::squareZeroMatrix();
	}

/* Translation to JavaScript String */

	/*
	 * Function: getBitVectorString(bv)
	 * Returns a base-64-encoded string that represents the given BitVector object
	 * TODO: use a base-64-encoded string or any string format that communicates to js
	 */
	string getBitVectorString(const BitVector<N> & bv){
		return "";
	}

	/*
	 * Function: getBitMatrixString(bm)
	 * Returns a base-64-encoded string that represents the given BitMatrix object
	 * TODO: use a base-64-encoded string or any string format that communicates to js
	 */
	string getBitMatrixString(const BitMatrix<N> & bm){
		return "";
	}

/* Encryption and Decryption(upload + search) */

	/*
	 * Function: getEncryption(pk, bv)
	 * Returns the encrypted version of 
	 */
	const BitVector<2*N> getEncryption(const PrivateKey<N, L> & pk, const BitVector<N> & bv){
		return pk.encrypt(bv);
	}

EMSCRIPTEN_BINDINGS(crypto_module) {
	emscripten::function("getBitVector", &getBitVector);
	emscripten::function("getBitMatrix", &getBitMatrix);
	emscripten::function("getBitVectorString", &getBitVectorString);
	emscripten::function("getBitMatrixString", &getBitMatrixString);
	emscripten::function("getEncryption", &getEncryption);
}