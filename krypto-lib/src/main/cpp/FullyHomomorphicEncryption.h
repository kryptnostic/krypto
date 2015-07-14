/**
 * FullyHomomorphicEncryption.h
 * krypto
 * Created by: HPH
 * Copyright: Kryptnostic
 */

#ifndef __krypto__FullyHomomorphicEncryption__
#define __krypto__FullyHomomorphicEncryption__

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "MultivariatePolynomialFunction.h"
#include "BitMatrix.h"

using namespace std;

template<unsigned int N>
class FullyHomomorphicEncryption {
public:
	FullyHomomorphicEncryption(): 
		_A(BitMatrix<N>::randomInvertibleMatrix(N<<6)),
		_B(BitMatrix<N>::randomInvertibleMatrix(N<<6)),
		_f1(MultivariatePolynomialFunction<N,N>::denseRandomMultivariateFunctions()),
		_f2(MultivariatePolynomialFunction<N,N>::denseRandomMultivariateFunctions()),
		_C1u(BitMatrix<2*N>::randomInvertibleMatrix((2*N)<<6)), //_Ciu for unary
		_C2u(BitMatrix<2*N>::randomInvertibleMatrix((2*N)<<6)), 
		_C1b(BitMatrix<3*N>::randomInvertibleMatrix((3*N)<<6)), //_Cib for binary
		_C2b(BitMatrix<3*N>::randomInvertibleMatrix((3*N)<<6)){
	}

	/**
	 * Input: plaintext in F^N
	 * Output: ciphertext in F^{2N}
	 */
	const BitVector<2*N> encrypt(const BitVector<N> & x, const BitVector<N> & r) {
		BitVector<N> fr = _f2(_f1(r));
		BitVector<N> Bx = _B*x;
		BitVector<N> top = Bx ^ (r ^ fr);
		BitVector<N> bottom = _A*r;
		BitVector<2*N> result = BitVector<N>::vcat2(top, bottom);
		return BitVector<N>::vcat2(top, bottom);
	}
	
	/**
	 * Input: ciphertext in F^{2N}
	 * Output: plaintext in F^N
	 */
	const BitVector<N> decrypt(const BitVector<2*N> & x) {
		BitVector<N> x1, x2;
		x.proj2(x1, x2);
		BitVector<N> Aix2 = _A.solve(x2);
		BitVector<N> fAix2 = _f2(_f1(Aix2));
		return _B.solve(x1 ^ (Aix2 ^ fAix2)); 
	}

	const BitVector<N> randUnary(const BitMatrix<N> & R, const BitVector<2*N> & x){
		BitVector<N> x1, x2;
		x.proj2(x1, x2);	
		BitVector<N> Aix2 = _A.solve(x2);	
		return R * Aix2;
	}

	const BitVector<N> randBinary(const BitMatrix<N> & Rx, const BitMatrix<N> & Ry, BitVector<2*N> & cx, BitVector<2*N> & cy){
		BitVector<N> cx1, cx2, cy1, cy2;
		cx.proj2(cx1, cx2);
		cy.proj2(cy1, cy2);
		return Rx*_A.solve(cx2) ^ Ry*_A.solve(cy2);
	}

	//TODO: output obfuscation chain
	const BitVector<2*N> Obfuscate_Unary(const BitVector<2*N> & x, const BitMatrix<N> & R){ 
		BitVector<N> x1, x2;
		x.proj2(x1, x2);
		BitVector<N> Aix2 = _A.solve(x2); 
		BitVector<N> fAix2 = _f2(_f1(Aix2));
		BitVector<N> RAix2 = R*Aix2;
		BitVector<N> fRAix2 = _f2(_f1(RAix2));
		BitVector<2*N> v = BitVector<N>::vcat2(fAix2, fRAix2);
		return _C2u * v;		
	}

	const BitVector<3*N> Obfuscate_Binary(const BitVector<2*N> & x, const BitVector<2*N> & y, BitMatrix<N> & Rx, BitMatrix<N> & Ry){
		BitVector<N> x1, x2, y1, y2;
		x.proj2(x1, x2);
		y.proj2(y1, y2);
		BitVector<N> Aix2 = _A.solve(x2);
		BitVector<N> Aiy2 = _A.solve(y2);		
		BitVector<N> f1Aix2 = _f1(Aix2);
		BitVector<N> f1Aiy2 = _f1(Aiy2);
		BitVector<N> w = _f1(Rx*Aix2 ^ Ry*Aiy2);
		BitVector<3*N> fp1x = _C1b * BitVector<N>::vcat3(f1Aix2, f1Aiy2, w);
		BitVector<N> v1, v2, v3;
		BitVector<3*N> C1ifp1x = _C1b.solve(fp1x);
		C1ifp1x.proj3(v1, v2, v3); 
		BitVector<N> f2v1 = _f2(v1), f2v2 = _f2(v2), f2v3 = _f2(v3);
		return _C2b * BitVector<N>::vcat3(f2v1, f2v2, f2v3);
	}

