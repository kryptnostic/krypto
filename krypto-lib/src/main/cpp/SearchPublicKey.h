#ifndef krypto_SearchPublicKey_h
#define krypto_SearchPublicKey_h

#include "PublicKey.h"
#include "SearchPrivateKey.h"

template<unsigned int N>
class SearchPublicKey{
public:

	SearchPublicKey(const SearchPrivateKey<N> & rk, const BridgeKey<N> & bk):
	_rk(rk), _bk(bk),
	_h(rk.getHash()),
	_g0(rk.getG0()),
	_g1(rk.getG1()),
	_g2(rk.getG2())
	{}

	/*
	 * Function: homomorphicHash
	 * Returns H[h](E(t), E(d)), should be made a private function
	 */
	const BitVector<2*N> homomorphicHash(const BitVector<2*N> & t, const BitVector<2*N> & d) const{
		PublicKey<N> pub(_bk);
		BitVector<2*N> result = _rk.getS();
		unsigned int count = 0;
		BitMatrix<N> C = _h.getTransposedContributionMatrix();
		BitMatrix<N> Ai = BitMatrix<N>::squareZeroMatrix();
		BitMatrix<N> Ji = BitMatrix<N>::squareZeroMatrix();
		for(size_t i = 0; i < N; ++i){
			Ai.zero();
			Ji.zero();
			Ji.template setCol<N>(i);
			for(size_t j = i; j < N; ++j){
				Ai.template setCol<N>(j, C.getRow(count));
				++count;
			}
			BitVector<2*N> v1 = pub.homomorphicLMM(_bk.getLMMZ(Ai), t);
			for(size_t j = 0; j < N; ++j){
				Ai.template setCol<N>(j, C.getRow(count));
				++count;
			}
			BitVector<2*N> v2 = pub.homomorphicLMM(_bk.getLMMZ(Ai), d);
			result = pub.homomorphicXOR(result, pub.homomorphicAND(pub.homomorphicLMM(_bk.getLMMZ(Ji), t), pub.homomorphicXOR(v1, v2)));
		}
		for(size_t i = 0; i < N; ++i){
			Ai.zero();
			Ji.zero();
			Ji.template setCol<N>(i);
			for(size_t j = i; j < N; ++j){
				Ai.template setCol<N>(j, C.getRow(count));
				++count;
			}
			result = pub.homomorphicXOR(result, pub.homomorphicAND(pub.homomorphicLMM(_bk.getLMMZ(Ji), d), pub.homomorphicLMM(_bk.getLMMZ(Ai), d)));
		}
		result = pub.homomorphicXOR(result, _rk.getV());
		return result;
	}

	/*
	 * Function: getAddress(t, d)
	 * Returns the raw address of token t in document d (if available)
	 */
	const BitVector<N> getAddress(const BitVector<2*N> & t, const BitVector<2*N> & d) const{
		BitVector<2*N> E_f = homomorphicHash(t, d);
		//TODO: compute E_g from g_2,g_1,g_0
		//TODO: compute result using D_g and E_g
		//test commit
		return BitVector<N>::zeroVector(); //TODO
	}

private:
	BridgeKey<N> _bk;
	SearchPrivateKey<N> _rk;
	MultiQuadTuple<2*N,N> _h;
	MultiQuadTuple<2*N,N> _g0;
	MultiQuadTuple<N,N> _g1;
	MultiQuadTuple<N,N> _g2;
};

#endif/* defined(__krypto__SearchPublicKey__) */