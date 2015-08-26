//
//  PrivateKey.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 7/22/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  Implementation of private key generation
//	Accessed by clients
//

#ifndef krypto_PrivateKey_h
#define krypto_PrivateKey_h

#include "MultiQuadTupleChain.h"

/*
 * Template for PrivateKey
 * Length of plaintext = N * 2^6, Length of obfuscation chain = L
 */
template<unsigned int N>
class PrivateKey {

template<unsigned int N1> friend class BridgeKey;
template<unsigned int N2> friend class ClientHashFunction;
template<unsigned int N3> friend class SearchPrivateKey;
template<unsigned int N4> friend class KryptnosticClient;

public:

	/*
     * Default constructor
     * Constructs a PrivateKey with randomly initialized private variables
     */
	PrivateKey():
		_A(BitMatrix<N>::randomInvertibleMatrix()),
		_B(BitMatrix<N>::randomInvertibleMatrix()),
		_M(BitMatrix<2*N>::randomInvertibleMatrix()),
		_f(MultiQuadTupleChain<N,2>::randomMultiQuadTupleChain()){
		generateObfuscationMatrixChains();
	}

    /*
     * Function: encrypt(m)
     * Returns the encrypted ciphertext (length 2N * 2^6) of the plaintext (length N * 2^6)
     */
	const BitVector<2*N> encrypt(const BitVector<N> &m) const{//returns x = E(m, r) given a plaintext m
		BitVector<N> r = BitVector<N>::randomVector();
		BitVector<N> top = (_B * m) ^ (r ^ _f(r));
		BitVector<N> bottom = _A * r;
		return _M * BitVector<N>::vCat(top, bottom);
	}

    /*
     * Function: decrypt(x)
     * Returns the decrypted plaintext (length N * 2^6) from a ciphertext (length 2N * 2^6)
     */
	const BitVector<N> decrypt(const BitVector<2*N> &x) const{//returns m = D(x) given a ciphertext x
		BitVector<2*N> mix = _M.solve(x);
		BitVector<N> x1, x2;
		mix.proj(x1, x2);
		BitVector<N> Aix2 = _A.solve(x2);
		BitVector<N> fAix2 = _f(Aix2);
		return _B.solve(x1 ^ (Aix2 ^ fAix2));
	}

protected:
	const BitMatrix<N> getA() const{
		return _A;
	}

	const BitMatrix<N> getB() const{
		return _B;
	}

	const BitMatrix<2*N> getM() const{
		return _M;
	}

	const MultiQuadTupleChain<N,2> getf() const{
		return _f;
	}

	const vector<BitMatrix<2*N> > getUnaryObfChain() const{
		return _Cu;
	}

	const vector<BitMatrix<3*N> > getBinaryObfChain() const{
		return _Cb;
	}

private:
	const BitMatrix<N> _A, _B; //SL_n(F_2)
	const BitMatrix<2*N> _M; //SL_{2n}(F_2)
	MultiQuadTupleChain<N,2> _f; //{f_1,...,f_L} random quadratic function tuples
	vector<BitMatrix<2*N> > _Cu; //chain of obfuscation matrix for unary operations
	vector<BitMatrix<3*N> > _Cb; //chain of obfuscation matrix for binary operations

	void generateObfuscationMatrixChains(){ //generates C_{u1},...,C_{uL} and C_{b1},...,C_{bL}
		for(unsigned int i = 0; i < 2; ++i){
			_Cu.push_back(BitMatrix<(2*N)>::randomInvertibleMatrix());
			_Cb.push_back(BitMatrix<(3*N)>::randomInvertibleMatrix());
		}
	}
};

#endif /* defined(__krypto__PrivateKey__) */