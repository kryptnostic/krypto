#ifndef krypto_EncryptedSearchPrivateKey_h
#define krypto_EncryptedSearchPrivateKey_h

#include "PrivateKey.h"

template<unsigned int N, unsigned int L>
class EncryptedSearchPrivateKey{
public:
	EncryptedSearchPrivateKey(const PrivateKey<N, L> & pk, const MultiQuadTuple<2*N,N> & h) : 
	_pk(pk), _h(h), _s(pk.encrypt(BitVector<N>::zeroVector())), _v(pk.encrypt(h.getConstantTerms())){
		generateSearchObfuscationFunctionChain();
	}

	const BitVector<2*N> getS() const{
		return _s;
	}

	const BitVector<2*N> getV() const{
		return _v;
	}

	const MultiQuadTuple<2*N,N> getHash() const{
		return _h;
	}

private:
	PrivateKey<N, L> _pk;
	MultiQuadTuple<2*N,N> _h;
	BitVector<2*N> _s; //obfuscation vector to use during search 
	BitVector<2*N> _v; //obfuscation vector to use during search
	static const unsigned int NN = N << 6;
	static const unsigned int twoNN = NN << 1;
	void generateSearchObfuscationFunctionChain(){ 
		MultiQuadTuple<N,N> g = MultiQuadTuple<N,N>::randomMultiQuadTuple();
		BitMatrix<N> Cs1 = BitMatrix<N>::randomInvertibleMatrix(); 
		BitMatrix<N> Cs2 = BitMatrix<N>::randomInvertibleMatrix();
		MultiQuadTuple<2*N,N> g0 = _h.template rMult<N>(Cs1);
		MultiQuadTuple<N,N> g1 = (_pk.getf().get(0).template rMult<N>(Cs2)) * Cs1.inv();
		//MultiQuadTuple<N,N> g2 = (_pk.getf().get(1) ^ g) * Cs2.inv();
	}
};

#endif/* defined(__krypto__EncryptedSearchPrivateKey__) */