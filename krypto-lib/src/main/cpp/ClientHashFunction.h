//
//  ClientHashFunction.h
//  krypto
//
//  Created by Robin Zhang on 8/21/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ struct concatenating the three components of the ClientHashFunction h_\Omega
//

#include "MultiQuadTuple.h"

#define N 128

using namespace std;

struct ClientHashFunction
{
	/* Data */
	BitMatrix<N, 4*N> hashMatrix;
	MultiQuadTuple<2*N, N> augmentedF2;
	MultiQuadTuple<2*N, N> concealedF1;
	BitMatrix<N> _C;

	void initialize(const BitMatrix<N, 2*N> & K, const PrivateKey<N> & pk){
		_C = BitMatrix<N>::randomInvertibleMatrix();
		hashMatrix = generateHashMatrix(K, pk);
		augmentedF2 = generateAugmentedF2(K, pk);
		concealedF1 = generateConcealedF1(pk);
	}

/* Getters */

	const BitMatrix<N, 4*N> getHashMatrix() const{
		return hashMatrix;
	}

	const MultiQuadTuple<2*N, N> getAugmentedF2() const{
		return augmentedF2;
	}

	const MultiQuadTuple<2*N, N> getConcealedF1() const{
		return concealedF1;
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
	const MultiQuadTuple<2*N, N> generateAugmentedF2(const BitMatrix<N, 2*N> & K, const PrivateKey<N> & pk) const{
		MultiQuadTuple<N, N> f2 = pk.getf().get(1);
		MultiQuadTuple<N, N> topBot = (f2 * _C);
		MultiQuadTuple<2*N, 2*N> augmentedDecrypt; //needs to be augmentation w/ diff inputs of topBot, topBot
		return augmentedDecrypt.rMult(K);
	}

	/*
     * Function: generateConcealedF1()
     * Returns the C^{-1} f1 portion of the hash function
     * Applied to x and y separately
     */
	const MultiQuadTuple<2*N, N> generateConcealedF1(const PrivateKey<N> & pk) const{
		MultiQuadTuple<N, N> f1 = pk.getf().get(0);
		BitMatrix<N, 2*N> Mi2 = pk.getM().inv().splitV(1);
		BitMatrix<N, 2*N> inner = pk.getA().inv() * Mi2;
		return (f1 * inner).rMult(_C.inv());
	}
};