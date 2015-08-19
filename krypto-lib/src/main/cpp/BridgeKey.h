//
//  BridgeKey.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 7/22/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  Implementation of bridge key generation
//	Accessed by clients, used to generate components of the public keys (Refer to implementation.pdf for the notations)
//

#ifndef krypto_BridgeKey_h
#define krypto_BridgeKey_h

#include "PrivateKey.h"

/*
 * Template for BridgeKey
 * Plaintext Length: 64N, Length of the obfuscation chain: L
 * L === 2 for now
 */
template<unsigned int N, unsigned int L>
class BridgeKey{
public:

	/*
     * Constructor
     * Constructs a BridgeKey with a given PrivateKey
     * and BitMatrix K (for left-matrix multiplcation)
     */
	BridgeKey(const PrivateKey<N,L> &pk) :
	_pk(pk),
	_R(BitMatrix<N>::randomInvertibleMatrix()),
	_Rx(BitMatrix<N>::randomInvertibleMatrix()),
	_Ry(BitMatrix<N>::randomInvertibleMatrix()),
	_M(pk.getM()),
	_Ai(pk.getA().inv()),
	_Bi(pk.getB().inv()),
	_Cu1(pk.getUnaryObfChain()[0]),
	_Cu2(pk.getUnaryObfChain()[1]),
	_Cb1(pk.getBinaryObfChain()[0]),
	_Cb2(pk.getBinaryObfChain()[1]),
	_ARAi(pk.getA() * _R * pk.getA().inv()),
	_ARxAi(pk.getA() * _Rx * pk.getA().inv()),
	_ARyAi(pk.getA() * _Ry * pk.getA().inv()),
	_AiM2(_Ai.template pMult<2*N>(_M.inv(), N, 2*N-1))
	{}

/* Unary unified code */

	/*
	 * Function: getUnaryG1
	 * Returns the first function of the obfuscation chain for unary operations (e.g. left matrix multiplication)
	 */
	const MultiQuadTuple<2*N, 2*N> getUnaryG1() const{
		MultiQuadTupleChain<N,L> f = _pk.getf();

		BitMatrix<N, 2*N> matTop = _AiM2;
		BitMatrix<N, 2*N> matBot = _R * matTop;

		MultiQuadTuple<2*N, N> top = f.get(0) * matTop;
		MultiQuadTuple<2*N, N> bot = f.get(0) * matBot;
		MultiQuadTuple<2*N, 2*N> aug = MultiQuadTuple<2*N, 2*N>::augV(top, bot);
		return aug.template rMult<2*N>(_Cu1);
	}

	/*
	 * Function: getUnaryG2
	 * Returns the second function of the obfuscation chain for unary operations (e.g. left matrix multiplication)
	 */
	const MultiQuadTuple<2*N, 2*N> getUnaryG2() const{
		MultiQuadTupleChain<N,L> f = _pk.getf();

		BitMatrix<N, 2*N> matTop = _Cu1.inv().splitV2(0);
		BitMatrix<N, 2*N> matBot = _Cu1.inv().splitV2(1);

		MultiQuadTuple<2*N, N> top = f.get(1) * matTop;
		MultiQuadTuple<2*N, N> bot = f.get(1) * matBot;
		MultiQuadTuple<2*N, 2*N> aug = MultiQuadTuple<2*N, 2*N>::augV(top, bot);
		return aug.template rMult<2*N>(_Cu2);
	}

/* Left Matrix Multiplication */

	/*
	 * Function: getLMMZ
	 * Returns matrix Z used for homomorphic left matrix multiplication
	 * Dimension of Z: 2*(N * 2^6) by 4*(N * 2^6)
	 */
	const BitMatrix<2*N, 4*N> getLMMZ(const BitMatrix<N> & K) const{
		BitMatrix<N> zeroN = BitMatrix<N>::zeroMatrix();
		BitMatrix<N> RAi = _R * _Ai;

		BitMatrix<N> _BKBi = _pk.getB() * K * _Bi;
		BitMatrix<N> _BKBiAi = _BKBi * _Ai;

		BitMatrix<N, 2*N> XTop = BitMatrix<N, 2*N>::augH(_BKBi, _BKBiAi ^ RAi);
		BitMatrix<N, 2*N> XBot = BitMatrix<N, 2*N>::augH(zeroN, _ARAi);
		BitMatrix<2*N> X = _M * BitMatrix<2*N>::augV(XTop, XBot) * _M.inv();

		BitMatrix<N, 2*N> YTop = BitMatrix<N, 2*N>::augH(_BKBi, BitMatrix<N>::identityMatrix());
		BitMatrix<N, 2*N> YBot = BitMatrix<N, 2*N>::augH(zeroN, zeroN);
		BitMatrix<2*N> Y = _M * BitMatrix<2*N>::augV(YTop, YBot) * _Cu2.inv();
		return BitMatrix<4*N>::augH(X, Y);
	}

/* Binary unified code */

