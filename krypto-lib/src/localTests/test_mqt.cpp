#include "../main/cpp/MultiQuadTuple.h"
#include <iostream>
#include <time.h>

using namespace std;

#define sma 64

void testPartialEval() {
	MultiQuadTuple<2*sma,sma> f;
    f.randomize();
    BitVector<sma> x = BitVector<sma>::randomVector();
    BitVector<sma> y = BitVector<sma>::randomVector();
    BitVector<2*sma> z = BitVector<2*sma>::vCat(x, y);

    //f(x || y)
    //g(y) = f(x || y)
    MultiQuadTuple<sma, sma> g = f.partialEval<sma>(x);
    g(y).print();
    f(z).print();
    //ASSERT_TRUE(g(y).equals(f(z)));	
}

int main(int argc, char **argv) {
	testPartialEval();
	return 0;
}

