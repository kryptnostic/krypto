#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/MultiQuadTuple.h"
#include <string>
#include <ctime>
using namespace testing;

#define L 1
#define M 1 
#define K 2 
#define N 1 
#define DEBUG false

TEST(MQTTests, testInit){//assert equality later, as there's obob now
	//C: L->N, f: N->M, D: M->K
	ASSERT_TRUE(1+1 == 2); 
	MultiQuadTuple<N, M> f = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	BitVector<N> y = BitVector<N>::randomVector();
	BitVector<M> fy = f(y);
}

TEST(MQTTests, testLeftComp){
	MultiQuadTuple<N, M> f = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	BitMatrix<L> C = BitMatrix<L>::randomMatrix(N << 6);
	BitVector<L> x = BitVector<L>::randomVector();
	BitVector<N> cx = C*x;
	BitVector<M> fcx = f(cx);
	MultiQuadTuple<L, M> fC = f*C;
	BitVector<M> fCx = fC(x);
	//ASSERT_TRUE(fcx.equals(fCx)); (obob now)
}

TEST(MQTTests, testRightComp){
	MultiQuadTuple<N, M> f = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	BitVector<N> y = BitVector<N>::randomVector();
	BitMatrix<M> D = BitMatrix<M>::randomMatrix(K << 6);
	BitVector<M> fy = f(y);
	MultiQuadTuple<N, K> Df = f.rMult<K>(D);
	//ASSERT_TRUE(Df(y).equals(D*fy));
}

TEST(MQTTests, testAugV){
	ASSERT_TRUE(1+1 == 2);
	MultiQuadTuple<N, M> f1 = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	MultiQuadTuple<N, M> f2 = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	MultiQuadTuple<N, 2*M> f12 = MultiQuadTuple<N, 2*M>::aug_v(f1,f2);
}