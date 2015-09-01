#include "../main/cpp/MultiQuadTuple.h"
#include <iostream>
#include <time.h>
#include <assert.h>

using namespace std;

#define N 128

void testPartialEval() {
	MultiQuadTuple<2*N,N> f;
    f.randomize();
    BitVector<N> x = BitVector<N>::randomVector();
    BitVector<N> y = BitVector<N>::randomVector();
    BitVector<2*N> z = BitVector<2*N>::vCat(x, y);

    //f(x || y)
    //g(y) = f(x || y)
    MultiQuadTuple<N, N> g = f.partialEval<N>(x);
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

