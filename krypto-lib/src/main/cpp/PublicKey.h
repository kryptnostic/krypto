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

#define DEBUG false

//L will be known by the server anyway (as we pass {f_1',...,f_L'} to it)
template<unsigned int N>
class PublicKey{
public:
	PublicKey(const BridgeKey<N> &bk) :
	_bk(bk),
	_gu1(bk.getUnaryG1()),
	_gu2(bk.getUnaryG2()),
	_gb1(bk.getBinaryG1()),
	_gb2(bk.getBinaryG2()),
	_Xx(bk.getXORXx()),
	_Xy(bk.getXORXy()),
	_Y(bk.getXORY()),
	_Z1(bk.getANDZ1()),
	_Z2(bk.getANDZ2())//,
	//_z(bk.getANDz())
	{
	}

	const BitVector<2*N> homomorphicLMM(const BitMatrix<2*N, 4*N> & Z, const BitVector<2*N> &x) const{
		BitVector<2*N> t = _gu2(_gu1(x));
		BitVector<4*N> inner = BitVector<4*N>::template vCat<2*N, 2*N>(x, t);

		return Z * inner;
	}

	const BitVector<2*N> homomorphicXOR(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
		BitVector<3*N> t = binaryT(x, y);
		return (_Xx * x) ^ (_Xy * y) ^ (_Y * t);
	}

	const BitVector<2*N> homomorphicAND(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
		BitVector<3*N> t = binaryT(x, y);
		BitVector<7*N> coordinates = BitVector<7*N>::vCat(x, y, t);
		cout << "It breaks here" << endl;
		MultiQuadTuple<7*N, 2*N> _z = _bk.getANDz();
		std::cout << "blah" << endl;
		BitVector<2*N> left = _z(coordinates);
		BitVector<2*N> mid = _Z1 * x;
		BitVector<2*N> right = _Z2 * y;

		return left ^ mid ^ right;
	}

	/*
	const BitVector<2*N> homomorphicADD(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
		//TODO
	}
	*/

private:
	const BridgeKey<N> _bk;
	const MultiQuadTuple<2*N, 2*N> _gu1;
	const MultiQuadTuple<2*N, 2*N> _gu2;
	const MultiQuadTuple<4*N, 3*N> _gb1;
	const MultiQuadTuple<3*N, 3*N> _gb2;
	const BitMatrix<2*N> _Xx;
	const BitMatrix<2*N> _Xy;
	const BitMatrix<2*N, 3*N> _Y;
	const BitMatrix<2*N> _Z1;
	const BitMatrix<2*N> _Z2;
	//MultiQuadTuple<7*N, 2*N> _z;

	const BitVector<3*N> binaryT(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
		BitVector<4*N> concatXY = BitVector<4*N>::template vCat<2*N, 2*N>(x, y);
		return _gb2(_gb1(concatXY));
	}
};

#endif