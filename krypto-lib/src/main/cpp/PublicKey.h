//
//  PublicKey.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 7/23/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  Implementation of public key generation
//	Accessed by servers
//

#ifndef krypto_PublicKey_h
#define krypto_PublicKey_h

#include "BridgeKey.h"
#include "MultiQuadTuple.h"
#include "BitMatrix.h"

using namespace std;

//L will be known by the server anyway (as we pass {f_1',...,f_L'} to it)
template<unsigned int N, unsigned int L>
class PublicKey{
public:
	PublicKey(BridgeKey<N,L> &bk) : 
	_bk(bk),
	//_gu1(bk.get_UNARY_g1()),
	//_gu2(bk.get_UNARY_g2()),
	//_Z(bk.get_LMM_Z()),
	_gb1(bk.get_BINARY_g1()),
	_gb2(bk.get_BINARY_g2()),
	_Xx(bk.get_XOR_Xx()),
	_Xy(bk.get_XOR_Xy()),
	_Y(bk.get_XOR_Y()),
	_Z1(bk.get_AND_Z1()),
	_Z2(bk.get_AND_Z2()),
	_z(bk.get_AND_z())
	{
	}

	const BitVector<2*N> homomorphicLMM(BitMatrix<N> & K, BitVector<2*N> &x) const{
		MultiQuadTuple<2*N, 2*N> _gu1 = _bk.get_UNARY_g1();
		MultiQuadTuple<2*N, 2*N> _gu2 = _bk.get_UNARY_g2();
		BitMatrix<4*N> _Z = _bk.get_LMM_Z(K);
		BitVector<2*N> t = _gu2(_gu1(x));
		BitVector<4*N> inner = BitVector<4*N>::vcat(x, t);

		return _Z.template operator*<2*N>(inner);
	}

	const BitVector<2*N> homomorphicXOR(BitVector<2*N> &x, BitVector<2*N> &y) const{
		BitVector<3*N> t = calculateT(x, y);
		return _Xx.template operator*<2*N>(x) ^ _Xy.template operator*<2*N>(y) ^ _Y.template operator*<2*N>(t);
	}

	const BitVector<2*N> homomorphicAND(BitVector<2*N> &x, BitVector<2*N> &y) const{
		BitVector<3*N> t = calculateT(x, y);
		BitVector<7*N> coordinates = BitVector<7*N>::vcat(x, y, t);
		
		BitVector<2*N> left = _z(coordinates);
		BitVector<2*N> mid = _Z1.template operator*<2*N>(x);
		BitVector<2*N> right = _Z2.template operator*<2*N>(y);
		
		return left ^ mid ^ right;
	}


private:
	BridgeKey<N, L> _bk;
	//MultiQuadTuple<2*N, 2*N> _gu1;
	//MultiQuadTuple<2*N, 2*N> _gu2;
	//BitMatrix<4*N> _Z;
	MultiQuadTuple<4*N, 3*N> _gb1;
	MultiQuadTuple<3*N, 3*N> _gb2;
	BitMatrix<2*N> _Xx;
	BitMatrix<2*N> _Xy;
	BitMatrix<3*N> _Y;
	MultiQuadTuple<7*N, 2*N> _z;
	BitMatrix<2*N> _Z1;
	BitMatrix<2*N> _Z2;

	unsigned int NN = N << 6;

	const BitVector<3*N> calculateT(BitVector<2*N> &x, BitVector<2*N> &y) const{
		BitVector<4*N> concatXY = BitVector<4*N>::vcat(x, y);
		return _gb2(_gb1(concatXY));
	}
};

#endif