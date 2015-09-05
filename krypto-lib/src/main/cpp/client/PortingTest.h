#ifndef krypto_PortingTest_h
#define krypto_PortingTest_h

#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include <string>

using namespace emscripten;
typedef unsigned char byte;

class PortingTest {
public:

PortingTest(byte *c):
i(*reinterpret_cast<byte *>(c))
{}

const val getInteger() const{
	return val(memory_view<byte>(sizeof(int), (byte *) &i));
}

/*
const val getIntegerFromVal(byte b[]) const{
	int inputInteger = *reinterpret_cast<const int*>(b.data());
	return val(memory_view<byte>(sizeof(int), (byte *) &inputInteger));
}
*/

private:
	int i;
};

#endif