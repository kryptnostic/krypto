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
	PublicKey(const BridgeKey<N,L> &bk) : 
	_bk(bk),
	_gu1(bk.getUnaryG1()),
	_gu2(bk.getUnaryG2()),
	_gb1(bk.getBinaryG1()),
	_gb2(bk.getBinaryG2()),
	_Xx(bk.getXORXx()),
	_Xy(bk.getXORXy()),
	_Y(bk.getXORY()),
	_Z1(bk.getANDZ1()),
	_Z2(bk.getANDZ2()),
	_z(bk.getANDz())
	{
	}

	const BitVector<2*N> homomorphicLMM(const BitMatrix<N> & K, const BitVector<2*N> &x) const{
		BitMatrix<4*N> _Z = _bk.getLMMZ(K);
		BitVector<2*N> t = _gu2(_gu1(x));
		//BitVector<4*N> inner = BitVector<4*N>::template vCat2<2*N, 2*N>(x, t);
		BitVector<4*N> inner = BitVector<4*N>::template vCat<2*N, 2*N>(x, t);

		return _Z.template operator*<2*N>(inner);
	}

	const BitVector<2*N> homomorphicXOR(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
		BitVector<3*N> t = binaryT(x, y);
		return _Xx.template operator*<2*N>(x) ^ _Xy.template operator*<2*N>(y) ^ _Y.template operator*<2*N>(t);
	}

	const BitVector<2*N> homomorphicAND(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
		BitVector<3*N> t = binaryT(x, y);
		//BitVector<7*N> coordinates = BitVector<7*N>::vCat3(x, y, t);
		BitVector<7*N> coordinates = BitVector<7*N>::vCat(x, y, t);
		
		BitVector<2*N> left = _z(coordinates);
		BitVector<2*N> mid = _Z1.template operator*<2*N>(x);
		BitVector<2*N> right = _Z2.template operator*<2*N>(y);
		
		return left ^ mid ^ right;
	}


private:
	BridgeKey<N, L> _bk;
	MultiQuadTuple<2*N, 2*N> _gu1;
	MultiQuadTuple<2*N, 2*N> _gu2;
	MultiQuadTuple<4*N, 3*N> _gb1;
	MultiQuadTuple<3*N, 3*N> _gb2;
	BitMatrix<2*N> _Xx;
	BitMatrix<2*N> _Xy;
	BitMatrix<3*N> _Y;
	MultiQuadTuple<7*N, 2*N> _z;
	BitMatrix<2*N> _Z1;
	BitMatrix<2*N> _Z2;

	unsigned int NN = N << 6;

	const BitVector<3*N> binaryT(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
		//BitVector<4*N> concatXY = BitVector<4*N>::template vCat2<2*N, 2*N>(x, y);
		BitVector<4*N> concatXY = BitVector<4*N>::template vCat<2*N, 2*N>(x, y);
		return _gb2(_gb1(concatXY));
	}
};

#endif