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
	_ls(bk.getLeftShiftMatrix()),
	_rs(bk.getRightShiftMatrix()),
	_lc(bk.getLeftColumnMatrix()),
	_rc(bk.getRightColumnMatrix()),
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

	//single left shift (if the leftmost bit of x is nonzero, it'll be zeroed)
	const BitVector<2*N> homomorphicLEFTSHIFT(const BitVector<2*N> &x) const{
		return homomorphicLMM(_ls, x);
	}

	//single right shift (if the rightmost bit of x is nonzero, it'll be zeroed)
	const BitVector<2*N> homomorphicRIGHTSHIFT(const BitVector<2*N> &x) const{
		return homomorphicLMM(_rs, x);
	}

	//add with carry over (aka integer addition in base 2)
	const BitVector<2*N> homomorphicADD(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
		BitVector<2*N> currentLHS = x;
		BitVector<2*N> currentRHS = y;
		BitVector<2*N> sum = homomorphicXOR(currentLHS, currentRHS);
		BitVector<2*N> carry = homomorphicAND(currentLHS, currentRHS);
		for(int i = 0; i < 2*N; ++i){
			currentLHS = sum;
			currentRHS = homomorphicLEFTSHIFT(carry);
			sum = homomorphicXOR(currentLHS, currentRHS);
			carry = homomorphicAND(currentLHS, currentRHS);
		}
		return sum;
	}

	//integer multiplication in base 2 (inputs should be < N bits)
	const BitVector<2*N> homomorphicMULT(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
		BitVector<2*N> shiftedX = x;
		BitVector<2*N> result = homomorphicAND(homomorphicLMM(_lc, shiftedX), y);
		for(int i = 1; i < N; ++i){
			result = homomorphicLEFTSHIFT(result);
			shiftedX = homomorphicLEFTSHIFT(shiftedX);
			result = homomorphicADD(result, homomorphicAND(homomorphicLMM(_lc, shiftedX), y));
		}
		return result;
	}
//*/
private:
	const BitMatrix<2*N, 4*N> _ls;
	const BitMatrix<2*N, 4*N> _rs;
	const BitMatrix<2*N, 4*N> _lc;
	const BitMatrix<2*N, 4*N> _rc;
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