	/*
	 * Function: getBinaryG1
	 * Returns the first function of the obfuscation chain for binary operations (e.g. XOR, AND)
	 */
	const MultiQuadTuple<4*N, 3*N> getBinaryG1() const{
		MultiQuadTupleChain<N,L> f = _pk.getf();

		BitMatrix<N, 2*N> M2 = _M.inv().splitV2(1);
		BitMatrix<N, 4*N> M2ProjX = BitMatrix<N, 4*N>::augH(M2, BitMatrix<N, 2*N>::zeroMatrix());
		BitMatrix<N, 4*N> M2ProjY = BitMatrix<N, 4*N>::augH(BitMatrix<N, 2*N>::zeroMatrix(), M2);

		BitMatrix<N, 4*N> matTop = _Ai * M2ProjX;
		BitMatrix<N, 4*N> matMid = _Ai * M2ProjY;
		BitMatrix<N, 4*N> matBotX = _Rx * matTop;
		BitMatrix<N, 4*N> matBotY = _Ry * matMid;

		MultiQuadTuple<4*N, N> top = f.get(0) * matTop;
		MultiQuadTuple<4*N, N> mid = f.get(0) * matMid;
		MultiQuadTuple<4*N, N> bot = f.get(0) * (matBotX ^ matBotY);
		MultiQuadTuple<4*N, 3*N> aug = MultiQuadTuple<4*N, 3*N>::augV(top, mid, bot);
		return aug.template rMult<3*N>(_Cb1);
	}

	/*
	 * Function: getBinaryG2
	 * Returns the second function (g2) of the obfuscation chain for binary operations (e.g. XOR, AND)
	 */
	const MultiQuadTuple<3*N, 3*N>getBinaryG2() const{
		MultiQuadTupleChain<N,L> f = _pk.getf();

		BitMatrix<3*N> Cb1Inv = _Cb1.inv();
		BitMatrix<N, 3*N> matTop = Cb1Inv.splitV3(0);
		BitMatrix<N, 3*N> matMid = Cb1Inv.splitV3(1);
		BitMatrix<N, 3*N> matBot = Cb1Inv.splitV3(2);

		MultiQuadTuple<3*N, N> top = f.get(1) * matTop;
		MultiQuadTuple<3*N, N> mid = f.get(1) * matMid;
		MultiQuadTuple<3*N, N> bot = f.get(1) * matBot;
		MultiQuadTuple<3*N, 3*N> aug = MultiQuadTuple<3*N, 3*N>::augV(top, mid, bot);
		return aug.template rMult<3*N>(_Cb2);
	}


/* XOR */

	/*
	 * Function: getXORXx
	 * Returns matrix Xx used for homomorphic XOR
	 */
	const BitMatrix<2*N> getXORXx() const{
		BitMatrix<N> idN = BitMatrix<N>::identityMatrix();

		BitMatrix<2*N> XTop = BitMatrix<2*N>::augH(idN, (idN ^ _Rx) * _Ai);
		BitMatrix<2*N> XBot = BitMatrix<2*N>::augH(BitMatrix<N>::zeroMatrix(), _ARxAi);
		return _M * BitMatrix<2*N>::augV(XTop, XBot) * _M.inv();
	}

	/*
	 * Function: getXORXy
	 * Returns matrix Xy used for homomorphic XOR
	 */
	const BitMatrix<2*N> getXORXy() const{
		BitMatrix<N> idN = BitMatrix<N>::identityMatrix();

		BitMatrix<N, 2*N> XTop = BitMatrix<N, 2*N>::augH(idN, (idN ^ _Ry) * _Ai);
		BitMatrix<N, 2*N> XBot = BitMatrix<N, 2*N>::augH(BitMatrix<N>::zeroMatrix(), _ARyAi);
		return _M * BitMatrix<2*N>::augV(XTop, XBot) * _M.inv();
	}

	/*
	 * Function: getXORY
	 * Returns matrix Y used for homomorphic XOR
	 */
	const BitMatrix<2*N, 3*N> getXORY() const{
		BitMatrix<N> idN = BitMatrix<N>::identityMatrix();

		BitMatrix<N, 3*N> YTop = BitMatrix<N, 3*N>::augH(idN, idN, idN);
		return _M * BitMatrix<2*N, 3*N>::augV(YTop, BitMatrix<N, 3*N>::zeroMatrix(N)) * _Cb2.inv();
	}

/* AND */

