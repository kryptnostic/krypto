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
 * Plaintext Length: N
 */
template<unsigned int N>
class BridgeKey{
public:

	/*
     * Constructor
     * Constructs a BridgeKey with a given PrivateKey
     * and BitMatrix K (for left-matrix multiplcation)
     */
	BridgeKey(const PrivateKey<N> &pk) :
	_pk(pk),
	_R(BitMatrix<N>::randomInvertibleMatrix()),
	_Rx(BitMatrix<N>::randomInvertibleMatrix()),
	_Ry(BitMatrix<N>::randomInvertibleMatrix()),
	_M(pk.getM()),
	_Cu1(pk.getUnaryObf1()),
	_Cu2(pk.getUnaryObf2()),
	_Cb1(pk.getBinaryObf1()),
	_Cb2(pk.getBinaryObf2()),
	_Ai(pk.getA().inv()),
	_Bi(pk.getB().inv()),
	_ARAi(pk.getA() * _R * _Ai),
	_ARxAi(pk.getA() * _Rx * _Ai),
	_ARyAi(pk.getA() * _Ry * _Ai),
	_AiM2(_Ai.template pMult<2*N, 2*N>(_M.inv(), N))
	{}

/* Unary unified code */

	/*
	 * Function: getUnaryG1
	 * Returns the first function of the obfuscation chain for unary operations (e.g. left matrix multiplication)
	 */
	const MultiQuadTuple<2*N, 2*N> getUnaryG1() const{
		const MultiQuadTupleChain<N,2> & f = _pk.getf();

		const BitMatrix<N, 2*N> & matTop = _AiM2;
		const BitMatrix<N, 2*N> & matBot = _R * matTop;

		const MultiQuadTuple<2*N, N> & top = f.get(0) * matTop;
		const MultiQuadTuple<2*N, N> & bot = f.get(0) * matBot;
		MultiQuadTuple<2*N, 2*N> aug;// = MultiQuadTuple<2*N, 2*N>::augV(top, bot);
		aug.augV(top, bot);
		return aug.template rMult<2*N>(_Cu1);
	}

	/*
	 * Function: getUnaryG2
	 * Returns the second function of the obfuscation chain for unary operations (e.g. left matrix multiplication)
	 */
	const MultiQuadTuple<2*N, 2*N> getUnaryG2() const{
		const MultiQuadTupleChain<N,2> & f = _pk.getf();

		const BitMatrix<N, 2*N> & matTop = _Cu1.inv().splitV2(0);
		const BitMatrix<N, 2*N> & matBot = _Cu1.inv().splitV2(1);

		const MultiQuadTuple<2*N, N> & top = f.get(1) * matTop;
		const MultiQuadTuple<2*N, N> & bot = f.get(1) * matBot;
		MultiQuadTuple<2*N, 2*N> aug;// = MultiQuadTuple<2*N, 2*N>::augV(top, bot);
		aug.augV(top, bot);
		return aug.template rMult<2*N>(_Cu2);
	}

/* Left Matrix Multiplication */

	/*
	 * Function: getLMMZ
	 * Returns matrix Z used for homomorphic left matrix multiplication
	 * Dimension of Z: 2*(N * 2^6) by 4*(N * 2^6)
	 */
	const BitMatrix<2*N, 4*N> getLMMZ(const BitMatrix<N> & K) const{
		const BitMatrix<N> & zeroN = BitMatrix<N>::zeroMatrix();
		const BitMatrix<N> & RAi = _R * _Ai;

		const BitMatrix<N> & _BKBi = _pk.getB() * K * _Bi;
		const BitMatrix<N> & _BKBiAi = _BKBi * _Ai;

		const BitMatrix<N, 2*N> & XTop = BitMatrix<N, 2*N>::augH(_BKBi, _BKBiAi ^ RAi);
		const BitMatrix<N, 2*N> & XBot = BitMatrix<N, 2*N>::augH(zeroN, _ARAi);
		const BitMatrix<2*N> & X = _M * BitMatrix<2*N>::augV(XTop, XBot) * _M.inv();

		const BitMatrix<N, 2*N> & YTop = BitMatrix<N, 2*N>::augH(_BKBi, BitMatrix<N>::identityMatrix());
		const BitMatrix<N, 2*N> & YBot = BitMatrix<N, 2*N>::augH(zeroN, zeroN);
		const BitMatrix<2*N> & Y = _M * BitMatrix<2*N>::augV(YTop, YBot) * _Cu2.inv();
		return BitMatrix<2*N, 4*N>::augH(X, Y);
	}

/* Binary unified code */

