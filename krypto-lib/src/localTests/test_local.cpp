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

	BitVector<N> Cx = C.template operator*<N>(x);
	BitVector<M> f_Cx = f(Cx);

	MultiQuadTuple<L, M> fC = f*C;
	BitVector<M> fC_x = fC(x);

	cout << "LEFT COMPOSE TEST:" << endl << endl;

	cout << "f (C*x) = ";
	f_Cx.print();
	cout << endl;

	cout << "f*C (x) = ";
	fC_x.print();
	cout << endl;

	cout << "x = ";
	x.print();
	cout << endl;

	cout << "---------------------------------" << endl;
}

void testRightCompose(){ //f:N->M, D:M->K
	MultiQuadTuple<N, M> f = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	BitMatrix<M> D = BitMatrix<M>::randomMatrix(K << 6); 
	BitVector<N> x = BitVector<N>::randomVector();

	MultiQuadTuple<N, K> Df = f.rMult<K>(D); 
	BitVector<K> Df_x = Df(x);

	BitVector<M> fx = f(x);
	BitVector<K> D_fx = D.template operator*<K>(fx);

	cout << "RIGHT COMPOSE TEST:" << endl << endl;

	cout << "D * f(x) = ";
	Df_x.print();
	cout << endl;

	cout << "D*f (x) = ";
	D_fx.print();
	cout << endl;

	cout << "x = ";
	x.print();
	cout << endl;

	cout << "---------------------------------" << endl;
}

int main(int argc, char **argv) {
	testLeftCompose();
	testRightCompose();
	return 0;
}