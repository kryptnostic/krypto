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
	_R(BitMatrix<N>::randomInvertibleMatrix(N<<6)),
	_BKBi(pk.getB() * K * pk.getB().inv()),
	_BKBiAi(_BKBi * pk.getA().inv()),
	_ARAi(pk.getA() * _R * pk.getA().inv())
	{
	}	

/* Left Matrix Multiplication */

	const BitMatrix<2*N> get_LMM_Z() const{
		//to be implemented
		return BitMatrix<N>::randomInvertibleMatrix(N<<7);
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

	const BitVector<2*N> get_XOR_z() const{
		//to be implemented
		return BitVector<2*N>::randomVector();
	}

	const BitMatrix<2*N> get_XOR_X() const{
		//to be implemented
		return BitMatrix<2*N>::randomInvertibleMatrix(N<<6);
	}

	const BitMatrix<3*N> get_XOR_Y1() const{
		//to be implemented
		return BitMatrix<3*N>::randomInvertibleMatrix(N<<6);
	}

	const BitMatrix<3*N> get_XOR_Y2() const{
		//to be implemented
		return BitMatrix<3*N>::randomInvertibleMatrix(N<<6);
	}

	const BitMatrix<N> get_XOR_Z1() const{
		//to be implemented
		return BitMatrix<N>::randomInvertibleMatrix(N<<6);
	}

	const BitMatrix<N> get_XOR_Z2() const{
		//to be implemented
		return BitMatrix<N>::randomInvertibleMatrix(N<<6);
	}

	const BitMatrix<N> get_XOR_g1() const{
		//to be implemented
		return BitMatrix<N>::randomInvertibleMatrix(_dim_quad<<6);
	}

	const BitMatrix<N> get_XOR_g2() const{
		//to be implemented
		return BitMatrix<N>::randomInvertibleMatrix(_dim_quad<<6);
	}

/* AND */

	const BitMatrix<2*N> get_AND_Xx() const{
		//to be implemented
		return BitMatrix<2*N>::randomInvertibleMatrix(N<<7);
	}

	const BitMatrix<2*N> get_AND_Xy() const{
		//to be implemented
		return BitMatrix<2*N>::randomInvertibleMatrix(N<<7);
	}

	const BitMatrix<3*N> get_AND_Y() const{
		//to be implemented
		return BitMatrix<3*N>::randomInvertibleMatrix(N<<7);
	}

	const BitMatrix<3*N> get_AND_g1() const{
		//to be implemented
		return BitMatrix<3*N>::randomInvertibleMatrix(_dim_quad<<6);
	}

	const BitMatrix<3*N> get_AND_g2() const{
		//to be implemented
		return BitMatrix<3*N>::randomInvertibleMatrix(_dim_quad<<6);
	}


private:
	BitMatrix<N> _R; //TODO: delegate the random matrix generation task to some other class?
	BitMatrix<N> _BKBi; 
	BitMatrix<N> _BKBiAi;
	BitMatrix<N> _ARAi;
	PolynomialFunctionTupleChain<2*N,L> _g_u; //obsfucated chain for unary operations
	PolynomialFunctionTupleChain<3*N,L> _g_b; //obsfucated chain for binary operations
	int _dim_quad = 64; //dimension of bitmatrix used to represent quadratic poly's

	// Computes matrix X used in the computation of Z
	const BitMatrix<2*N> getX() const{
		//to be implemented
		return _R;
	}

	// Computes matrix Y used in the computation of Z
	const BitMatrix<2*N> getY() const{
		//to be implemented
		return _R;
	}
};

#endif