#ifndef krypto_SearchPrivateKey_h
#define krypto_SearchPrivateKey_h

#include "PrivateKey.h"

template<unsigned int N, unsigned int L>
class SearchPrivateKey{
public:
	SearchPrivateKey(const PrivateKey<N, L> & pk, const MultiQuadTuple<2*N,N> & h) : 
	_pk(pk), _h(h), 
	_s(pk.encrypt(BitVector<N>::zeroVector())), 
	_v(pk.encrypt(h.getConstantTerms())),
	_g(MultiQuadTuple<N,N>::randomMultiQuadTuple()),
	_Cs1(BitMatrix<N>::randomInvertibleMatrix()),
	_Cs2(BitMatrix<N>::randomInvertibleMatrix()),
	_g0(_h.template rMult<N>(_Cs1)),
	_g1((_pk.getf().get(0).template rMult<N>(_Cs2)) * _Cs1.inv()),
	_g2((_pk.getf().get(1) ^ _g) * _Cs2.inv())
	{}

/* Getters */

	const MultiQuadTuple<2*N,N> getG0() const{
		return _g0;
	}

	const MultiQuadTuple<N,N> getG1() const{
		return _g1;
	}

	const MultiQuadTuple<N,N> getG2() const{
		return _g2;
	}

	/*
     * Returns the global hash function given by the server.
	 */
	const MultiQuadTuple<2*N,N> getHash() const{
		return _h;
	}

	/*
	 * Returns the zero vector encrypted to obfuscate server during 
	 * address calculation. 
	 */	 
	const BitVector<2*N> getS() const{
		return _s;
	}

	/*
	 * Returns the constants vector of the global hash function encrypted to obfuscate server during
	 * address calculation.
	 */
	const BitVector<2*N> getV() const{
		return _v;
	}

private:
	PrivateKey<N, L> _pk;
	MultiQuadTuple<2*N,N> _h;
	BitVector<2*N> _s; //obfuscation vector to use during search 
	BitVector<2*N> _v; //obfuscation vector to use during search
	MultiQuadTuple<N,N> _g; //random function generated for zero knowledge re-encryption
	BitMatrix<N> _Cs1;
	BitMatrix<N> _Cs2;
	MultiQuadTuple<2*N,N> _g0;
	MultiQuadTuple<N,N> _g1;
	MultiQuadTuple<N,N> _g2;
	static const unsigned int NN = N << 6;
	static const unsigned int twoNN = NN << 1;
};

#endif/* defined(__krypto__SearchPrivateKey__) */