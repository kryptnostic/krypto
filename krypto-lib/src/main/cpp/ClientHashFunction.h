//
//  ClientHashFunction.h
//  krypto
//
//  Created by Robin Zhang on 8/21/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ struct concatenating the three components of the ClientHashFunction h_\Omega
//

#include "PrivateKey.h"
#include "MultiQuadTuple.h"

using namespace std;

template<unsigned int N>
struct ClientHashFunction
{
/* Data */

	BitMatrix<N, 4*N> hashMatrix;
	MultiQuadTuple<N, 2*N> augmentedF2;
	MultiQuadTuple<N, 2*N> concealedF1;
	BitMatrix<N> _C; //to conceal individual function pieces, not accessible by server

/* Initialization */

	void initialize(const BitMatrix<N, 2*N> & K, const PrivateKey<N> & pk){
		_C = BitMatrix<N>::randomInvertibleMatrix();
		hashMatrix = generateHashMatrix(K, pk);
		augmentedF2 = generateAugmentedF2(K, pk);
		concealedF1 = generateConcealedF1(pk);
	}

/* Generation of individual components */

	/*
     * Function: generateHashMatrix()
     * Returns the matrix portion of the hash function
     * Applied to x concatenated with y
     */
	const BitMatrix<N, 4*N> generateHashMatrix(const BitMatrix<N, 2*N> & K, const PrivateKey<N> & pk){
		BitMatrix<2*N> Mi = pk.getM().inv();
		BitMatrix<N, 2*N> Mi1 = Mi.splitV(0);
		BitMatrix<N, 2*N> Mi2 = Mi.splitV(1);

		BitMatrix<N, 2*N> left = pk.getB().inv() * Mi1;
		BitMatrix<N, 2*N> right = pk.getA().inv() * Mi2;
		BitMatrix<N, 2*N> decryptMatrix = left ^ right;

		BitMatrix<N, 2*N> zero = BitMatrix<N, 2*N>::zeroMatrix();
		BitMatrix<N, 4*N> top = BitMatrix<N, 4*N>::augH(decryptMatrix, zero);
		BitMatrix<N, 4*N> bot = BitMatrix<N, 4*N>::augH(zero, decryptMatrix);
		return K * (BitMatrix<2*N, 4*N>::augV(top, bot));
	}

	/*
     * Function: generateAugmentedF2()
     * Returns the K (f2 C || f2 C) portion of the hash function
     * Applied to concealedF1(x) concatenated with concealedF1(y)
     */
	const MultiQuadTuple<N, 2*N> generateAugmentedF2(const BitMatrix<N, 2*N> & K, const PrivateKey<N> & pk) const{
		MultiQuadTuple<N, N> f2 = pk.getf().get(1);
		MultiQuadTuple<N, N> topBot = (f2 * _C);
		MultiQuadTuple<N, 2*N> augmentedDecrypt = MultiQuadTuple<N, 2*N>::augV(topBot, topBot);
		return augmentedDecrypt.rMult(K);
	}

	/*
     * Function: generateConcealedF1()
     * Returns the f1 C portion of the hash function
     * Applied to x and y separately
     */
	const MultiQuadTuple<N, 2*N> generateConcealedF1(const PrivateKey<N> & pk) const{
		MultiQuadTuple<N, N> f1 = pk.getf().get(0);
		BitMatrix<N, 2*N> Mi2 = pk.getM().inv().splitV(1);
		BitMatrix<N, 2*N> inner = pk.getA().inv() * Mi2;
		return (f1 * inner).rMult(_C.inv());
	}

/* Evaluation */

	/*
	 * Function: operator()
	 * Returns the hashed value given 2 inputs in the encrypted space
	 */
	const BitMatrix<N> operator()(const BitVector<2*N> & eSearchToken, const BitVector<2*N> & eObjSearchKey) const{
		BitVector<N> hashMatrixOutput = hashMatrix * BitVector<4*N>::vCat(eSearchToken, eObjSearchKey);
		BitVector<2*N> augmentedOutputF1 = BitVector<2*N>::vCat(concealedF1(eSearchToken), concealedF1(eObjSearchKey));
		BitVector<N> functionalOutput = augmentedF2(augmentedOutputF1);
		return hashMatrixOutput ^ functionalOutput;
	}
};