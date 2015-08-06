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
#include "BitMatrix.h"
#include "MultiQuadTuple.h"

using namespace std;

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
	BridgeKey(PrivateKey<N,L> &pk, BitMatrix<N> K) : 
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
	_BKBi(pk.getB() * K * pk.getB().inv()),
	_BKBiAi(_BKBi * pk.getA().inv()),
	_ARAi(pk.getA() * _R * pk.getA().inv()),
	_ARxAi(pk.getA() * _Rx * pk.getA().inv()),
	_ARyAi(pk.getA() * _Ry * pk.getA().inv()),
	_AiM2(_pk.getA().inv().template pMult<2*N>(_M.inv(), NN, 2*NN-1))
	{
	}

/* Unary unified code */

	/*
	 * Function: get_UNARY_g1
	 * Returns the first function of the obfuscation chain for unary operations (e.g. left matrix multiplication)
	 */
	const MultiQuadTuple<2*N, 2*N> get_UNARY_g1() const{
		MultiQuadTupleChain<N,L> f = _pk.getf();

		BitMatrix<2*N> mat_top = _AiM2;
		BitMatrix<2*N> mat_bot = _R * mat_top;

		MultiQuadTuple<2*N, N> top = f.get(0) * mat_top;
		MultiQuadTuple<2*N, N> bot = f.get(0) * mat_bot;
		MultiQuadTuple<2*N, 2*N> aug = MultiQuadTuple<2*N, 2*N>::aug_v(top, bot);
		return aug.template rMult<2*N>(_Cu1);
	}

	/*
	 * Function: get_UNARY_g2
	 * Returns the second function of the obfuscation chain for unary operations (e.g. left matrix multiplication)
	 */
	const MultiQuadTuple<2*N, 2*N> get_UNARY_g2() const{
		MultiQuadTupleChain<N,L> f = _pk.getf();

		BitMatrix<2*N> mat_top = _Cu1.inv().split_v_2(0);
		BitMatrix<2*N> mat_bot = _Cu1.inv().split_v_2(1);

		MultiQuadTuple<2*N, N> top = f.get(1) * mat_top;
		MultiQuadTuple<2*N, N> bot = f.get(1) * mat_bot;
		MultiQuadTuple<2*N, 2*N> aug = MultiQuadTuple<2*N, 2*N>::aug_v(top, bot);
		return aug.template rMult<2*N>(_Cu2);
	}

/* Left Matrix Multiplication */

	/*
	 * Function: get_LMM_Z
	 * Returns matrix Z used for homomorphic left matrix multiplication
	 * Dimension of Z: 2*(N * 2^6) by 4*(N * 2^6)
	 */
	const BitMatrix<4*N> get_LMM_Z() const{
		BitMatrix<N> zeroN = BitMatrix<N>::squareZeroMatrix();
		BitMatrix<N> RAi = _R * _Ai;

		BitMatrix<2*N> X_top = BitMatrix<2*N>::aug_h(_BKBi, _BKBiAi ^ RAi);
		BitMatrix<2*N> X_bottom = BitMatrix<2*N>::aug_h(zeroN, _ARAi);
		BitMatrix<2*N> X = _M * BitMatrix<2*N>::aug_v(X_top, X_bottom) * _M.inv();

		BitMatrix<2*N> Y_top = BitMatrix<2*N>::aug_h(_BKBi, BitMatrix<N>::squareIdentityMatrix());
		BitMatrix<2*N> Y_bottom = BitMatrix<2*N>::aug_h(zeroN, zeroN);
		BitMatrix<2*N> Y = _M * BitMatrix<2*N>::aug_v(Y_top, Y_bottom) * _Cu2.inv();
		return BitMatrix<4*N>::aug_h(X, Y);
	}

