#include "../main/cpp/MultiQuadTuple.h"
#include "../main/cpp/BridgeKey.h"
#include "../main/cpp/PublicKey.h"
#include <iostream>
#include <time.h>

using namespace std;

#define L 1
#define M 1 
#define K 2 
#define N 1 
#define DEBUG false

void testLeftCompose(MultiQuadTuple<N, M> &f, BitVector<N> &x){ //C:L->N, f:N->M
	BitMatrix<L> C = BitMatrix<L>::randomMatrix(N << 6);

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

void testRightCompose(MultiQuadTuple<N, M> &f, BitVector<N> &x){ //f:N->M, D:M->K
	BitMatrix<M> D = BitMatrix<M>::randomMatrix(K << 6); 

	MultiQuadTuple<N, K> Df = f.rMult<K>(D); 
	BitVector<K> Df_x = Df(x);

	BitVector<M> fx = f(x);
	BitVector<K> D_fx = D.template operator*<K>(fx);

	cout << "RIGHT COMPOSE TEST:" << endl << endl;

	cout << "D * f(x) = ";
	D_fx.print();
	cout << endl;

	cout << "D*f(x) = ";
	Df_x.print();
	cout << endl;

	cout << "x = ";
	x.print();
	cout << endl;

	cout << "---------------------------------" << endl;
}

void testBridgeKeyInstantiation(PrivateKey<N, 2> &pk) {
	BridgeKey<N, 2*N> bk(pk, BitMatrix<N>::squareIdentityMatrix());

	MultiQuadTuple<2*N, 2*N> u_g1 = bk.get_UNARY_g1();
	MultiQuadTuple<2*N, 2*N> u_g2 = bk.get_UNARY_g2();

	BitMatrix<4*N> Z = bk.get_LMM_Z();

	MultiQuadTuple<2*N, 3*N> b_gx1 = bk.get_BINARY_gx1();
	MultiQuadTuple<2*N, 3*N> b_gy1 = bk.get_BINARY_gy1();
	MultiQuadTuple<3*N, 3*N> b_g2 = bk.get_BINARY_g2();

	BitMatrix<2*N> Xx = bk.get_XOR_Xx();
	BitMatrix<2*N> Xy = bk.get_XOR_Xy();
	BitMatrix<3*N> Y = bk.get_XOR_Y();

	BitMatrix<2*N> Z1 = bk.get_AND_Z1();
	BitMatrix<2*N> Z2 = bk.get_AND_Z2();
	BitMatrix<N> z = bk.get_AND_z();
}

void testPublicKey(PrivateKey<N, 2> &pk) {
	BridgeKey<N, 2> bk(pk, BitMatrix<N>::squareIdentityMatrix());
	PublicKey<N, 2> pub(bk);

	BitVector<N> x; //zero vector
	BitVector<2*N> encrypted = pk.encrypt(x);
	BitVector<2*N> encryptedLMM = pub.homomorphicLMM(encrypted);
	BitVector<N> unencryptedLMM = pk.decrypt(encryptedLMM); //should be zero
}

int main(int argc, char **argv) {
	clock_t begin = clock();

	// MultiQuadTuple<N, M> f = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	// BitVector<N> x = BitVector<N>::randomVector();
	// testLeftCompose(f, x);
	// testRightCompose(f, x);

	PrivateKey<N, 2> pk;

 	//testBridgeKeyInstantiation(pk);
	testPublicKey(pk);

 	clock_t end = clock();
 	cout << "Time elapsed: " << double(end - begin) / CLOCKS_PER_SEC << " sec" << endl;
	return 0;
}