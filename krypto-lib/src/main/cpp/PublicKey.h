/*Accessed by server, used to generate components of the public keys*/
#ifndef krypto_PublicKey_h
#define krypto_PublicKey_h

#include <assert.h>
#include "BridgeKey.h"
#include "MultiQuadTuple.h"
#include "BitMatrix.h"

using namespace std;

//L will be known by the server anyway (as we pass {f_1',...,f_L'} to it)
template<unsigned int N, unsigned int L>
class PublicKey{
public:
	PublicKey(BridgeKey<N,L> bk) : 
	_bk(bk),
	_gu1(bk.get_UNARY_g1()),
	_gu2(bk.get_UNARY_g2()),
	_Z(bk.get_LMM_Z()),
	_gbx1(bk.get_BINARY_gx1()),
	_gby1(bk.get_BINARY_gy1()),
	_gb2(bk.get_BINARY_g2()),
	_Xx(bk.get_XOR_Xx()),
	_Xy(bk.get_XOR_Xy()),
	_Y(bk.get_XOR_Y()),
	_Z1(bk.get_AND_Z1()),
	_Z2(bk.get_AND_Z2())
	{
		MultiQuadTuple<7*N, N> _z(_bk.get_AND_z());
	}

	const BitVector<2*N> homomorphicLMM(BitVector<2*N> x) const{
		BitVector<2*N> t = gu2(gu1(x));
		BitVector<4*N> inner = BitVector<4*N>::vcat2(x, t);

		return _Z * inner;
	}

	const BitVector<2*N> homomorphicXOR(BitVector<2*N> x, BitVector<2*N> y) const{
		BitVector<3*N> t = calculateT(x, y);
		return _Xx * x ^ _Xy * y ^ _Y * t;
	}

	const BitVector<2*N> homomorphicAND(BitVector<2*N> x, BitVector<2*N> y) const{
		BitVector<3*N> t = calculateT(x, y);
		BitVector<7*N> coordinates = BitVector<7*N>::vcat3(x, y, t);

		BitVector<N> zv = BitVector<N>::zeroVector();
		BitVector<2*N> left = BitVector<2*N>::vcat2(_z(coordinates), zv);
		BitVector<2*N> mid = _Z1 * x;
		BitVector<2*N> right = _Z2 * y;

		return left ^ mid ^ right;
	}

private:
	BridgeKey<N, L> _bk;
	MultiQuadTuple<2*N, 2*N> _gu1;
	MultiQuadTuple<2*N, 2*N> _gu2;
	BitMatrix<4*N> _Z;
	MultiQuadTuple<2*N, 3*N> _gbx1;
	MultiQuadTuple<2*N, 3*N> _gby1;
	MultiQuadTuple<3*N, 3*N> _gb2;
	BitMatrix<2*N> _Xx;
	BitMatrix<2*N> _Xy;
	BitMatrix<3*N> _Y;
	MultiQuadTuple<7*N, N> _z;
	BitMatrix<2*N> _Z1;
	BitMatrix<2*N> _Z2;

	const BitVector<3*N> calculateT(BitVector<2*N> x, BitVector<2*N> y) const{
		return gb2(gbx1(x) ^ gby1(x));
	}
};

#endif