/* Binary unified code */

	/*
	 * Function: get_BINARY_g1
	 * Returns the first function of the obfuscation chain for binary operations (e.g. XOR, AND)
	 */
	const MultiQuadTuple<4*N, 3*N> get_BINARY_g1() const{
		MultiQuadTupleChain<N,L> f = _pk.getf();

		BitMatrix<2*N> M2 = _M.inv().split_v_2(1);
		BitMatrix<4*N> M2_projX = BitMatrix<4*N>::aug_h(M2, BitMatrix<2*N>::zeroMatrix(N << 6));
		BitMatrix<4*N> M2_projY = BitMatrix<4*N>::aug_h(BitMatrix<2*N>::zeroMatrix(N << 6), M2);

		BitMatrix<4*N> mat_top = _Ai * M2_projX;
		BitMatrix<4*N> mat_mid = _Ai * M2_projY;
		BitMatrix<4*N> mat_botX = _Rx * mat_top;
		BitMatrix<4*N> mat_botY = _Ry * mat_mid;

		MultiQuadTuple<4*N, N> top = f.get(0) * mat_top;
		MultiQuadTuple<4*N, N> mid = f.get(0) * mat_mid;
		MultiQuadTuple<4*N, N> bot = f.get(0) * (mat_botX ^ mat_botY);
		MultiQuadTuple<4*N, 3*N> aug = MultiQuadTuple<4*N, 3*N>::aug_v(MultiQuadTuple<4*N, 2*N>::aug_v(top, mid), bot);
		return aug.template rMult<3*N>(_Cb1);
	}

	/*
	 * Function: get_BINARY_g2
	 * Returns the second function (g2) of the obfuscation chain for binary operations (e.g. XOR, AND)
	 */
	const MultiQuadTuple<3*N, 3*N>get_BINARY_g2() const{
		MultiQuadTupleChain<N,L> f = _pk.getf();

		BitMatrix<3*N> Cb1_inv = _Cb1.inv();
		BitMatrix<3*N> mat_top = Cb1_inv.split_v_3(0);
		BitMatrix<3*N> mat_mid = Cb1_inv.split_v_3(1);
		BitMatrix<3*N> mat_bot = Cb1_inv.split_v_3(2);

		MultiQuadTuple<3*N, N> top = f.get(1) * mat_top;
		MultiQuadTuple<3*N, N> mid = f.get(1) * mat_mid;
		MultiQuadTuple<3*N, N> bot = f.get(1) * mat_bot;
		MultiQuadTuple<3*N, 3*N> aug = MultiQuadTuple<3*N, 3*N>::aug_v(MultiQuadTuple<3*N, 2*N>::aug_v(top, mid), bot);
		return aug.template rMult<3*N>(_Cb2);
	}


/* XOR */

	/*
	 * Function: get_XOR_Xx
	 * Returns matrix Xx used for homomorphic XOR
	 * Dimension of Xx: 2*(N * 2^6) by 2*(N * 2^6)
	 */
	const BitMatrix<2*N> get_XOR_Xx() const{
		BitMatrix<N> idN = BitMatrix<N>::squareIdentityMatrix();

		BitMatrix<2*N> X_top = BitMatrix<2*N>::aug_h(idN, (idN ^ _Rx) * _Ai);
		BitMatrix<2*N> X_bottom = BitMatrix<2*N>::aug_h(BitMatrix<N>::squareZeroMatrix(), _ARxAi);
		return _M * BitMatrix<2*N>::aug_v(X_top, X_bottom) * _M.inv();
	}

	/*
	 * Function: get_XOR_Xy
	 * Returns matrix Xy used for homomorphic XOR
	 * Dimension of Xy: 2*(N * 2^6) by 2*(N * 2^6)
	 */
	const BitMatrix<2*N> get_XOR_Xy() const{
		BitMatrix<N> idN = BitMatrix<N>::squareIdentityMatrix();

		BitMatrix<2*N> X_top = BitMatrix<2*N>::aug_h(idN, (idN ^ _Ry) * _Ai);
		BitMatrix<2*N> X_bottom = BitMatrix<2*N>::aug_h(BitMatrix<N>::squareZeroMatrix(), _ARyAi);
		return _M * BitMatrix<2*N>::aug_v(X_top, X_bottom) * _M.inv();
	}

	/*
	 * Function: get_XOR_Y
	 * Returns matrix Y used for homomorphic XOR
	 * Dimension of Y: 2*(N * 2^6) by 3*(N * 2^6)
	 */
	const BitMatrix<3*N> get_XOR_Y() const{
		BitMatrix<N> idN = BitMatrix<N>::squareIdentityMatrix();

		BitMatrix<3*N> Y_top = BitMatrix<3*N>::aug_h(idN, BitMatrix<2*N>::aug_h(idN, idN));
		return _M * BitMatrix<3*N>::aug_v(Y_top, BitMatrix<3*N>::zeroMatrix(NN)) * _Cb2.inv();
	}

