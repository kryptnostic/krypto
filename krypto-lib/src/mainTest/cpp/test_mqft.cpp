#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/MultivariateQuadraticFunctionTuple.h"
#include <string>
using namespace testing;

#define N 1

TEST(MQFTTests, testRandom){
	MultivariateQuadraticFunctionTuple<1, 1> f1 = MultivariateQuadraticFunctionTuple<1, 1>::randomMultivariateQuadraticFunctionTuple();
	MultivariateQuadraticFunctionTuple<1, 1> f2 = MultivariateQuadraticFunctionTuple<1, 1>::randomMultivariateQuadraticFunctionTuple();
	MultivariateQuadraticFunctionTuple<1, 2> f = MultivariateQuadraticFunctionTuple<1, 2>::aug_v(f1, f2);
}

TEST(MQFTTests, testComposition){
	MultivariateQuadraticFunctionTuple<N, N> f = MultivariateQuadraticFunctionTuple<N, N>::randomMultivariateQuadraticFunctionTuple();
	BitMatrix<N> C = BitMatrix<N>::randomInvertibleMatrix();
	MultivariateQuadraticFunctionTuple<N, N> g = f*C; //this is incredibly slow
}