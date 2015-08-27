#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include "../cpp/MultiQuadTuple.h"
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;
typedef unsigned char byte;

#define N 64
static MultiQuadTuple<N, N> input;

const val getMQT(){
	input.randomize();
	byte *pointer = (byte *)(&input);
	return val(memory_view<byte>(sizeof(MultiQuadTuple<N, N>), pointer));
}

void readMQT( std::string s ){
	const MultiQuadTuple<N,N> value = *reinterpret_cast<const MultiQuadTuple<N,N>*>(s.data());
	BitVector<N> v = BitVector<N>::randomVector();
	BitVector<N> expected = input( v );
	BitVector<N> actual = value( v );

	assert( v == v );
	assert( actual == expected );
}

void readBV( std::string s ){
	const BitVector<N> value = *reinterpret_cast<const BitVector<N>*>(s.data())
}

EMSCRIPTEN_BINDINGS(test_module) {
	emscripten::function("readMQT", &readMQT);
	emscripten::function("getMQT", &getMQT);
}