	const BitVector<2*N> MMult_Hom(const BitMatrix<N> & K, const BitVector<2*N> & x, const BitMatrix<N> & R){
		BitVector<2*N> t = Obfuscate_Unary(x, R);
		BitVector<N> x1, x2;
		x.proj2(x1, x2);
		BitVector<2*N> C2it = _C2u.solve(t);
		BitVector<N> C2it1, C2it2;
		C2it.proj2(C2it1, C2it2);
		BitVector<N> Aix2 = _A.solve(x2); 
		const BitVector<N> RAix2 = R*Aix2;
		BitVector<N> top = (_B*(K*_B.solve((x1 ^ Aix2) ^ C2it1)) ^ C2it2) ^ RAix2;
		BitVector<N> bottom = _A*RAix2;
		return BitVector<N>::vcat2(top, bottom);
	}

	const BitVector<2*N> Xor_Hom(const BitVector<2*N> & x, const BitVector<2*N> & y, BitMatrix<N> & Rx, BitMatrix<N> & Ry){
		BitVector<3*N> t = Obfuscate_Binary(x, y, Rx, Ry);
		BitVector<2*N> s = x ^ y;
		BitVector<N> s1, s2;
		s.proj2(s1, s2);
		BitVector<N> Ais2 = _A.solve(s2);
        BitVector<3*N> C2it = _C2b.solve(t);
		BitVector<N> C2it1, C2it2, C2it3;		
		C2it.proj3(C2it1, C2it2, C2it3);
		BitVector<N> t1 = s1 ^ Ais2;
		BitVector<N> t2 = (C2it1 ^ C2it2) ^ C2it3;
		BitVector<N> x1, x2, y1, y2;
		x.proj2(x1, x2);
		y.proj2(y1, y2);
		BitVector<N> Aix2 = _A.solve(x2);
		BitVector<N> Aiy2 = _A.solve(y2);
		BitVector<N> t3 = (Rx * Aix2) ^ (Ry * Aiy2);
		BitVector<N> top = (t1 ^ t2) ^ t3;
		BitVector<N> RxAix2 = Rx * Aix2;
		BitVector<N> RyAiy2 = Ry * Aiy2;
		BitVector<N> bottom = _A*(RxAix2 ^ RyAiy2);
		return BitVector<N>::vcat2(top, bottom);
	}

	const BitVector<2*N> And_Hom(const BitVector<2*N> & x, const BitVector<2*N> & y, BitMatrix<N> & Rx, BitMatrix<N> & Ry){
		BitVector<3*N> t = Obfuscate_Binary(x, y, Rx, Ry);
		BitVector<N> x1, x2, y1, y2;
		x.proj2(x1, x2);
		y.proj2(y1, y2);
		BitVector<N> Aix2 = _A.solve(x2);
		BitVector<N> Aiy2 = _A.solve(y2);
		BitVector<3*N> C2it = _C2b.solve(t);
		BitVector<N> C2it1, C2it2, C2it3;		
		C2it.proj3(C2it1, C2it2, C2it3);
		BitVector<N> w1 = _B.solve((x1 ^ Aix2) ^ C2it1);
		BitVector<N> w2 = _B.solve((y1 ^ Aiy2) ^ C2it2);
		BitVector<N> RxAix2 = Rx * Aix2;
		BitVector<N> RyAiy2 = Ry * Aiy2;
		BitVector<N> top = ((_B * (w1 & w2)) ^ C2it3) ^ (RxAix2 ^ RyAiy2);
		BitVector<N> bottom = _A*(RxAix2 ^ RyAiy2);		
		return BitVector<N>::vcat2(top, bottom);
	}

private:
	BitMatrix<N> _A;	
	BitMatrix<N> _B;
	MultivariatePolynomialFunction<N,N> _f1;
	MultivariatePolynomialFunction<N,N> _f2;
	BitMatrix<2*N> _C1u;
	BitMatrix<2*N> _C2u;
	BitMatrix<3*N> _C1b;
	BitMatrix<3*N> _C2b;	
};

#endif /* defined(__krypto__FullyHomomorphicEncryption__) */