/* AND */

	/*
	 * Function: get_AND_z
	 * Returns function tuple z used for homomorphic AND
	 */
	const MultiQuadTuple<7*N, 2*N> get_AND_z() const{
		BitMatrix<2*N> X = get_AND_X();
		BitMatrix<3*N> Y1 = get_AND_Y1();
		BitMatrix<3*N> Y2 = get_AND_Y2();
		BitMatrix<3*N> Y3 = get_AND_Y3();
		BitMatrix<7*N> Y3t = BitMatrix<7*N>::aug_h(BitMatrix<4*N>::zeroMatrix(N << 6), Y3);

		BitMatrix<N> contrib = BitMatrix<N>::aug_v(BitMatrix<N>::aug_v(get_AND_P(X, Y2), get_AND_Q(X, Y1)), get_AND_S(Y1, Y2));
		MultiQuadTuple<7*N, N> z_top(contrib, BitVector<N>::zeroVector());
		z_top = z_top.template rMult<N>(_pk.getB());
		z_top = z_top ^ MultiQuadTuple<7*N, N>::getMultiQuadTuple(Y3t);
		MultiQuadTuple<7*N, N> zeroMQT = MultiQuadTuple<7*N, N>::zeroMultiQuadTuple();
		MultiQuadTuple<7*N, 2*N> z = MultiQuadTuple<7*N, 2*N>::aug_v(z_top, zeroMQT);
		MultiQuadTuple<7*N, 2*N> result = z.template rMult<2*N>(_M);

		return result;
	}

	/*
	 * Function: get_AND_Z1
	 * Returns matrix Z1 used for homomorphic AND
	 * Dimension of Z1: 2*(N * 2^6) by 2*(N * 2^6) 
	 */
	const BitMatrix<2*N> get_AND_Z1() const{
		BitMatrix<2*N> top = _Rx * _AiM2;
		BitMatrix<2*N> bottom = _pk.getA() * top;
		return _M * BitMatrix<2*N>::aug_v(top, bottom);
	}

	/*
	 * Function: get_AND_Z2
	 * Returns matrix Z2 used for homomorphic AND
	 * Dimension of Z2: 2*(N * 2^6) by 2*(N * 2^6) 
	 */
	const BitMatrix<2*N> get_AND_Z2() const{
		BitMatrix<2*N> top = _Ry * _AiM2;
		BitMatrix<2*N> bottom = _pk.getA() * top;
		return _M * BitMatrix<2*N>::aug_v(top, bottom);
	}


