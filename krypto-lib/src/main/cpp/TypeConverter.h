//
//  TypeConverter.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 8/21/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  Embind val => C++ Data Structure Converters
//  Necessary for retrieval from persistent storage
//  Preliminary step before C++ computation is done
//

//TODO: Figure out how to do this exactly (embind?)

#ifndef TypeConverter_h
#define TypeConverter_h

#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include "PrivateKey.h"

using namespace emscripten;

class TypeConverter {
public:
	TypeConverter();

	template<unsigned int N_KEY>
	const PrivateKey<N_KEY> getPrivateKey(const val privateKeyVal) const{
		memory_view<unsigned char> memoryView = getMemoryView(privateKeyVal); //figure this out
		PrivateKey<N_KEY> dummy = (PrivateKey)(memoryView.data);
		PrivateKey<N_KEY> dummy;
		return dummy;
	}

	template<unsigned int N_BV>
	const BitVector<N_BV> getBitVector(const val bitVectorVal) const{
		BitVector<N_BV> dummy;
		return dummy;
	}

	template<unsigned int N_BM_ROWS, unsigned int N_BM_COLS = N_BM_ROWS>
	const BitMatrix<N_BM_ROWS, N_BM_COLS> getBitMatrix(const val bitMatrixVal) const{
		BitMatrix<N_BM_ROWS, N_BM_COLS> dummy;
		return dummy;
	}

private:

};

#endif