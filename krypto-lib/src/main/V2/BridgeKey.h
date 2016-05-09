//
//  BridgeKey.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 7/22/15.
//	Modified by njhlai
//  Copyright (c) 2016 Kryptnostic. All rights reserved.
//
//  Implementation of bridge key generation
//	Accessed by clients, used to generate components of the public keys (Refer to implementation.pdf for the notations)
//

#ifndef krypto_BridgeKey_h
#define krypto_BridgeKey_h

#include "PrivateKey.h"
#include <iostream>
#include <ctime>

/*
 * Template for BridgeKey
 * Plaintext Length: N
 */
template<unsigned int N, unsigned int MON>
class BridgeKey{
public:

	/*
     * Constructor
     * Constructs a BridgeKey with a given PrivateKey
     * and BitMatrix K (for left-matrix multiplcation)
     */
	BridgeKey(const PrivateKey<N,MON> &pk) :
	_pk(pk),
	_R(BitMatrix<N>::randomInvertibleMatrix()),
	_M(pk.getM()),
	// this can be made more... modular (i.e. abstracting to referencing l instead of specialising l = 2)
	_Cu1(pk.getUnaryObf1()),
	_Cu2(pk.getUnaryObf2()),
	_Cb1(pk.getBinaryObf1()),
	_Cb2(pk.getBinaryObf2())
	{}

/* Unary unified code */

	/*
	 * Function: getUnaryG
	 * Returns the obfuscation matrix unary operations (e.g. left matrix multiplication)
	 */
	const BitMatrix<2*N> getPreUnaryGMatrix() const {
		const BitMatrix<N,2*N> & matTop = _M.inv().splitV2(1);
		const BitMatrix<N,2*N> & matBot = _R * matTop;

		return _Cu1 * BitMatrix<2*N>::augV(matTop, matBot);
	}

	/*
	 * Function: getUnaryG2
	 * Returns the obfuscation polynomial for unary operations (e.g. left matrix multiplication)
	 */
	const ConstantChainHeader<2*N,2*N> getUnaryG() const{
		const SecretPolynomial<N,N,MON> & f = _pk.getf();
		const BitMatrix<2*N> Cu1i = _Cu1.inv();

		const SecretPolynomial<2*N,2*N,MON> & Gu = SecretPolynomial<2*N,2*N,MON>::permute(SecretPolynomial<2*N,2*N,MON>::getUnaryF(f) * Cu1i, _Cu2);

		return Gu.convertToPublicPolynomial();
	}

/* Left Matrix Multiplication */

	/*
	 * Function: getLMMZ
	 * Returns matrix Z used for homomorphic left matrix multiplication
	 */
	const BitMatrix<2*N, 4*N> getLMMZ(const BitMatrix<N> & T) const{
		const BitMatrix<N> & zeroN = BitMatrix<N>::zeroMatrix();

		const BitMatrix<N, 2*N> & XTop = BitMatrix<N, 2*N>::augH(T, zeroN);
		const BitMatrix<N, 2*N> & XBot = BitMatrix<N, 2*N>::augH(zeroN, _R);
		const BitMatrix<2*N> & X = _M * BitMatrix<2*N>::augV(XTop, XBot) * _M.inv();

		const BitMatrix<N, 2*N> & YTop = BitMatrix<N, 2*N>::augH(T, BitMatrix<N>::identityMatrix());
		const BitMatrix<2*N> & Y = _M * BitMatrix<2*N>::augV(YTop, BitMatrix<N,2*N>::zeroMatrix()) * _Cu2.inv();

		return BitMatrix<2*N, 4*N>::augH(X, Y);
	}

/* Shift Matrices */
	/*
	 * Function: getLeftShiftMatrix
	 * Returns a matrix LS used for homomorphic left shift
	 */
	const BitMatrix<2*N, 4*N> getLeftShiftMatrix() const{
		return getLMMZ(BitMatrix<N>::leftShiftMatrix());
	}	

	/*
	 * Function: getRightShiftMatrix
	 * Returns a matrix RS used for homomorphic right shift
	 */
	const BitMatrix<2*N, 4*N> getRightShiftMatrix() const{
		return getLMMZ(BitMatrix<N>::rightShiftMatrix());
	}	

	const BitMatrix<2*N, 4*N> getLeftColumnMatrix() const{
		BitMatrix<N> M = BitMatrix<N>::zeroMatrix();
		for(int i = 0; i < N; ++i) M.set(i, 0);
		return getLMMZ(M);
	}

	/* 
	 * Function: getRightColumnMatrix
	 * Returns a matrix that is the encrypted version of [ 0 | ... | 0 | 1 ] 
	 * where each of {0,1} here is a column
	 */
	const BitMatrix<2*N, 4*N> getRightColumnMatrix() const{
		BitMatrix<N> M = BitMatrix<N>::zeroMatrix();
		for(int i = 0; i < N; ++i) M.set(i, N - 1);
		return getLMMZ(M);
	}

/* Binary unified code */

