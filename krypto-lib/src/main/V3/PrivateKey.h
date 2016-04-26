//
//	PrivateKey.h
//	krypto
//
//	Created by njhlai
//	Copyright (c) 2016 Kryptnostic. All rights reserved.
//
// 	Implementation of private key generation
//	Accessed by clients
//

#ifndef krypto_PrivateKey_h
#define krypto_PrivateKey_h 

#include <NTL/ZZ.h>
#include <NTL/mat_ZZ_p.h>
#include <ctime>

using namespace std;
using namespace NTL;

/*
 * Template for PrivateKey
 */
class PrivateKey
{

friend class BridgeKey;
// friend class ClientHashFunction;
// friend class SearchPrivateKey;
// friend class KryptnosticClient;

public:
	/*
	 * Constructor
	 */
	PrivateKey(const unsigned int N)
	{
		generateParameters(N);
	}

	/*
	 * Function: encrypts a message m
	 * Returns: E(m, r)
	 */
	const vec_ZZ_p encrypt(const ZZ_p & m) const {
		ZZ_p::init(n);

		ZZ_p r = random_ZZ_p();

		vec_ZZ_p saltedmsg;
		saltedmsg.append(m + power(r, e));
		saltedmsg.append(r);

		return _M * saltedmsg;
	}

	/*
	 * Function: decrypts a ciphertext c
	 * Returns: D(c)
	 */
	const ZZ_p decrypt(const vec_ZZ_p & c) const {
		ZZ_p::init(n);

		// for NTL 9.5.0, the inverse function can't produce the correct inverse
		// so let's just suck it up and actually calculate it
		// it's only a 2x2 matrix anyway (lucky us!!)
		ZZ_p di = inv((_M(1,1) * _M(2,2)) - (_M(2,1) * _M(1,2))); // TODO: inv actually leaks memory

		mat_ZZ_p Mi;
		Mi.SetDims(2, 2);
		Mi.put(0, 0, _M(2,2) * di);
		Mi.put(0, 1, - _M(1, 2) * di);
		Mi.put(1, 0, - _M(2, 1) * di);
		Mi.put(1, 1, _M(1, 1) * di);

		// now back to the actual decryption
		vec_ZZ_p decryptVector = Mi * c;
		return decryptVector(1) - power(decryptVector(2), e);
	}

protected:
	/*
	 * Some protected functions for getting various components of PrivateKey
	 */
	const mat_ZZ_p getM() const {
		return _M;
	}

	const ZZ getModulus() const {
		return n;
	}

	const unsigned int getExponent() const {
		return e;
	}

	const ZZ getP() const {
		return p;
	}

	const ZZ getQ() const {
		return q;
	}

private:
	ZZ n; // modulus
	ZZ p, q; //primes
	unsigned int e; // exponent
	mat_ZZ_p _M; // secret matrix from GL(2, Z_n)

	/*
	 * Function: generate the modulus n, the exponent e, and the secret matrix M as components of PrivateKey
	 */
	void generateParameters(const unsigned int N) {
		// Sets seed for random prime generation
		// TODO: Probably need a better seed? Maybe not...
		// SetSeed(to_ZZ(time(0)));
		// Actually, YOU are currently responsible for setting the seed yourself. Sorry #SorryNotSorry

		// randomly generate to N-bits prime
		/*ZZ */p = RandomPrime_ZZ(N / 2);
		/*ZZ */q = p;
		while (q == p) q = RandomPrime_ZZ(N / 2);

		// define modulus n = pq
		n = p * q;

		// init modulus
		ZZ_p::init(n);

		// define exponent e
		e = 5;

		// construct secret matrix M
		// for now, the number 100 is entirely arbitrary, to ensure the power function doesn't take too long
		// for NTL 9.5.0, the determinant calculation provided can't produce the correct value
		// so let's just suck it up and actually calculate it
		// it's only a 2x2 matrix anyway (lucky us!!)
		_M.SetDims(2, 2);
		ZZ_p determinant = (_M(1,1) * _M(2,2)) - (_M(2,1) * _M(1,2));
		while (determinant == 0)
		{
			for (int i = 0; i < 4; ++i)
			{
				ZZ_p prime;
				conv(prime, (((i / 2) + (i % 2)) % 2 == 0 ? p : q));
				unsigned int exponent = 0;
				while (exponent == 0) exponent = rand() % 100;
				_M.put(i / 2, i % 2, random_ZZ_p() * power(prime, exponent));
			}
			determinant = (_M(1,1) * _M(2,2)) - (_M(2,1) * _M(1,2));
		}
	}
};

#endif