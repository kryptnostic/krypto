#include "FullyHomomorphicEncryption.h"

using namespace std;

#define N 1

void testAssignment(){
	BitMatrix<N> A = BitMatrix<N>::randomInvertibleMatrix(N<<6);
	A.set(0,0), A.set(1,1), A.set((N<<6)-1,(N<<6)-1);
	assert(A.get(0,0) && A.get(1,1) && A.get((N<<6)-1,(N<<6)-1));
	cout << "Passed assignment test" << endl;
}

void testRREF(){
	BitMatrix<N> I = BitMatrix<N>::squareIdentityMatrix();
	assert(I.rref().isIdentity());
	cout << "Passed RREF test" << endl;
}

void testSolve(){
	BitVector<N> v = BitVector<N>::randomVector();
	BitMatrix<N> M = BitMatrix<N>::randomInvertibleMatrix(N << 6);
	bool solvable;
	BitVector<N> x = M.solve(v);
	BitVector<N> mx = M*x;
	assert(mx.equals(v));
	cout << "Passed solve test" << endl;
}

void testInv(){
	BitMatrix<N> M = BitMatrix<N>::randomInvertibleMatrix(N << 6);
	BitMatrix<N> Mi = M.inv();
	BitMatrix<N> Il = M*Mi;
	BitMatrix<N> Ir = Mi*M;
	assert(Il.isIdentity());
	assert(Ir.isIdentity());
	BitVector<N> x = BitVector<N>::randomVector();
	BitVector<N> mix = Mi*x;
	assert(mix.equals(M.solve(x)));
	cout << "Passed inverse test" << endl;
}

void testFHE(){
	BitVector<N> m = BitVector<N>::randomVector();
	FullyHomomorphicEncryption<N> fhe;
	BitVector<(N<<1)> c = fhe.encrypt(m);
	BitVector<N> mt = fhe.decrypt(c);
	assert(m.equals(mt));
	cout << "Passed FHE test" << endl;
}

int main(){
	testAssignment();
	testSolve();
	testInv();
	testRREF();
	testFHE();
	return 0;
}

/*
TODO:
1) Speed test, against standard C++ bit library
2) Change proj in BitVector to something which is not copying 
*/