	/*
	 * Function: getBinaryG1
	 * Returns the first function of the obfuscation chain for binary operations (e.g. XOR, AND)
	 */
	const MultiQuadTuple<4*N, 3*N> getBinaryG1() const{
		const MultiQuadTupleChain<N,2> & f = _pk.getf();

		const BitMatrix<N, 2*N> & M2 = _M.inv().splitV2(1);
		const BitMatrix<N, 4*N> & M2ProjX = BitMatrix<N, 4*N>::augH(M2, BitMatrix<N, 2*N>::zeroMatrix());
		const BitMatrix<N, 4*N> & M2ProjY = BitMatrix<N, 4*N>::augH(BitMatrix<N, 2*N>::zeroMatrix(), M2);

		const BitMatrix<N, 4*N> & matTop = _Ai * M2ProjX;
		const BitMatrix<N, 4*N> & matMid = _Ai * M2ProjY;
		const BitMatrix<N, 4*N> & matBotX = _Rx * matTop;
		const BitMatrix<N, 4*N> & matBotY = _Ry * matMid;

		const MultiQuadTuple<4*N, N> & top = f.get(0) * matTop;
		const MultiQuadTuple<4*N, N> & mid = f.get(0) * matMid;
		const MultiQuadTuple<4*N, N> & bot = f.get(0) * (matBotX ^ matBotY);
		MultiQuadTuple<4*N, 3*N> aug;// = MultiQuadTuple<4*N, 3*N>::augV(top, mid, bot);
		aug.augV(top, mid, bot);
		return aug.template rMult<3*N>(_Cb1);
	}

	/*
	 * Function: getBinaryG2
	 * Returns the second function (g2) of the obfuscation chain for binary operations (e.g. XOR, AND)
	 */
	const MultiQuadTuple<3*N, 3*N>getBinaryG2() const{
		const MultiQuadTupleChain<N,2> & f = _pk.getf();

		const BitMatrix<3*N> & Cb1Inv = _Cb1.inv();
		const BitMatrix<N, 3*N> & matTop = Cb1Inv.splitV3(0);
		const BitMatrix<N, 3*N> & matMid = Cb1Inv.splitV3(1);
		const BitMatrix<N, 3*N> & matBot = Cb1Inv.splitV3(2);

		const MultiQuadTuple<3*N, N> & top = f.get(1) * matTop;
		const MultiQuadTuple<3*N, N> & mid = f.get(1) * matMid;
		const MultiQuadTuple<3*N, N> & bot = f.get(1) * matBot;
		MultiQuadTuple<3*N, 3*N> aug;// = MultiQuadTuple<3*N, 3*N>::augV(top, mid, bot);
		aug.augV(top, mid, bot);
		return aug.template rMult<3*N>(_Cb2);
	}


/* XOR */

	/*
	 * Function: getXORXx
	 * Returns matrix Xx used for homomorphic XOR
	 */
	const BitMatrix<2*N> getXORXx() const{
		const BitMatrix<N> & idN = BitMatrix<N>::identityMatrix();

		const BitMatrix<N, 2*N> & XTop = BitMatrix<N, 2*N>::augH(idN, (idN ^ _Rx) * _Ai);
		const BitMatrix<N, 2*N> & XBot = BitMatrix<N, 2*N>::augH(BitMatrix<N>::zeroMatrix(), _ARxAi);
		return _M * BitMatrix<2*N>::augV(XTop, XBot) * _M.inv();
	}

