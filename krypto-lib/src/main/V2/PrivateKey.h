//
//  PrivateKey.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 7/22/15.
//	Modified by njhlai
//  Copyright (c) 2016 Kryptnostic. All rights reserved.
//
//  Implementation of private key generation
//	Accessed by clients
//

#ifndef krypto_PrivateKey_h
#define krypto_PrivateKey_h

#include "MonomialMatrix.h"

/*
 * Template for newPrivateKey
 * Length of plaintext = N * 2^6, Length of obfuscation chain = L
 */
template<unsigned int N, unsigned int MON>
class PrivateKey {

template<unsigned int N1, unsigned int M1> friend class BridgeKey;
template<unsigned int N2> friend class ClientHashFunction;
template<unsigned int N3> friend class SearchPrivateKey;
template<unsigned int N4, unsigned int M4> friend class KryptnosticClient;

public:

	/*
     * Default constructor
     * Constructs a newPrivateKey with randomly initialized private variables
     */
	PrivateKey():
		_M(BitMatrix<2*N>::randomInvertibleMatrix()){
		generateObfuscationPolynomials();
	}

    /*
     * Function: encrypt(m)
     * Returns the encrypted ciphertext (length 2N * 2^6) of the plaintext (length N * 2^6)
     */
	const BitVector<2*N> encrypt(const BitVector<N> &m) const{//returns x = E(m, r) given a plaintext m
		const BitVector<N> & r = BitVector<N>::randomVector();
		const BitVector<N> & top = m ^ _f(r);
		const BitVector<N> & bottom = r;
		return _M * BitVector<N>::vCat(top, bottom);
	}

    /*
     * Function: decrypt(x)
     * Returns the decrypted plaintext (length N * 2^6) from a ciphertext (length 2N * 2^6)
     */
	const BitVector<N> decrypt(const BitVector<2*N> &x) const{//returns m = D(x) given a ciphertext x
		const BitVector<2*N> & mix = _M.solve(x);
		BitVector<N> x1, x2;
		mix.proj(x1, x2);
		const BitVector<N> & fx2 = _f(x2);
		return x1 ^ fx2;
	}

protected:
	const BitMatrix<2*N> getM() const{
		return _M;
	}

	const SecretPolynomial<N,N,MON> getf() const{
		return _f;
	}

	const BitMatrix<2*N> getUnaryObf1() const{
		return _Cu[0];
	}

	const BitMatrix<2*N> getUnaryObf2() const{
		return _Cu[1];
	}

	const BitMatrix<3*N> getBinaryObf1() const{
		return _Cb[0];
	}

	const BitMatrix<3*N> getBinaryObf2() const{
		return _Cb[1];
	}
private:
	const BitMatrix<2*N> _M; //SL_{2n}(F_2)
	SecretPolynomial<N,N,MON> _f; //random quadratic polynomial
	BitMatrix<2*N> _Cu[2]; //chain of obfuscation matrix for unary operations
	BitMatrix<3*N> _Cb[2]; //chain of obfuscation matrix for binary operations

	void generateObfuscationPolynomials(){ //generates _f, Gu, Gb, C_{u1},...,C_{uL} and C_{b1},...,C_{bL}
		_Cu[1] = BitMatrix<(2*N)>::generateSparseInvertibleMatrix(1);
		_Cb[1] = BitMatrix<(3*N)>::generateSparseInvertibleMatrix(1);
		
		const BitMatrix<N> & A = BitMatrix<N>::randomInvertibleMatrix();
		const BitMatrix<2*N> & Bu = (BitMatrix<2*N>::generateSparseInvertibleMatrix(2)).transpose()/* * _Cu[1].inv()*/;
		const BitMatrix<3*N> & Bb = (BitMatrix<3*N>::generateSparseInvertibleMatrix(2)).transpose()/* * _Cb[1].inv()*/;

		_f = SecretPolynomial<N,N,MON>::zero();
		_f.setLinearFactors(0, A);

		const BitMatrix<2*N> & stackAi = BitMatrix<2*N>::augV(
			BitMatrix<N,2*N>::augH(A.inv(), BitMatrix<N>::zeroMatrix()),
			BitMatrix<N,2*N>::augH(BitMatrix<N>::zeroMatrix(), A.inv()));
		_Cu[0] = (Bu * stackAi).inv().transpose();
		_Cb[0] = (Bb * BitMatrix<3*N>::augV(
			BitMatrix<2*N,3*N>::augH(stackAi, BitMatrix<2*N,N>::zeroMatrix()),
			BitMatrix<N,3*N>::augH(BitMatrix<N,2*N>::zeroMatrix(), A.inv()))).inv().transpose();

		for (int i = 0; i < MON; ++i)
		{
			const BitMatrix<N> & miniP = BitMatrix<N>::generateSparseInvertibleMatrix(1);
			_f.setLinearFactors(i, A * miniP);
		}

		// want a constant vector which is sparse
		// this is one way to do it since the non-constant part of f is a high degree homogeneous polynomial
		_f.setConstant(_f(BitVector<N>::randomVector()));
	}
};

#endif /* defined(__krypto__PrivateKey__) */