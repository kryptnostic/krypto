#ifndef __krypto__FullyHomomorphicEncryption__
#define __krypto__FullyHomomorphicEncryption__

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../src/main/cpp/MultivariatePolynomialFunction.h"
#include "../src/main/cpp/BitMatrix.h"

using namespace std;

template<unsigned int N>
class FullyHomomorphicEncryption {
public:
	FullyHomomorphicEncryption(): 
		_r(BitVector<N>::randomVector()),
		_A(BitMatrix<N>::randomInvertibleMatrix(N<<6)),
		_B(BitMatrix<N>::randomInvertibleMatrix(N<<6)),
		_f1(MultivariatePolynomialFunction<N,N>::denseRandomMultivariateFunctions()),
		_f2(MultivariatePolynomialFunction<N,N>::denseRandomMultivariateFunctions()){
	}

	/**
	 * Input: plaintext in F^N
	 * Output: ciphertext in F^{2N}
	 */
	const BitVector<(N<<1)> encrypt(const BitVector<N> & x){
		BitVector<N> fr = _f2(_f1(_r));
		BitVector<N> Bx = _B*x;
		BitVector<N> top = Bx ^ (_r ^ fr);
		BitVector<N> bottom = _A*_r;
		return top.vcat(bottom);
	}
	
	/**
	 * Input: ciphertext in F^{2N}
	 * Output: plaintext in F^N
	 */
	const BitVector<N> decrypt(const BitVector<2*N> & x){
		BitMatrix<N> Ai = _A.inv();
		BitMatrix<N> Bi = _B.inv();
		BitVector<N> x1, x2;
		x.proj2(x1, x2);
		BitVector<N> Aix2 = _A.solve(x2); 
		BitVector<N> fAix2 = _f2(_f1(Aix2));
		return _B.solve(x1 ^ (Aix2 ^ fAix2)); 
	}

private:
	BitMatrix<N> _A;	
	BitMatrix<N> _B;
	BitVector<N> _r; //random vector
	MultivariatePolynomialFunction<N,N> _f1;
	MultivariatePolynomialFunction<N,N> _f2;
	
	//vector<BitMatrix<(N<<1)>> C; //chain of obfuscation matrix
	//BitMatrix R; //used in the ciphertext space
	//vector<MultivariatePolynomialFunction<N,N>> _f(_l); //length: l (the length of the obfuscation chain)

};

#endif /* defined(__krypto__FullyHomomorphicEncryption__) */