	/*
	 * Function: getXORXy
	 * Returns matrix Xy used for homomorphic XOR
	 */
	const BitMatrix<2*N> getXORXy() const{
		const BitMatrix<N> idN = BitMatrix<N>::identityMatrix();

		const BitMatrix<N, 2*N> XTop = BitMatrix<N, 2*N>::augH(idN, (idN ^ _Ry) * _Ai);
		const BitMatrix<N, 2*N> XBot = BitMatrix<N, 2*N>::augH(BitMatrix<N>::zeroMatrix(), _ARyAi);
		return _M * BitMatrix<2*N>::augV(XTop, XBot) * _M.inv();
	}

	/*
	 * Function: getXORY
	 * Returns matrix Y used for homomorphic XOR
	 */
	const BitMatrix<2*N, 3*N> getXORY() const{
		const BitMatrix<N> & idN = BitMatrix<N>::identityMatrix();

		const BitMatrix<N, 3*N> & YTop = BitMatrix<N, 3*N>::augH(idN, idN, idN);
		return _M * BitMatrix<2*N, 3*N>::augV(YTop, BitMatrix<N, 3*N>::zeroMatrix()) * _Cb2.inv();
	}

/* AND */

	/*
	 * Function: getANDz
	 * Returns function tuple z used for homomorphic AND
	 */
	const MultiQuadTuple<7*N, 2*N> getANDz() const{
		BitMatrix<N, 2*N> X = getANDX();
		const BitMatrix<3*N> & Cb2i = _Cb2.inv();
		const BitMatrix<N, 3*N> & Y1 = _Bi.pMult(Cb2i, 0);
		const BitMatrix<N, 3*N> & Y2 = _Bi.pMult(Cb2i, N);
		const BitMatrix<N, 3*N> & Y3 = Cb2i.splitV3(2);

		const BitMatrix<((7*N * (7*N + 1)) >> 1), N> & contrib = BitMatrix<((7*N * (7*N + 1)) >> 1), N>::augV(getANDP(X, Y2), getANDQ(X, Y1), getANDS(Y1, Y2));
		MultiQuadTuple<7*N, N> zTop;
		zTop.setContributions(contrib, BitVector<N>::zeroVector());
		zTop = zTop.template rMult<N>(_pk.getB());

		MultiQuadTuple<7*N, N> Y3M;
		Y3M.setAsMatrix(BitMatrix<N, 7*N>::augH(BitMatrix<N, 4*N>::zeroMatrix(), Y3));
		//zTop = zTop ^ Y3M;

		//MultiQuadTuple<7*N, N> zeroMQT;
		//zeroMQT.zero();
		MultiQuadTuple<7*N, 2*N> z;
		//z.augV(zTop, zeroMQT);
		return z;//.template rMult<2*N>(_M);
	}

	/*
	 * Function: getANDZ1
	 * Returns matrix Z1 used for homomorphic AND
	 */
	const BitMatrix<2*N> getANDZ1() const{
		const BitMatrix<N, 2*N> & top = _Rx * _AiM2;
		const BitMatrix<N, 2*N> & bottom = _pk.getA() * top;
		return _M * BitMatrix<2*N>::augV(top, bottom);
	}

	/*
	 * Function: getANDZ2
	 * Returns matrix Z2 used for homomorphic AND
	 * Dimension of Z2: 2*(N * 2^6) by 2*(N * 2^6)
	 */
	const BitMatrix<2*N> getANDZ2() const{
		const BitMatrix<N, 2*N> & top = _Ry * _AiM2;
		const BitMatrix<N, 2*N> & bottom = _pk.getA() * top;
		return _M * BitMatrix<2*N>::augV(top, bottom);
	}


private:
	const PrivateKey<N> _pk;
	const BitMatrix<N> _R;
	const BitMatrix<N> _Rx;
	const BitMatrix<N> _Ry;
	const BitMatrix<2*N> _M;
	const BitMatrix<2*N> _Cu1;
	const BitMatrix<2*N> _Cu2;
	const BitMatrix<3*N> _Cb1;
	const BitMatrix<3*N> _Cb2;
	const BitMatrix<N> _Ai;
	const BitMatrix<N> _Bi;
	const BitMatrix<N> _ARAi;
	const BitMatrix<N> _ARxAi;
	const BitMatrix<N> _ARyAi;
	const BitMatrix<N, 2*N> _AiM2;
	static const unsigned int twoN = N << 1;
	static const unsigned int threeN = 3 * N;

/*Helper functions for getANDz*/

