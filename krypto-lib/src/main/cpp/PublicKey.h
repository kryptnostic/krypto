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
	_gu1(bk.getUnaryG1()),
	_gu2(bk.getUnaryG2()),
	_gb1(bk.getBinaryG1()),
	_gb2(bk.getBinaryG2()),
	_XOR(bk.getXOR()),
	_AND(bk.getAND())
	{
	}

	const BitVector<2*N> homomorphicLMM(const BitMatrix<2*N, 4*N> & Z, const BitVector<2*N> &x) const{
		return Z * BitVector<4*N>::template vCat<2*N, 2*N>(x, _gu2(_gu1(x)));
	}

	const BitVector<2*N> homomorphicXOR(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
		return _XOR(x, y, binaryT(x, y));
	}

	const BitVector<2*N> homomorphicAND(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
		return _AND(x, y, binaryT(x, y));
	}

	/*
	const BitVector<2*N> homomorphicSHIFT(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
		//TODO
	}
	*/

	/*
	const BitVector<2*N> homomorphicADD(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
		//TODO
	}
	*/

private:
	const MultiQuadTuple<2*N, 2*N> _gu1;
	const MultiQuadTuple<2*N, 2*N> _gu2;
	const MultiQuadTuple<4*N, 3*N> _gb1;
	const MultiQuadTuple<3*N, 3*N> _gb2;
	const typename BridgeKey<N>::H_XOR _XOR;
	const typename BridgeKey<N>::H_AND _AND;

	const BitVector<3*N> binaryT(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
		const BitVector<4*N> & concatXY = BitVector<4*N>::template vCat<2*N, 2*N>(x, y);
		return _gb2(_gb1(concatXY));
	}
};

#endif