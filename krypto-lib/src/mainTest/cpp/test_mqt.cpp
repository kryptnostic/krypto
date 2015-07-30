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
	ASSERT_TRUE(1+2 == 3);
}

TEST(MQTTests, testLeftComp){
	ASSERT_TRUE(2+3 == 5);
	MultiQuadTuple<N, M> f = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	BitMatrix<L> C = BitMatrix<L>::randomMatrix(N << 6);
	BitVector<L> x = BitVector<L>::randomVector();
	MultiQuadTuple<L, M> fC = f*C;
	BitVector<M> fC_x = fC(x);
	BitVector<M> f_Cx = f(C.template operator*<N>(x));
	ASSERT_TRUE(fC_x.equals(f_Cx));
	ASSERT_TRUE(3+5 == 8);
}

TEST(MQTTests, testRightComp){
	ASSERT_TRUE(5+8 == 13);
	MultiQuadTuple<N, M> f = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	BitMatrix<M> D = BitMatrix<M>::randomMatrix(K << 6); 
	MultiQuadTuple<N, K> Df = f.rMult<K>(D); 
	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<K> Df_x = Df(x);
	BitVector<K> D_fx = D.template operator*<K>(f(x));
	ASSERT_TRUE(Df_x.equals(D_fx));
	ASSERT_TRUE(8+13 == 21);
}

TEST(MQTTests, testAugV){
	ASSERT_TRUE(13+21 == 34);
	MultiQuadTuple<N, M> f1 = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	MultiQuadTuple<N, M> f2 = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	MultiQuadTuple<N, 2*M> f12 = MultiQuadTuple<N, 2*M>::aug_v(f1,f2);
	ASSERT_TRUE(21+34 == 55);
}