private:
	PrivateKey<N,L> _pk;
	BitMatrix<N> _R; //TODO: delegate the random matrix generation task to some other class?
	BitMatrix<N> _Rx;
	BitMatrix<N> _Ry;
	BitMatrix<2*N> _M;
	BitMatrix<2*N> _Cu1;
	BitMatrix<2*N> _Cu2;
	BitMatrix<3*N> _Cb1;
	BitMatrix<3*N> _Cb2;
	BitMatrix<N> _BKBi; 
	BitMatrix<N> _BKBiAi;
	BitMatrix<N> _ARAi;
	BitMatrix<N> _ARxAi;
	BitMatrix<N> _ARyAi;
	MultiQuadTupleChain<2*N,L> _g_u; //obsfucated chain for unary operations
	MultiQuadTupleChain<3*N,L> _g_b; //obsfucated chain for binary operations
	
	BitMatrix<N> _Ai;
	BitMatrix<N> _Bi;
	BitMatrix<2*N> _AiM2;
	static const unsigned int twoN = N << 1;
	static const unsigned int threeN = 3 * N;
	static const unsigned int NN = N << 6;
	static const unsigned int twoNN = NN << 1;
	static const unsigned int threeNN = 3 * NN;

/*Helper functions for get_AND_z*/

	/*
	 * Function: get_AND_X
	 * Returns matrix X used to compute z for homomorphic AND
	 * Dimension of X: (N * 2^6) by 2*(N * 2^6)
	 */
	const BitMatrix<2*N> get_AND_X() const{
		BitMatrix<2*N> inner = BitMatrix<2*N>::aug_h(BitMatrix<N>::squareIdentityMatrix(), _Ai);
		return _Bi * inner * _M.inv();
	}

	/*
	 * Function: get_AND_Y1
	 * Returns matrix Y1 used to compute z for homomorphic AND
	 * Dimension of X: (N * 2^6) by 3*(N * 2^6)
	 */
	const BitMatrix<3*N> get_AND_Y1() const{
		return _Bi.pMult(_Cb2.inv(), 0, NN-1);
	}

	/*
	 * Function: get_AND_Y2
	 * Returns matrix Y2 used to compute z for homomorphic AND
	 * Dimension of X: (N * 2^6) by 3*(N * 2^6)
	 */
	const BitMatrix<3*N> get_AND_Y2() const{
		return _Bi.pMult(_Cb2.inv(), NN, 2*NN-1);
	}

	/*
	 * Function: get_AND_Y3
	 * Returns matrix Y3 used to compute z for homomorphic AND
	 * Dimension of X: (N * 2^6) by 3*(N * 2^6)
	 */
	const BitMatrix<3*N> get_AND_Y3() const{
		return _Cb2.inv().split_v_3(2);
	}

	/* 
	 * Function: get_AND_Pk
	 * top chunk of contrib matrix for z for homomorphic AND
	 * level ranges from 0 to 64 * 2N - 1
	 * Dimension: 7 * 64N - level
	 */
	const BitMatrix<N> get_AND_Pk(const int level, const BitMatrix<2*N> &X, const BitMatrix<3*N> &Y2) const{
		BitMatrix<N> top = BitMatrix<N>::zeroMatrix(twoNN - level); //not sure if there's obob here

		BitMatrix<N> mid = BitMatrix<N>::zeroMatrix(twoNN);
		for (size_t j = 0; j < NN; ++j) { //col within middle block
			if(X.get(j, level)){ 
				for (size_t i = 0; i < twoNN; ++i) { //row within middle block
					mid.set(i, j, X.get(j, i));
				}				
			}
		}

		BitMatrix<N> bot = BitMatrix<N>::zeroMatrix(threeNN);
		for (int j = 0; j < NN; ++j) { //col within bottom block
			if(X.get(j, level)){
				for (size_t i = 0; i < threeNN; ++i) { //row within bottom block		
					bot.set(i, j, Y2.get(j, i));
				}				
			}
		}
		return BitMatrix<N>::aug_v(BitMatrix<N>::aug_v(top, mid), bot);
	}

	/*
	 * Function: get_AND_Qk
	 * middle chunk of contrib matrix for z
	 * level ranges from 0 to 64 * 2N - 1
	 * Dimension: 5 * 64N - level
	 */
	const BitMatrix<N> get_AND_Qk(const int level, const BitMatrix<2*N>&X, const BitMatrix<3*N> &Y1) const{
		BitMatrix<N> top = BitMatrix<N>::zeroMatrix(twoNN - level);
		BitMatrix<N> bot = BitMatrix<N>::zeroMatrix(threeNN);
		for (size_t j = 0; j < NN; ++j) { //col within bottom block
			if(X.get(j, level)){
				for (size_t i = 0; i < threeNN; ++i) { //row within bottom block
					bot.set(i, j, Y1.get(j, i));
				}
			}
		}
		return BitMatrix<N>::aug_v(top, bot);
	}

	/*
	 * Function: get_AND_Sk
	 * level ranges from 0 to 64 * 3N - 1
	 * Dimension: 3 * 64N - level
	 */
	const BitMatrix<N> get_AND_Sk(const int level, const BitMatrix<3*N> &Y1, const BitMatrix<3*N> &Y2) const{
		BitMatrix<N> contrib = BitMatrix<N>::zeroMatrix(threeNN - level);
		for (size_t j = 0; j < NN; ++j) { //cols
			bool prod = Y1.get(j, level) && Y2.get(j, level); //first row
			contrib.set(0, j, prod);
/*
			for (size_t i = 1; i < threeNN - level; ++i) { //rows
				bool prod1 = Y1.get(j, level) && Y2.get(j, level + i);
				bool prod2 = Y1.get(j, level + i) && Y2.get(j, level);
				contrib.set(i, j, prod1 ^ prod2);
			}
*/
			if(Y1.get(j, level)){
				for (size_t i = 1; i < threeNN - level; ++i) { //rows
					bool prod1 = Y2.get(j, level + i);
					contrib.set(i, j, Y2.get(j, level + i)); 
				}				
			}
			if(Y2.get(j, level)){
				for(size_t i = 1; i < threeNN - level; ++i){
					contrib.set(i, j, contrib.get(i, j) ^ Y1.get(j, level + i)); 
				} 
			}			
		}
		return contrib;
	}

	/*
	 * Function: get_AND_P (entire chunk)
	 * top chunk of contrib matrix for z for homomorphic AND
	 */
	const BitMatrix<N> get_AND_P(const BitMatrix<2*N> &X, const BitMatrix<3*N> &Y2) const{
		BitMatrix<N> contrib = BitMatrix<N>::zeroMatrix(NN*(12*NN + 1));
		unsigned int count = 0;
		unsigned int toAdd = 7 * NN;
		for(size_t level = 0; level < twoNN; ++level){
			contrib.setSubMatrix(count, get_AND_Pk(level, X, Y2));
			count += toAdd;
			--toAdd;
		}
		return contrib;
	}

	/*
	 * Function: get_AND_Q (entire chunk)
	 * middle chunk of contrib matrix for z for homomorphic AND
	 */
	const BitMatrix<N> get_AND_Q(const BitMatrix<2*N> &X, const BitMatrix<3*N> &Y1) const{
		BitMatrix<N> contrib = BitMatrix<N>::zeroMatrix(NN*(8*NN + 1));
		unsigned int count = 0;
		unsigned int toAdd = 5 * NN;
		for(size_t level = 0; level < twoNN; ++level){
			contrib.setSubMatrix(count, get_AND_Qk(level, X, Y1));
			count += toAdd;
			--toAdd;
		}
		return contrib;		
	}

	/*
	 * Function: get_AND_S (entire chunk)
	 * bottom chunk of contrib matrix for z
	 */	
	const BitMatrix<N> get_AND_S(const BitMatrix<3*N> &Y1, const BitMatrix<3*N> &Y2) const{
		BitMatrix<N> contrib = BitMatrix<N>::zeroMatrix((3*NN*(3*NN + 1)) >> 1);
		unsigned int count = 0;
		unsigned int toAdd = 3 * NN;
		for(size_t level = 0; level < threeNN; ++level){
			contrib.setSubMatrix(count, get_AND_Sk(level, Y1, Y2));
			count += toAdd;
			--toAdd;
		}		
		return contrib;
	}	
};

#endif