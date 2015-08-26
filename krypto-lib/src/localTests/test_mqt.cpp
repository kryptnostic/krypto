#include "../main/cpp/MultiQuadTuple.h"
#include <iostream>
#include <time.h>
#include <assert.h>

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
    cout << "Partial Eval g(y) =   ";
    g(y).print();
    cout << "Normal Eval f(x, y) = ";
    f(z).print();

    assert(g(y) == f(z));
    //ASSERT_TRUE(g(y).equals(f(z)));


}

int main(int argc, char **argv) {
	testPartialEval();
	return 0;
}

