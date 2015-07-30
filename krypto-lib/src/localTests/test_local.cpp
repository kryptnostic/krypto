#include "../main/cpp/MultiQuadTuple.h"
#include <iostream>
using namespace std;

#define L 1
#define M 1 
#define K 2 
#define N 1 
#define DEBUG false

void testLeftCompose(){ //C:L->N, f:N->M
	MultiQuadTuple<N, M> f = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	BitMatrix<L> C = BitMatrix<L>::randomMatrix(N << 6);
	BitVector<L> x = BitVector<L>::randomVector();
	MultiQuadTuple<L, M> fC = f*C;
	BitVector<M> fC_x = fC(x);
	BitVector<M> f_Cx = f(C.template operator*<N>(x));
	fC_x.print();
	f_Cx.print();
}

void testRightCompose(){ //f:N->M, D:M->K
	MultiQuadTuple<N, M> f = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	BitMatrix<M> D = BitMatrix<M>::randomMatrix(K << 6); 
	MultiQuadTuple<N, K> Df = f.rMult<K>(D); 
	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<K> Df_x = Df(x);
	BitVector<K> D_fx = D.template operator*<K>(f(x));
	Df_x.print();
	D_fx.print();
}

int main(int argc, char **argv) {
	testLeftCompose();
	testRightCompose();
	return 0;
}