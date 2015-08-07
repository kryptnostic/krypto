#ifndef krypto_EncryptedSearchPublicKey_h
#define krypto_EncryptedSearchPublicKey_h

#include "PublicKey.h"
#include "MultiQuadTuple.h"
#include "BitMatrix.h"

using namespace std;

template<unsigned int N, unsigned int L>
class EncryptedSearchPublicKey{
public:
	EncryptedSearchPublicKey(PrivateKey<N, 2> &pk) : 
	_pk(pk){}

	//untested, TODO: test
	const BitVector<2*N> homomorphicHash(MultiQuadTuple<2*N, N> &h, BitVector<2*N> &t, BitVector<2*N> &d) const{
		PrivateKey<N, 2> pk;
		BridgeKey<N, 2> bk(pk);
		PublicKey<N, 2> pub(bk);
		BitVector<2*N> result = pk.encrypt(BitVector<N>::zeroVector()); 
		unsigned int count = 0;
		for(size_t i = 0; i < twoNN; ++i){
			for(size_t j = i; j < twoNN; ++j){
				BitMatrix<N> Aij = BitMatrix<N>::squareZeroMatrix();
				Aij.setCol(i, h.getTransposedContributionMatrix().getRow(count));
				BitMatrix<N> Jj = BitMatrix<N>::squareZeroMatrix();
				Jj.setCol(j, 1);
				BitVector<2*N> x_1;
				BitVector<2*N> x_2;
				if(i < NN){
					if(j < NN){ //t_{i}t_{j}
						x_1 = pub.homomorphicLMM(Aij, t);
						x_2 = pub.homomorphicLMM(Jj, t);
					} else { //t_{i}d_{j-NN}
						x_1 = pub.homomorphicLMM(Aij, t);
						x_2 = pub.homomorphicLMM(Jj, d);
					}
				} else { //d_{i-NN}d_{j-NN}
					x_1 = pub.homomorphicLMM(Aij, d);
					x_2 = pub.homomorphicLMM(Jj, d);	
					
				}	
				BitVector<2*N> andV = pub.homomorphicAND(x_1, x_2);
				result = pub.homomorphicXOR(result, andV);
				++count;
			}
		}
		return result;
	}

private:
	PrivateKey<N, L> _pk;
	static const unsigned int NN = N << 6;
	static const unsigned int twoNN = NN << 1;
};

#endif/* defined(__krypto__EncryptedSearchPublicKey__) */