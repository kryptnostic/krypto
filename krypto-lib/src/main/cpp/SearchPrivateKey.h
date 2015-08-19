#ifndef krypto_SearchPrivateKey_h
#define krypto_SearchPrivateKey_h

#include "PrivateKey.h"

template<unsigned int N, unsigned int L>
class SearchPrivateKey{
public:
	SearchPrivateKey(const PrivateKey<N, L> & pk, const MultiQuadTuple<2*N,N> & h) :
	_pk(pk),
	_h(h),
	_s(pk.encrypt(BitVector<N>::zeroVector())),
	_v(pk.encrypt(h.getConstantTerms())),
	_g(MultiQuadTuple<N,N>::randomMultiQuadTuple()),
	_Cs1(BitMatrix<N>::randomInvertibleMatrix()),
	_Cs2(BitMatrix<N>::randomInvertibleMatrix()),
	_g0(_h.template rMult<N>(_Cs1)),
	_g1((_pk.getf().get(0).template rMult<N>(_Cs2)) * _Cs1.inv()),
	_g2((_pk.getf().get(1) ^ _g) * _Cs2.inv()),
	_K(generateK()),
	_C(BitMatrix<N>::randomInvertibleMatrix())
	{ }

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

	/*
     * Function: generateK()
     * Returns the random client-specific n x 2n matrix K_\Omega
     */
	const BitMatrix<2*N> getK() const{
		return _K;
	}

/* Decrypter for re-encryption */


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

	BitMatrix<2*N> _K;
	BitMatrix<N> _C;

	/*
     * Function: generateK()
     * Returns a random client-specific n x 2n matrix K_\Omega
     * with 0 bottom left and top right blocks
     * Assumes N is even
     */
	const BitMatrix<2*N> generateK() const{
		const unsigned int half = N >> 1;
		BitMatrix<half> K11 = BitMatrix<half>::randomInvertibleMatrix();
		BitMatrix<half> K12 = BitMatrix<half>::randomInvertibleMatrix();
		BitMatrix<half> K23 = BitMatrix<half>::randomInvertibleMatrix();
		BitMatrix<half> K24 = BitMatrix<half>::randomInvertibleMatrix();
		BitMatrix<N> zero = BitMatrix<N>::zeroMatrix(N << 5);

		BitMatrix<2*N> top = BitMatrix<2*N>::augH(BitMatrix<N>::augH(K11, K12), zero);
		BitMatrix<2*N> bot = BitMatrix<2*N>::augH(zero, BitMatrix<N>::augH(K23, K24));

		return BitMatrix<2*N>::augV(top, bot);
	}

	/*
     * Function: generateHashMatrix()
     * Returns the matrix portion of the hash function
     * Applied to x concatenated with y
     */
	const BitMatrix<2*N> generateHashMatrix() const{ //dimensions are off
		BitMatrix<2*N> Mi = _pk.getM().inv();
		BitMatrix<2*N> Mi1 = Mi.splitV(0);
		BitMatrix<2*N> Mi2 = Mi.splitV(1);

		BitMatrix<2*N> left = _pk.getB().inv() * Mi1;
		BitMatrix<2*N> right = _pk.getA().inv() * Mi2;
		BitMatrix<2*N> decryptMatrix = left ^ right;
		return _K * (BitMatrix<2*N>::augV(decryptMatrix, decryptMatrix));
	}

	/*
     * Function: generateAugmentedF2()
     * Returns the K (f2 C || f2 C) portion of the hash function
     * Applied to concealedF1(x) concatenated with concealedF1(y)
     */
	const MultiQuadTuple<2*N, N> generateAugmentedF2() const{ //dimensions are off
		MultiQuadTuple<N, N> f2 = _pk.getf().get(1);
		MultiQuadTuple<N, N> topBot = (f2 * _C);
		MultiQuadTuple<2*N, 2*N> augmentedDecrypt = MultiQuadTuple<2*N, 2*N>::augV(topBot, topBot);
		return augmentedDecrypt.rMult(_K);
	}

	/*
     * Function: generateConcealedF1()
     * Returns the f1 C portion of the hash function
     * Applied to x and y separately
     */
	const MultiQuadTuple<N, N> generateConcealedF1() const{ //dimensions are off
		MultiQuadTuple<N, N> f1 = _pk.getf().get(0);

		BitMatrix<2*N> Mi2 = _pk.getM().inv().splitV(1);
		BitMatrix<2*N> inner = _pk.getA().inv() * Mi2;
		return (f1 * inner).rMult(_C.inv());
	}
};

#endif/* defined(__krypto__SearchPrivateKey__) */