	/*
	 * Function: getBinaryG1
	 * Returns the obfuscation matrix for binary operations (e.g. XOR, AND)
	 */
	const BitMatrix<3*N,4*N> getPreBinaryGMatrix() const{
		const BitMatrix<N,2*N> & Mi2 = _M.inv().splitV2(1);

		const BitMatrix<N,4*N> & top = BitMatrix<N,4*N>::augH(Mi2, BitMatrix<N,2*N>::zeroMatrix());
		const BitMatrix<N,4*N> & mid = BitMatrix<N,4*N>::augH(BitMatrix<N,2*N>::zeroMatrix(), Mi2);
		const BitMatrix<N,4*N> & bot = BitMatrix<N,4*N>::augH(_Rx * Mi2, _Ry * Mi2);

		return _Cb1 * BitMatrix<3*N,4*N>::augV(top, mid, bot);
	}

	/*
	 * Function: getBinaryG2
	 * Returns the obfuscation polynomial for binary operations (e.g. XOR, AND)
	 */
	const ConstantChainHeader<3*N,3*N> getBinaryG() const{
		const SecretPolynomial<N,N,MON> & f = _pk.getf();
		const BitMatrix<3*N> & Cb1i = _Cb1.inv();

		const SecretPolynomial<3*N,3*N,MON> & Gb = SecretPolynomial<3*N,3*N,MON>::permute(SecretPolynomial<3*N,3*N,MON>::getBinaryF(f) * Cb1i, _Cb2);

		return Gb.convertToPublicPolynomial();
	}

/* XOR */
	struct H_XOR {
		BitMatrix<2*N> _Xx, _Xy;
		BitMatrix<3*N, 2*N> _Y;
		BitMatrix<4*N, 3*N> _gbMatrix;
		ConstantChainHeader<3*N, 3*N> _gbPoly;
		void initialize(const BitMatrix<2*N> & Xx, const BitMatrix<2*N> & Xy, const BitMatrix<3*N, 2*N> & Y, const BitMatrix<4*N, 3*N> & gbMatrix, const ConstantChainHeader<3*N, 3*N> & gbPoly){
			_Xx = Xx;
			_Xy = Xy;
			_Y = Y;
			_gbMatrix = gbMatrix;
			_gbPoly = gbPoly;
		}
		const BitVector<2*N> operator()(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
			const BitVector<4*N> & concatXY = BitVector<4*N>::template vCat<2*N, 2*N>(x, y);

			const BitVector<3*N> & t = _gbPoly(_gbMatrix.tMult(concatXY));

			return (_Xx.tMult(x)) ^ (_Xy.tMult(y)) ^ (_Y.tMult(t));
		}

		~H_XOR() {
			// freeAllNext must be called to properly delete ConstantHeaderChain
			_gbPoly.getMMC().freeAllNext();
		}
	};

	const H_XOR getXOR() const{
		H_XOR result;
		refreshParam();
		result.initialize(getXORXx().transpose(), getXORXy().transpose(), getXORY().transpose(), getPreBinaryGMatrix().transpose(), getBinaryG());
		return result;
	}

/* AND */
	struct H_AND {
		BitMatrix<25*N*N, N> _z;
		BitMatrix<2*N> _Zx, _Zy;
		BitMatrix<4*N,2*N> _Y;
		BitMatrix<4*N,3*N> _gbMatrix;
		ConstantChainHeader<3*N,3*N> _gbPoly;
		void initialize(const BitMatrix<25*N*N, N> & z, const BitMatrix<2*N> & Zx, const BitMatrix<2*N> & Zy, const BitMatrix<4*N,2*N> & Y, const BitMatrix<4*N,3*N> & gbMatrix, const ConstantChainHeader<3*N,3*N> & gbPoly){
			_z = z;
			_Zx = Zx;
			_Zy = Zy;
			_Y = Y;
			_gbMatrix = gbMatrix;
			_gbPoly = gbPoly;
		}
		const BitVector<2*N> operator()(const BitVector<2*N> &x, const BitVector<2*N> &y) const{
			const BitVector<4*N> & concatXY = BitVector<4*N>::template vCat<2*N, 2*N>(x, y);
			const BitVector<3*N> & t = _gbPoly(_gbMatrix.tMult(concatXY));

			const BitVector<5*N> & tx = BitVector<5*N>::vCat(x, t);
			const BitVector<5*N> & ty = BitVector<5*N>::vCat(y, t);

			return (_Zx.tMult(x)) ^ (_Zy.tMult(y)) ^ (_Y.tMult(BitVector<4*N>::vCat(_z.eval(tx, ty), t)));
		}