	/*
	 * Function: getANDz
	 * Returns function tuple z used for homomorphic AND
	 */
	const MultiQuadTuple<7*N, 2*N> getANDz() const{
		BitMatrix<N, 2*N> X = getANDX();
		BitMatrix<N, 3*N> Y1 = _Bi.pMult(_Cb2.inv(), 0, N-1);
		BitMatrix<N, 3*N> Y2 = _Bi.pMult(_Cb2.inv(), N, 2*N-1);
		BitMatrix<N, 3*N> Y3 = _Cb2.inv().splitV3(2);
		BitMatrix<N, 7*N> Y3t = BitMatrix<7*N>::augH(BitMatrix<4*N>::zeroMatrix(N << 6), Y3);

		const unsigned int N_SUM = (N*(12*N + 1) + N*(8*N + 1) + ((3*N*(3*N + 1)) >> 1));
		BitMatrix<N_SUM, N> contrib = BitMatrix<N_SUM, N>::augV(getANDP(X, Y2), getANDQ(X, Y1), getANDS(Y1, Y2));
		MultiQuadTuple<7*N, N> zTop(contrib, BitVector<N>::zeroVector());
		zTop = zTop.template rMult<N>(_pk.getB());
		zTop = zTop ^ MultiQuadTuple<7*N, N>::getMultiQuadTuple(Y3t);
		MultiQuadTuple<7*N, N> zeroMQT = MultiQuadTuple<7*N, N>::zeroMultiQuadTuple();

		MultiQuadTuple<7*N, 2*N> z = MultiQuadTuple<7*N, 2*N>::augV(zTop, zeroMQT);
		return z.template rMult<2*N>(_M);
	}

	/*
	 * Function: getANDZ1
	 * Returns matrix Z1 used for homomorphic AND
	 */
	const BitMatrix<2*N> getANDZ1() const{
		BitMatrix<N, 2*N> top = _Rx * _AiM2;
		BitMatrix<N, 2*N> bottom = _pk.getA() * top;
		return _M * BitMatrix<2*N>::augV(top, bottom);
	}

	/*
	 * Function: getANDZ2
	 * Returns matrix Z2 used for homomorphic AND
	 * Dimension of Z2: 2*(N * 2^6) by 2*(N * 2^6)
	 */
	const BitMatrix<2*N> getANDZ2() const{
		BitMatrix<N, 2*N> top = _Ry * _AiM2;
		BitMatrix<N, 2*N> bottom = _pk.getA() * top;
		return _M * BitMatrix<2*N>::augV(top, bottom);
	}


private:
	const PrivateKey<N,L> _pk;
	const BitMatrix<N> _R;
	const BitMatrix<N> _Rx;
	const BitMatrix<N> _Ry;
	const BitMatrix<2*N> _M;
	const BitMatrix<2*N> _Cu1;
	const BitMatrix<2*N> _Cu2;
	const BitMatrix<3*N> _Cb1;
	const BitMatrix<3*N> _Cb2;
	const BitMatrix<N> _ARAi;
	const BitMatrix<N> _ARxAi;
	const BitMatrix<N> _ARyAi;

	const BitMatrix<N> _Ai;
	const BitMatrix<N> _Bi;
	const BitMatrix<2*N> _AiM2;
	static const unsigned int twoN = N << 1;
	static const unsigned int threeN = 3 * N;

/*Helper functions for getANDz*/

	/*
	 * Function: getANDX
	 * Returns matrix X used to compute z for homomorphic AND
	 * Dimension of X: (N * 2^6) by 2*(N * 2^6)
	 */
	const BitMatrix<2*N> getANDX() const{
		BitMatrix<2*N> inner = BitMatrix<2*N>::augH(BitMatrix<N>::identityMatrix(), _Ai);
		return _Bi * inner * _M.inv();
	}

