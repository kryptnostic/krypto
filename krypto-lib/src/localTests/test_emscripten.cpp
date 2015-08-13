#include <iostream>
#include "test_BridgeKey.h"

using namespace std;

#define N 2

void testOps1() {
    BridgeKey<N, 2> bk;

    MultiQuadTuple<7*N, 2*N> _z = bk.getANDz();
    // BitVector<6> zero = BitVector<6>::zeroVector();
    // zero.print();

    clock_t diff = 0;
    cout << double(diff) << endl;
}

int main(int argc, char **argv) {

    for (int i = 0; i < 2; ++i) testOps1();
    // BitVector<6> zero = BitVector<6>::zeroVector();
    // zero.print();

    return 0;
}