	/*
	 * Function: getANDX
	 * Returns matrix X used to compute z for homomorphic AND
	 */
	const BitMatrix<N, 2*N> getANDX() const{
		const BitMatrix<N, 2*N> & inner = BitMatrix<N, 2*N>::augH(BitMatrix<N>::identityMatrix(), _Ai);
		return _Bi * inner * _M.inv();
	}

	/*
	 * Function: getANDPk
	 * top chunk of contrib matrix for z for homomorphic AND
	 * level ranges from 0 to 64 * 2N - 1
	 */
	template <unsigned int level>
	const BitMatrix<7*N - level, N> getANDPk(const BitMatrix<N, 2*N> &X, const BitMatrix<N, 3*N> &Y2) const{
		const BitMatrix<twoN - level, N> & top = BitMatrix<twoN - level, N>::zeroMatrix();

		BitMatrix<twoN, N> mid = BitMatrix<twoN, N>::zeroMatrix();
		for (unsigned int j = 0; j < N; ++j) { //col within middle block
			if(X.get(j, level)){
				for (unsigned int i = 0; i < twoN; ++i) { //row within middle block
					mid.set(i, j, X.get(j, i));
				}
			}
		}

		BitMatrix<threeN, N> bot = BitMatrix<threeN, N>::zeroMatrix();
		for (unsigned int j = 0; j < N; ++j) { //col within bottom block
			if(X.get(j, level)){
				for (unsigned int i = 0; i < threeN; ++i) { //row within bottom block
					bot.set(i, j, Y2.get(j, i));
				}
			}
		}
		return BitMatrix<7*N - level, N>::augV(top, mid, bot);
	}

	/*
	 * Function: getANDQk
	 * middle chunk of contrib matrix for z
	 * level ranges from 0 to 64 * 2N - 1
	 */
	template <unsigned int level>
	const BitMatrix<5*N - level, N> getANDQk(const BitMatrix<N, 2*N>&X, const BitMatrix<N, 3*N> &Y1) const{
		const BitMatrix<twoN - level, N> & top = BitMatrix<twoN - level, N>::zeroMatrix();
		BitMatrix<threeN, N> bot = BitMatrix<threeN, N>::zeroMatrix();
		for (unsigned int j = 0; j < N; ++j) { //col within bottom block
			if(X.get(j, level)){
				for (unsigned int i = 0; i < threeN; ++i) { //row within bottom block
					bot.set(i, j, Y1.get(j, i));
				}
			}
		}
		return BitMatrix<5*N - level, N>::augV(top, bot);
	}

