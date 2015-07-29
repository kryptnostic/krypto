#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/MultiQuadTuple.h"
#include <string>
using namespace testing;

#define N 1

TEST(MQFTTests, testRandom){
	MultiQuadTuple<1, 1> f1 = MultiQuadTuple<1, 1>::randomMultiQuadTuple();
	MultiQuadTuple<1, 1> f2 = MultiQuadTuple<1, 1>::randomMultiQuadTuple();
	MultiQuadTuple<1, 2> f = MultiQuadTuple<1, 2>::aug_v(f1, f2);
}

TEST(MQFTTests, testComposition){
	MultiQuadTuple<N, N> f = MultiQuadTuple<N, N>::randomMultiQuadTuple();
	BitMatrix<N> C = BitMatrix<N>::randomInvertibleMatrix();
	// MultiQuadTuple<N, N> g = f*C; //this is incredibly slow
}