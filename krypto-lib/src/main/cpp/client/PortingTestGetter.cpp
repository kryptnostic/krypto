#ifndef krypto_PortingTest_cpp
#define krypto_PortingTest_cpp

#include "PortingTest.h"
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(crypto_module) {
	class_< PortingTest >("PortingTest")
		.constructor<unsigned char *>()
		.function("getInteger", &PortingTest::getInteger)
		//.function("getIntegerFromVal", &PortingTest::getIntegerFromVal)
		;
}
#endif