	/*
	 * Function: getANDSk
	 * level ranges from 0 to 64 * 3N - 1
	 */
	template <unsigned int level>
	const BitMatrix<3*N - level, N> getANDSk(const BitMatrix<N, 3*N> &Y1, const BitMatrix<N, 3*N> &Y2) const{
		BitMatrix<threeN - level, N> contrib = BitMatrix<threeN - level, N>::zeroMatrix();
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
	const BitMatrix<N*(12*N + 1), N> getANDP(const BitMatrix<N, 2*N> &X, const BitMatrix<N, 3*N> &Y2) const{
		BitMatrix<N*(12*N + 1), N> contrib = BitMatrix<N*(12*N + 1), N>::zeroMatrix();
		unsigned int count = 0;
		//unsigned int toAdd = 7 * N;
		for(unsigned int level = 0; level < twoN; ++level){
			/****************************************************************/
			//BitMatrix<twoN - level, N> top = BitMatrix<twoN - level, N>::zeroMatrix();

			BitMatrix<twoN, N> mid = BitMatrix<twoN, N>::zeroMatrix();
			for (unsigned int j = 0; j < N; ++j) { //col within middle block
				if(X.get(j, level)){
					for (unsigned int i = 0; i < twoN; ++i) { //row within middle block
						mid.set(i, j, X.get(j, i));
					}
				}
			}

			contrib.setSubMatrix(count + twoN - level, mid);

			BitMatrix<threeN, N> bot = BitMatrix<threeN, N>::zeroMatrix();
			for (unsigned int j = 0; j < N; ++j) { //col within bottom block
				if(X.get(j, level)){
					for (unsigned int i = 0; i < threeN; ++i) { //row within bottom block
						bot.set(i, j, Y2.get(j, i));
					}
				}
			}

			contrib.setSubMatrix(count + twoN - level + twoN, bot);

			count += (7*N - level);
			/****************************************************************/
			//contrib.setSubMatrix(count, getANDPk<level>(X, Y2));
			//count += toAdd;
			//--toAdd;
		}
		return contrib;
	}

	/*
	 * Function: getANDQ (entire chunk)
	 * middle chunk of contrib matrix for z for homomorphic AND
	 */
	const BitMatrix<N*(8*N + 1), N> getANDQ(const BitMatrix<N, 2*N> &X, const BitMatrix<N, 3*N> &Y1) const{
		BitMatrix<N*(8*N + 1), N> contrib = BitMatrix<N*(8*N + 1), N>::zeroMatrix();
		unsigned int count = 0;
		//unsigned int toAdd = 5 * N;
		for(unsigned int level = 0; level < twoN; ++level){
			/****************************************************************/
			BitMatrix<threeN, N> bot = BitMatrix<threeN, N>::zeroMatrix();
			for (unsigned int j = 0; j < N; ++j) { //col within bottom block
				if(X.get(j, level)){
					for (unsigned int i = 0; i < threeN; ++i) { //row within bottom block
						bot.set(i, j, Y1.get(j, i));
					}
				}
			}
			contrib.setSubMatrix(count + twoN - level, bot);
			count += (5*N - level);
			/****************************************************************/
			//contrib.setSubMatrix(count, getANDQk<level>(X, Y1));
			//count += toAdd;
			//--toAdd;
		}
		return contrib;
	}

	/*
	 * Function: getANDS (entire chunk)
	 * bottom chunk of contrib matrix for z
	 */
	const BitMatrix<((3*N*(3*N + 1)) >> 1), N> getANDS(const BitMatrix<N, 3*N> &Y1, const BitMatrix<N, 3*N> &Y2) const{
		BitMatrix<((3*N*(3*N + 1)) >> 1), N> contrib = BitMatrix<((3*N*(3*N + 1)) >> 1), N>::zeroMatrix();
		unsigned int count = 0;
		//unsigned int toAdd = 3 * N;
		for(unsigned int level = 0; level < threeN; ++level){
			/****************************************************************/
			for (unsigned int j = 0; j < N; ++j) { //cols
				bool prod = Y1.get(j, level) && Y2.get(j, level); //first row
				contrib.set(count, j, prod);
				if(Y1.get(j, level)){
					for (unsigned int i = 1; i < threeN - level; ++i) { //rows
						bool prod1 = Y2.get(j, level + i);
						contrib.set(i+count, j, Y2.get(j, level + i));
					}
				}
				if(Y2.get(j, level)){
					for(unsigned int i = 1; i < threeN - level; ++i){
						contrib.set(i+count, j, contrib.get(i+count, j) ^ Y1.get(j, level + i));
					}
				}
			}
			count += (3*N - level);
			/****************************************************************/
			//contrib.setSubMatrix(count, getANDSk<level>(Y1, Y2));
			//count += toAdd;
			//--toAdd;
		}
		return contrib;
	}
};

#endif