//
//	BridgeKey.h
//	krypto
//
//	Created by njhlai
//	Copyright (c) 2016 Kryptnostic. All rights reserved.
//
//	Implementation of bridge key generation
//	Accessed by clients, used to generate components of the public keys (Refer to implementation.pdf for the notations)
//

#ifndef krypto_BridgeKey_h
#define krypto_BridgeKey_h 

#include "PrivateKey.h"

class BridgeKey{
public:
	/*
	 * Constructor
	 */
	BridgeKey(const PrivateKey &pk) :
	_n(pk.getModulus()),
	_e(pk.getExponent()),
	_M(pk.getM()) 
	{}

	/*
	 * Structure for H_ADD operations
	 */
	struct H_ADD {
		ZZ _n; // modulus
		unsigned int _e; // exponent
		ZZ_p _c1, _c2; // a^{e+1} * det^{-e}, c^{e+1} * det^{-e} respectively

		/*
		 * Function: initialise and record all necessary param and coefficients
		 */
		void initialise(const ZZ &n, const unsigned int e, const ZZ_p & c1, const ZZ_p & c2) {
			_n = n;
			_e = e;
			_c1 = c1;
			_c2 = c2;
		}

		/*
		 * Function: compute H_ADD(x, y)
		 * Return: H_ADD(x, y)
		 */
		const vec_ZZ_p operator() (const vec_ZZ_p &x, const vec_ZZ_p &y) const {
			ZZ_p::init(_n);

			vec_ZZ_p result;
			result.append(x(1) + y(1) - _c1 * (power(x(2), _e) + power(y(2), _e) - power(x(2) + y(2), _e)));
			result.append(x(2) + y(2) + _c2 * (power(x(1), _e) + power(y(1), _e) - power(x(1) + y(1), _e)));

			return result;
		}
	};

	/*
	 * Function: Initialise H_ADD structure
	 * Return: H_ADD structure
	 */
	const H_ADD getADD() const {
		H_ADD result;

		// for NTL 9.5.0, the determinant calculation provided can't produce the correct value
		// so let's just suck it up and actually calculate it
		// it's only a 2x2 matrix anyway (lucky us!!)
		const ZZ_p deti = inv((_M(1,1) * _M(2,2)) - (_M(2,1) * _M(1,2)));
		result.initialise(_n, _e, power(_M(1, 1) * deti, _e) * _M(1, 1), power(_M(2, 1) * deti, _e) * _M(2, 1));

		return result;
	}

	/*
	 * Structure for H_MULT operations
	 */
	struct H_MULT {
		ZZ _n;
		unsigned int _e;
		ZZ_p _c11, _c12, _c13; // (ad^2 + Rbc^2 * det) * det^{-2}, a^{e+1} * d * det^{-e-1}, a^{2e+1} * det^{-2e} * (1 + R^e * det^e) respectively
		ZZ_p _c21, _c22, _c23; // (bc^2 + Ra^2d * det) * det^{-2}, c^{e+1} * b * det^{-e-1}, c^{2e+1} * det^{-2e} * (1 + R^e * det^e) respectively

		/*
		 * Function: initialise and record all necessary param and coefficients
		 */
		void initialise(const ZZ &n, const unsigned int e, const ZZ_p & c11, const ZZ_p & c12, const ZZ_p & c13, const ZZ_p & c21, const ZZ_p & c22, const ZZ_p & c23) {
			_n = n;
			_e = e;
			_c11 = c11;
			_c12 = c12;
			_c13 = c13;
			_c21 = c21;
			_c22 = c22;
			_c23 = c23;
		}

		/*
		 * Function: compute H_MULT(x, y)
		 * Return: H_MULT(x, y)
		 */
		const vec_ZZ_p operator() (const vec_ZZ_p & x, const vec_ZZ_p & y) const {
			ZZ_p::init(_n);

			ZZ_p x1, x2, y1, y2, x1e, x2e, y1e, y2e;
			x1 = x(1);
			x2 = x(2);
			y1 = y(1);
			y2 = y(2);

			x1e = power(x1, _e);
			x2e = power(x2, _e);
			y1e = power(y1, _e);
			y2e = power(y2, _e);

			vec_ZZ_p result;
			result.append((_c11 * x1 * y1) - _c12 * ((x1 * y2e) + (x2e * y1)) + (_c13 * x2e * y2e));
			result.append((_c21 * x2 * y2) - _c22 * ((x2 * y1e) + (x1e * y2)) + (_c23 * x1e * y1e));

			return result;
		}
	};

	/*
	 * Function: Initilise H_MULT structure
	 * Return: H_MULT structure
	 */
	// const H_MULT getMULT() const {
	// 	H_MULT result;

	// 	// for NTL 9.5.0, the determinant calculation provided can't produce the correct value
	// 	// so let's just suck it up and actually calculate it
	// 	// it's only a 2x2 matrix anyway (lucky us!!)
	// 	const ZZ_p deti = inv((_M(1,1) * _M(2,2)) - (_M(2,1) * _M(1,2)));
	// 	result.initialise(_n, _e, 
	// 		inv(_M(1,1) + _M(1,2)), _M(2,2) * power(_M(1,1) * deti, _e + 1), 2 * _M(1,1) * power(_M(1,1) * deti, 2 * _e),
	// 		inv(_M(2,1) + _M(2,2)), _M(1,2) * power(_M(2,1) * deti, _e + 1), 2 * _M(2,1) * power(_M(2,1) * deti, 2 * _e));

	// 	return result;
	// }
	const H_MULT getMULT() const {
		H_MULT result;
		ZZ_p R = random_ZZ_p();

		// for NTL 9.5.0, the determinant calculation provided can't produce the correct value
		// so let's just suck it up and actually calculate it
		// it's only a 2x2 matrix anyway (lucky us!!)
		const ZZ_p deti = inv((_M(1,1) * _M(2,2)) - (_M(2,1) * _M(1,2)));
		result.initialise(_n, _e, 
			(_M(1,1) * sqr(_M(2,2)) + R * _M(1,2) * sqr(_M(2,1)) * inv(deti)) * sqr(deti), _M(2,2) * power(_M(1,1) * deti, _e + 1), (_M(1,1) * power(_M(1,1) * deti, 2 * _e)) * (1 + power(R * inv(deti), _e)),
			(sqr(_M(1,2)) * _M(2,1) + R * sqr(_M(1,1)) * _M(2,2) * inv(deti)) * sqr(deti), _M(1,2) * power(_M(2,1) * deti, _e + 1), (_M(2,1) * power(_M(2,1) * deti, 2 * _e)) * (1 + power(R * inv(deti), _e)));

		return result;
	}

	const unsigned int getExponent() const {
		return _e;
	}

private:
	const ZZ _n; // modulus
	const unsigned int _e; // exponent
	const mat_ZZ_p _M; // secret matrix M
};

#endif