		~H_AND() {
			// freeAllNext must be called to properly delete ConstantHeaderChain
			_gbPoly.getMMC().freeAllNext();
		}
	};

	const H_AND getAND() const{
		H_AND result;
		refreshParam();
		result.initialize(getANDz(), getANDZx().transpose(), getANDZy().transpose(), getANDY().transpose(), getPreBinaryGMatrix().transpose(), getBinaryG());
		return result;
	}

private:
	const PrivateKey<N,MON> _pk;
	const BitMatrix<N> _R;
	BitMatrix<N> _Rx;
	BitMatrix<N> _Ry;
	const BitMatrix<2*N> _M;
	const BitMatrix<2*N> _Cu1;
	const BitMatrix<2*N> _Cu2;
	const BitMatrix<3*N> _Cb1;
	const BitMatrix<3*N> _Cb2;

	/*
	 * Function: Refresh and re-randomise Rx, Ry and all associated variables
	 * Returns void
	 */
	const void refreshParam() const {
		// re-randomise Rx, Ry
		_Rx.copy(BitMatrix<N>::randomInvertibleMatrix());
		_Ry.copy(BitMatrix<N>::randomInvertibleMatrix());
	}

/* Helper Functions for getXOR */

	/*
	 * Function: getXORXx
	 * Returns matrix Xx used for homomorphic XOR
	 */
	const BitMatrix<2*N> getXORXx() const{
		const BitMatrix<N, 2*N> & XTop = BitMatrix<N, 2*N>::augH(BitMatrix<N>::identityMatrix(), BitMatrix<N>::zeroMatrix());
		const BitMatrix<N, 2*N> & XBot = BitMatrix<N, 2*N>::augH(BitMatrix<N>::zeroMatrix(), _Rx);

		return _M * BitMatrix<2*N>::augV(XTop, XBot) * _M.inv();
	}

	/*
	 * Function: getXORXy
	 * Returns matrix Xy used for homomorphic XOR
	 */
	const BitMatrix<2*N> getXORXy() const{
		const BitMatrix<N, 2*N> XTop = BitMatrix<N, 2*N>::augH(BitMatrix<N>::identityMatrix(), BitMatrix<N>::zeroMatrix());
		const BitMatrix<N, 2*N> XBot = BitMatrix<N, 2*N>::augH(BitMatrix<N>::zeroMatrix(), _Ry);

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

/* Helper functions for getAND */

	/*
	 * Function: getANDz
	 * Returns function tuple z used for homomorphic AND
	 */
	const BitMatrix<25*N*N, N> getANDz() const{
		unsigned int count = 0;
		const BitMatrix<N,2*N> & Mi1 = _M.inv().splitV2(0);
		const BitMatrix<3*N> & Cb2i = _Cb2.inv();

		BitMatrix<25*N*N,N> contrib = BitMatrix<25*N*N,N>::zeroMatrix();

		for (int i = 0; i < 5*N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				if ((i < 2*N ? Mi1.get(j, i) : Cb2i.get(j, i - 2*N)))
				{
					for (int k = 0; k < 5*N; ++k)
					{
						contrib.set(count + k, j, k < 2*N ? Mi1.get(j, k) : Cb2i.get(j + N, k - 2*N));
					}
				}
			}
			count += 5*N;
		}

		return contrib;
	}

	/*
	 * Function: getANDZx
	 * Returns matrix Zx used for homomorphic AND
	 */
	const BitMatrix<2*N> getANDZx() const{
		const BitMatrix<N, 2*N> & bottom = _Rx * _M.inv().splitV2(1);
		return _M * BitMatrix<2*N>::augV(BitMatrix<N, 2*N>::zeroMatrix(), bottom);
	}

	/*
	 * Function: getANDZy
	 * Returns matrix Zy used for homomorphic AND
	 */
	const BitMatrix<2*N> getANDZy() const{
		const BitMatrix<N, 2*N> & bottom = _Ry * _M.inv().splitV2(1);
		return _M * BitMatrix<2*N>::augV(BitMatrix<N, 2*N>::zeroMatrix(), bottom);
	}

	/*
	 * Function: getANDY
	 * Returns matrix Y used for homomorphic AND
	 */
	const BitMatrix<2*N,4*N> getANDY() const {
		const BitMatrix<N,4*N> & top = BitMatrix<N,4*N>::augH(BitMatrix<N>::identityMatrix(), _Cb2.inv().splitV3(2));
		const BitMatrix<2*N,4*N> & inner = BitMatrix<2*N,4*N>::augV(top, BitMatrix<N,4*N>::zeroMatrix());

		return _M * inner;
	}

};

#endif