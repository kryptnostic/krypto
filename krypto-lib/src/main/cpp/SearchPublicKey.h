#ifndef krypto_SearchPublicKey_h
#define krypto_SearchPublicKey_h

#include "PublicKey.h"
#include "SearchPrivateKey.h"

template<unsigned int N>
class SearchPublicKey{
public:

	SearchPublicKey(const SearchPrivateKey<N> & rk):
	_rk(rk)
	{}

	/*
	 * Function: homomorphicHash
	 * Returns H[h](E(t), E(d)), should be made a private function
	 * COMMENTED OUT FOR NOW AS SOME THE CALL TO getTransposedContributionMatrix should be changed
	 */
<<<<<<< HEAD
	const BitVector<2*N> homomorphicHash() const{
		BitVector<2*N> result;
		return BitVector<2*N>::zeroVector();
	}

	/*
	 * Function: getAddress(t, d)
	 * Returns the raw address of token t in document d (if available)
	 */
	const BitVector<N> getAddress(const BitVector<2*N> & t, const BitVector<2*N> & d) const{
		return BitVector<N>::zeroVector(); //TODO
	}

private:
	SearchPrivateKey<N> _rk;
};

#endif/* defined(__krypto__SearchPublicKey__) */