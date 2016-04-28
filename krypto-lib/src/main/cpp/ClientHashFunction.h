//
//  ClientHashFunction.h
//  krypto
//
//  Created by Robin Zhang on 8/21/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ struct concatenating the three components of the ClientHashFunction h_\Omega
//

#ifndef ClientHashFunction_h
#define ClientHashFunction_h

#include "PrivateKey.h"
#include "MultiQuadTuple.h"

using namespace std;

template<unsigned int N = 128>
struct ClientHashFunction
{
/* Data */

	BitMatrix<N, 4*N> hashMatrix;
	MultiQuadTuple<2*N, N> augmentedF2;
	MultiQuadTuple<2*N, N> concealedF1;

/* Initialization */

	/*
     * Function: initialize(C, K, pk)
     * Initializes a ClientHashFunction with given private matrices and privateKey
     */
	void initialize(const BitMatrix<N> & C, const BitMatrix<N, 2*N> & K, const PrivateKey<N> & pk){
		generateHashMatrix(K, pk);
		augmentedF2 = generateAugmentedF2(C, K, pk);
		concealedF1 = generateConcealedF1(C, pk);
	}

/* Generation of individual components */

	/*
     * Function: generateHashMatrix(C, pk)
     * Returns the matrix portion of the hash function
     * Applied to x concatenated with y
     */
	void generateHashMatrix(const BitMatrix<N, 2*N> & K, const PrivateKey<N> & pk){
		const BitMatrix<2*N> & Mi = pk.getM().inv();
		const BitMatrix<N, 2*N> & Mi1 = Mi.splitV2(0);

		const BitMatrix<N, 2*N> & decryptMatrix = Mi1;

		const BitMatrix<N, 2*N> & zero = BitMatrix<N, 2*N>::zeroMatrix();
		const BitMatrix<N, 4*N> & top = BitMatrix<N, 4*N>::augH(decryptMatrix, zero);
		const BitMatrix<N, 4*N> & bot = BitMatrix<N, 4*N>::augH(zero, decryptMatrix);
		hashMatrix = K * (BitMatrix<2*N, 4*N>::augV(top, bot));
	}

	/*
     * Function: generateAugmentedF2(C, K, pk)
     * Returns the K (f2 C || f2 C) portion of the hash function
     * Applied to concealedF1(x) concatenated with concealedF1(y)
     */
	const MultiQuadTuple<2*N, N> generateAugmentedF2(const BitMatrix<N> & C, const BitMatrix<N, 2*N> & K, const PrivateKey<N> & pk) const{
		MultiQuadTuple<N, N> f2 = pk.getf().get(1);
		MultiQuadTuple<N, N> topBot = f2 * C;

        const BitMatrix<N> & I = BitMatrix<N>::identityMatrix();
        const BitMatrix<N> & O = BitMatrix<N>::zeroMatrix();
        const BitMatrix<N, 2*N> & P1 = BitMatrix<N, 2*N>::augH(I, O);
        const BitMatrix<N, 2*N> & P2 = BitMatrix<N, 2*N>::augH(O, I);

        MultiQuadTuple<2*N, 2*N> augmentedDecrypt;
        augmentedDecrypt.template augV<N, N>(topBot * P1, topBot * P2);
		return augmentedDecrypt.rMult(K);
	}

	/*
     * Function: generateConcealedF1(C, pk)
     * Returns the C^{-1} f1 portion of the hash function
     * Applied to x and y separately
     */
	 const MultiQuadTuple<2*N, N> generateConcealedF1(const BitMatrix<N> & C, const PrivateKey<N> & pk) const{
		MultiQuadTuple<N, N> f1 = pk.getf().get(0);
		const BitMatrix<N, 2*N> & inner = pk.getM().inv().splitV2(1);
		return (f1 * inner).rMult(C.inv());
	}

/* Evaluation */

	/*
	 * Function: operator(eSearchToken, eObjSearchKey)
	 * Returns the address value given an encrypted search token and an encrypted objectSearchKey
	 */
	const BitVector<N> operator()(const BitVector<2*N> & eSearchToken, const BitVector<2*N> & eObjSearchKey) const{
		const BitVector<N> & hashMatrixOutput = hashMatrix * BitVector<4*N>::vCat(eSearchToken, eObjSearchKey);
		const BitVector<2*N> & augmentedOutputF1 = BitVector<2*N>::vCat(concealedF1(eSearchToken), concealedF1(eObjSearchKey));
		const BitVector<N> & functionalOutput = augmentedF2(augmentedOutputF1);
		return hashMatrixOutput ^ functionalOutput;
	}
};

#endif