	/*
	 * Function: getANDPk
	 * top chunk of contrib matrix for z for homomorphic AND
	 * level ranges from 0 to 64 * 2N - 1
	 * Dimension: 7 * 64N - level
	 */
	const BitMatrix<N> getANDPk(const int level, const BitMatrix<2*N> &X, const BitMatrix<3*N> &Y2) const{
		BitMatrix<N> top = BitMatrix<N>::zeroMatrix(twoN - level); //not sure if there's obob here

		BitMatrix<N> mid = BitMatrix<N>::zeroMatrix(twoN);
		for (unsigned int j = 0; j < N; ++j) { //col within middle block
			if(X.get(j, level)){
				for (unsigned int i = 0; i < twoN; ++i) { //row within middle block
					mid.set(i, j, X.get(j, i));
				}
			}
		}

		BitMatrix<N> bot = BitMatrix<N>::zeroMatrix(threeN);
		for (int j = 0; j < N; ++j) { //col within bottom block
			if(X.get(j, level)){
				for (unsigned int i = 0; i < threeN; ++i) { //row within bottom block
					bot.set(i, j, Y2.get(j, i));
				}
			}
		}
		return BitMatrix<N>::augV(top, mid, bot);
	}

	/*
	 * Function: getANDQk
	 * middle chunk of contrib matrix for z
	 * level ranges from 0 to 64 * 2N - 1
	 * Dimension: 5 * 64N - level
	 */
	const BitMatrix<N> getANDQk(const int level, const BitMatrix<2*N>&X, const BitMatrix<3*N> &Y1) const{
		BitMatrix<N> top = BitMatrix<N>::zeroMatrix(twoN - level);
		BitMatrix<N> bot = BitMatrix<N>::zeroMatrix(threeN);
		for (unsigned int j = 0; j < N; ++j) { //col within bottom block
			if(X.get(j, level)){
				for (unsigned int i = 0; i < threeN; ++i) { //row within bottom block
					bot.set(i, j, Y1.get(j, i));
				}
			}
		}
		return BitMatrix<N>::augV(top, bot);
	}

	/*
	 * Function: getANDSk
	 * level ranges from 0 to 64 * 3N - 1
	 * Dimension: 3 * 64N - level
	 */
	const BitMatrix<N> getANDSk(const int level, const BitMatrix<3*N> &Y1, const BitMatrix<3*N> &Y2) const{
		BitMatrix<N> contrib = BitMatrix<N>::zeroMatrix(threeN - level);
		for (unsigned int j = 0; j < N; ++j) { //cols
			bool prod = Y1.get(j, level) && Y2.get(j, level); //first row
			contrib.set(0, j, prod);
			if(Y1.get(j, level)){
				for (unsigned int i = 1; i < threeN - level; ++i) { //rows
					bool prod1 = Y2.get(j, level + i);
					contrib.set(i, j, Y2.get(j, level + i));
				}
			}
			if(Y2.get(j, level)){
				for(unsigned int i = 1; i < threeN - level; ++i){
					contrib.set(i, j, contrib.get(i, j) ^ Y1.get(j, level + i));
				}
			}
		}
		return contrib;
	}

	/*
	 * Function: getANDP (entire chunk)
	 * top chunk of contrib matrix for z for homomorphic AND
	 */
	const BitMatrix<N> getANDP(const BitMatrix<2*N> &X, const BitMatrix<3*N> &Y2) const{
		BitMatrix<N> contrib = BitMatrix<N>::zeroMatrix(N*(12*N + 1));
		unsigned int count = 0;
		unsigned int toAdd = 7 * N;
		for(unsigned int level = 0; level < twoN; ++level){
			contrib.setSubMatrix(count, getANDPk(level, X, Y2));
			count += toAdd;
			--toAdd;
		}
		return contrib;
	}

	/*
	 * Function: getANDQ (entire chunk)
	 * middle chunk of contrib matrix for z for homomorphic AND
	 */
	const BitMatrix<N> getANDQ(const BitMatrix<2*N> &X, const BitMatrix<3*N> &Y1) const{
		BitMatrix<N> contrib = BitMatrix<N>::zeroMatrix(N*(8*N + 1));
		unsigned int count = 0;
		unsigned int toAdd = 5 * N;
		for(unsigned int level = 0; level < twoN; ++level){
			contrib.setSubMatrix(count, getANDQk(level, X, Y1));
			count += toAdd;
			--toAdd;
		}
		return contrib;
	}

	/*
	 * Function: getANDS (entire chunk)
	 * bottom chunk of contrib matrix for z
	 */
	const BitMatrix<N> getANDS(const BitMatrix<3*N> &Y1, const BitMatrix<3*N> &Y2) const{
		BitMatrix<N> contrib = BitMatrix<N>::zeroMatrix((3*N*(3*N + 1)) >> 1);
		unsigned int count = 0;
		unsigned int toAdd = 3 * N;
		for(unsigned int level = 0; level < threeN; ++level){
			contrib.setSubMatrix(count, getANDSk(level, Y1, Y2));
			count += toAdd;
			--toAdd;
		}
		return contrib;
	}
};

#endif