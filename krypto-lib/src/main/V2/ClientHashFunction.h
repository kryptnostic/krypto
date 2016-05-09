//
//  ClientHashFunction.h
//  krypto
//
//  Created by Robin Zhang on 8/21/15.
//	Modified by njlai
//  Copyright (c) 2016 Kryptnostic. All rights reserved.
//
//  C++ struct concatenating the three components of the ClientHashFunction h_\Omega
//

#ifndef ClientHashFunction_h
#define ClientHashFunction_h

#include "PrivateKey.h"
#include "MonomialMatrix.h"
#include <iostream>

using namespace std;

template<unsigned int N>
struct ClientHashFunction
{
/* Data */

	BitMatrix<N, 4*N> hashMatrix;
	BitMatrix<N,2*N> augmentedK;
	ConstantChainHeader<2*N, 2*N> augmentedF;
	BitMatrix<2*N,4*N> concealedMatrix;

/* Initialization */

	/*
     * Function: initialize(K, pk)
     * Initializes a ClientHashFunction with given private matrices and privateKey
     */
    template<unsigned int MON>
	void initialize(const BitMatrix<N, 2*N> & K, const PrivateKey<N,MON> & pk){
		generateHashMatrix(K, pk);
		generateAugmentedParameters(urandom, K, pk);
	}

/* Generation of individual components */

	/*
     * Function: generateHashMatrix(K, pk)
     * Returns the matrix portion of the hash function
     * Applied to x concatenated with y
     */
    template<unsigned int MON>
	void generateHashMatrix(const BitMatrix<N, 2*N> & K, const PrivateKey<N,MON> & pk){
		const BitMatrix<2*N> & Mi = pk.getM().inv();
		const BitMatrix<N, 2*N> & Mi1 = Mi.splitV2(0);

		const BitMatrix<N, 2*N> & decryptMatrix = Mi1;

		const BitMatrix<N, 2*N> & zero = BitMatrix<N, 2*N>::zeroMatrix();
		const BitMatrix<N, 4*N> & top = BitMatrix<N, 4*N>::augH(decryptMatrix, zero);
		const BitMatrix<N, 4*N> & bot = BitMatrix<N, 4*N>::augH(zero, decryptMatrix);
		hashMatrix = K * (BitMatrix<2*N, 4*N>::augV(top, bot));
	}

	/*
     * Function: generateAugmentedParameters(K, pk)
     * Sets augmentedK = K * C_2^{-1}, augmentedF = C_2 \circ [f | f] \circ C_1^{-1}, concealedMatrix = C * augV([Mi2 | 0], [0 | Mi2]
     * Applied to x concatenated with y
     */
    template<unsigned int MON>
	void generateAugmentedParameters(const BitMatrix<N,2*N> & K, const PrivateKey<N,MON> & pk) {
		const SecretPolynomial<N,N,MON> f = pk.getf();
		const BitMatrix<N> Ai = f.getLinearFactors(0).inv();

		const BitMatrix<2*N> stackAi = BitMatrix<2*N>::augV(
			BitMatrix<N,2*N>::augH(Ai, BitMatrix<N>::zeroMatrix()),
			BitMatrix<N,2*N>::augH(BitMatrix<N>::zeroMatrix(), Ai));
		const BitMatrix<2*N> C2 = BitMatrix<2*N>::generateSparseInvertibleMatrix(1);
		const BitMatrix<2*N> B = BitMatrix<2*N>::augV(
			BitMatrix<N,2*N>::augH(BitMatrix<N>::generateSparseInvertibleMatrix(2), BitMatrix<N>::zeroMatrix()),
			BitMatrix<N,2*N>::augH(BitMatrix<N>::zeroMatrix(), BitMatrix<N>::generateSparseInvertibleMatrix(2))).transpose();
		const BitMatrix<2*N> C1i = (B * stackAi).transpose();
		SecretPolynomial<2*N,2*N,MON> G = SecretPolynomial<2*N,2*N,MON>::permute(SecretPolynomial<2*N,2*N,MON>::getUnaryF(f) * C1i, C2);

		const BitMatrix<N,2*N> & Mi2 = pk.getM().inv().splitV2(1);
		concealedMatrix = C1i.inv() * BitMatrix<2*N,4*N>::augV(
			BitMatrix<N,4*N>::augH(Mi2, BitMatrix<N,2*N>::zeroMatrix()),
			BitMatrix<N,4*N>::augH(BitMatrix<N,2*N>::zeroMatrix(), Mi2));
		augmentedK = K * C2.inv();

		augmentedF = G.convertToPublicPolynomial();
	}

/* Evaluation */

	/*
	 * Function: operator(eSearchToken, eObjSearchKey)
	 * Returns the address value given an encrypted search token and an encrypted objectSearchKey
	 */
	const BitVector<N> operator()(const BitVector<2*N> & eSearchToken, const BitVector<2*N> & eObjSearchKey) const{
		const BitVector<N> & hashMatrixOutput = hashMatrix * BitVector<4*N>::vCat(eSearchToken, eObjSearchKey);
		const BitVector<2*N> & augmentedOutputF1 = concealedMatrix * BitVector<4*N>::vCat(eSearchToken, eObjSearchKey);
		const BitVector<N> & functionalOutput = augmentedK * augmentedF(augmentedOutputF1);
		return hashMatrixOutput ^ functionalOutput;
	}

	~ClientHashFunction() {
		// freeAllNext must be called to properly delete ConstantHeaderChain
		augmentedF.getMMC().freeAllNext();
	}
};

#endif