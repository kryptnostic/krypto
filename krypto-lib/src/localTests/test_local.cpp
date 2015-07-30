#include "../main/cpp/MultiQuadTuple.h"
#include <iostream>
using namespace std;
//using namespace testing;

#define L 1
#define M 1 
#define K 2 
#define N 1 
#define DEBUG false

int main(int argc, char **argv) {

	BitMatrix<N> I = BitMatrix<N>::squareIdentityMatrix();
	BitMatrix<N> It = I.T<N>();


	MultiQuadTuple<N, M> f = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	// BitVector<N> y = BitVector<N>::randomVector();
	// BitVector<M> fy = f(y);

	BitMatrix<L> C = BitMatrix<L>::randomMatrix(N << 6);
	BitVector<L> x = BitVector<L>::randomVector();
	BitVector<N> cx = C*x;
	BitVector<M> fcx = f(cx);
	MultiQuadTuple<L, M> fC = f*C;
	BitVector<M> fCx = fC(x);

	fcx.print();
	fCx.print();

	BitMatrix<M> D = BitMatrix<M>::randomMatrix(K << 6);
	MultiQuadTuple<N, K> Df = f.rMult<K>(D);

	cout << "ASDfasd" << endl;

	return 0;
}

// TEST(MQTTests, testInit){//assert equality later, as there's obob now
// 	//C: L->N, f: N->M, D: M->K
// 	ASSERT_TRUE(1+1 == 2); 
// 	MultiQuadTuple<N, M> f = MultiQuadTuple<N, M>::randomMultiQuadTuple();
// 	BitVector<N> y = BitVector<N>::randomVector();
// 	BitVector<M> fy = f(y);

// 	BitMatrix<L> C = BitMatrix<L>::randomMatrix(N << 6);
// 	BitVector<L> x = BitVector<L>::randomVector();
// 	BitVector<N> cx = C*x;
// 	BitVector<M> fcx = f(cx);
// 	MultiQuadTuple<L, M> fC = f*C;
// 	BitVector<M> fCx = fC(x);

// 	BitMatrix<M> D = BitMatrix<M>::randomMatrix(K << 6);
// 	MultiQuadTuple<N, K> Df = f.rMult<K>(D);
// }

// TEST(MQTTests, testAugV){
// 	ASSERT_TRUE(1+1 == 2);
// 	MultiQuadTuple<N, M> f1 = MultiQuadTuple<N, M>::randomMultiQuadTuple();
// 	MultiQuadTuple<N, M> f2 = MultiQuadTuple<N, M>::randomMultiQuadTuple();
// 	MultiQuadTuple<N, 2*M> f12 = MultiQuadTuple<N, 2*M>::aug_v(f1,f2);
// }