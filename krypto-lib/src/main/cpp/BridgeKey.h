/*Accessed by client, used to generate components of the public keys*/
#ifndef krypto_BridgeKey_h
#define krypto_BridgeKey_h

#include <assert.h>
#include "PrivateKey.h"
#include "BitMatrix.h"

using namespace std;

//TODO: make BridgeKey a friend class of PrivateKey

template<unsigned int N, unsigned int L>
class BridgeKey{
public:
	BridgeKey(PrivateKey<N,L> pk, BitMatrix<N> K) : 
	_pk(pk),
	_R(BitMatrix<N>::randomInvertibleMatrix(N<<6)),
	_Rx(BitMatrix<N>::randomInvertibleMatrix(N<<6)),
	_Ry(BitMatrix<N>::randomInvertibleMatrix(N<<6)),
	_M(pk.getM()),
	_C1(pk.getUnaryObfChain()[1]),
	_C2(pk.getUnaryObfChain()[2]),
	_BKBi(pk.getB() * K * pk.getB().inv()),
	_BKBiAi(_BKBi * pk.getA().inv()),
	_ARAi(pk.getA() * _R * pk.getA().inv()),
	_ARxAi(pk.getA() * _Rx * pk.getA().inv()),
	_ARyAi(pk.getA() * _Ry * pk.getA().inv())
	{
	}	

/* Left Matrix Multiplication */

	const BitMatrix<2*N> get_LMM_Z() const{
		//untested!
		BitMatrix<N> zeroN = BitMatrix<N>::zeroMatrix(N << 6);

		BitMatrix<2*N> X_top = BitMatrix<N>::aug_h(_BKBi, _BKBiAi);
		BitMatrix<2*N> X_bottom = BitMatrix<N>::aug_h(zeroN, _ARAi);
		BitMatrix<2*N> X = BitMatrix<2*N>::aug_v(X_top, X_bottom) * _M.inv();

		BitMatrix<2*N> Y_top = BitMatrix<N>::aug_h(_BKBi, BitMatrix<N>::squareIdentityMatrix());
		BitMatrix<2*N> Y_bottom = BitMatrix<N>::aug_h(zeroN, zeroN);
		BitMatrix<2*N> Y = BitMatrix<2*N>::aug_v(X_top, X_bottom) * _C2.inv();
		return BitMatrix<2*N>::aug_h(X, Y);
	}

	const BitMatrix<N> get_LMM_g1() const{
		//to be implemented
		return BitMatrix<N>::randomInvertibleMatrix(_dim_quad<<6);
	}

	const BitMatrix<N> get_LMM_g2() const{
		//to be implemented
		return BitMatrix<N>::randomInvertibleMatrix(_dim_quad<<6);
	}


/* XOR */

	const BitMatrix<2*N> get_XOR_Xx() const{
		//untested!
		BitMatrix<N> idN = BitMatrix<N>::squareIdentityMatrix();

		BitMatrix<2*N> X_top = BitMatrix<N>::aug_h(idN, (idN + _Rx) * _pk.getA.inv());
		BitMatrix<2*N> X_bottom = BitMatrix<N>::aug_h(BitMatrix<N>::zeroMatrix(N << 6), _ARxAi);
		return BitMatrix<2*N>::aug_v(X_top, X_bottom) * _M.inv();
	}

	const BitMatrix<2*N> get_XOR_Xy() const{
		//untested!
		BitMatrix<N> idN = BitMatrix<N>::squareIdentityMatrix();

		BitMatrix<2*N> X_top = BitMatrix<N>::aug_h(idN, (idN + _Ry) * _pk.getA.inv());
		BitMatrix<2*N> X_bottom = BitMatrix<N>::aug_h(BitMatrix<N>::zeroMatrix(N << 6), _ARyAi);
		return BitMatrix<2*N>::aug_v(X_top, X_bottom) * _M.inv();
	}

	const BitMatrix<3*N> get_XOR_Y() const{
		//untested!
		BitMatrix<N> idN = BitMatrix<N>::squareIdentityMatrix();

		BitMatrix<3*N> Y_top = BitMatrix<N>::aug_h(idN, BitMatrix<N>::aug_h(idN, idN));
		return BitMatrix<3*N>::aug_v(Y_top, BitMatrix<3*N>::zeroMatrix(N << 6)) * _C2.inv();
	}

	const BitMatrix<3*N> get_XOR_g1() const{
		//to be implemented
		return BitMatrix<3*N>::randomInvertibleMatrix(_dim_quad<<6);
	}

	const BitMatrix<3*N> get_XOR_g2() const{
		//to be implemented
		return BitMatrix<3*N>::randomInvertibleMatrix(_dim_quad<<6);
	}


/* AND */

	const BitVector<2*N> get_AND_z() const{
		//to be implemented
		return BitVector<2*N>::randomVector();
	}

	const BitMatrix<2*N> get_AND_X() const{
		//to be implemented
		return BitMatrix<2*N>::randomInvertibleMatrix(N<<6);
	}

	const BitMatrix<3*N> get_AND_Y1() const{
		//to be implemented
		return BitMatrix<3*N>::randomInvertibleMatrix(N<<6);
	}

	const BitMatrix<3*N> get_AND_Y2() const{
		//to be implemented
		return BitMatrix<3*N>::randomInvertibleMatrix(N<<6);
	}

	const BitMatrix<2*N> get_AND_Z1() const{
		//untested!
		BitMatrix<2*N> M2 = _M.split_v(1, 2);
		BitMatrix<2*N> top = _Rx * pk.getA().inv() * M2;
		BitMatrix<2*N> bottom = pk.getA() * top;
		return BitMatrix<N>::aug_v(top, bottom);
	}

	const BitMatrix<N> get_AND_Z2() const{
		/BitMatrix<2*N> M2 = _M.split_v(1, 2);
		BitMatrix<2*N> top = _Ry * pk.getA().inv() * M2;
		BitMatrix<2*N> bottom = pk.getA() * top;
		return BitMatrix<N>::aug_v(top, bottom);
	}

	const BitMatrix<N> get_AND_g1() const{
		//to be implemented
		return BitMatrix<N>::randomInvertibleMatrix(_dim_quad<<6);
	}

	const BitMatrix<N> get_AND_g2() const{
		//to be implemented
		return BitMatrix<N>::randomInvertibleMatrix(_dim_quad<<6);
	}


private:
	PrivateKey<N,L> _pk;
	BitMatrix<N> _R; //TODO: delegate the random matrix generation task to some other class?
	BitMatrix<N> _Rx;
	BitMatrix<N> _Ry;
	BitMatrix<N> _M;
	BitMatrix<N> _C1;
	BitMatrix<N> _C2;
	BitMatrix<N> _BKBi; 
	BitMatrix<N> _BKBiAi;
	BitMatrix<N> _ARAi;
	BitMatrix<N> _ARxAi;
	BitMatrix<N> _ARyAi;
	PolynomialFunctionTupleChain<2*N,L> _g_u; //obsfucated chain for unary operations
	PolynomialFunctionTupleChain<3*N,L> _g_b; //obsfucated chain for binary operations
	int _dim_quad = 64; //dimension of bitmatrix used to represent quadratic poly's
};

#endif