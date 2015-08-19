//
//  MultiQuadTuple.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 7/29/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  Implementation of tuples of multivariate quadratic polynomials over F2
//

#ifndef __krypto__MultiQuadTuple__
#define __krypto__MultiQuadTuple__

#include "BitMatrix.h"

#define NUM_INPUT_MONOMIALS ((NUM_INPUTS * (NUM_INPUTS + 1)) >> 1)

template<unsigned int NUM_INPUTS, unsigned int NUM_OUTPUTS>
class MultiQuadTuple {
public:
/* Constructor */

	/*
	 * Constructor
	 * Constructs a MultiQuadTuple with given contribution matrix and constant vector (which represents the constant terms).
	 */
	MultiQuadTuple(const BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS> & contributions, const BitVector<NUM_OUTPUTS> & constants):
	_contributions(contributions), _constants(constants){}

/* Generation */

	/*
	 * Function: randomMultiQuadTuple()
	 * Generates a random MultiQuadTuple of the given numbers of input and output.
	 */
	const static MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> randomMultiQuadTuple(){
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(
			BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS>::randomMatrix(),
			BitVector<NUM_OUTPUTS>::randomVector());
	}

	/*
	 * Function: zeroMultiQuadTuple()
	 * Generates a zero MultiQuadTuple of the given numbers of input and output.
	 */
	const static MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> zeroMultiQuadTuple(){
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(
			BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS>::zeroMatrix(),
			BitVector<NUM_OUTPUTS>::zeroVector());
	}

	/*
	 * Function: getMultiQuadTuple()
	 * Generates a MultiQuadTuple equivalent to a given linear transformation.
	 */
	const static MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> getMultiQuadTuple(const BitMatrix<NUM_OUTPUTS, NUM_INPUTS> & M){
		BitMatrix<NUM_INPUTS, NUM_OUTPUTS> Mt = M.template transpose<NUM_OUTPUTS>();
		BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS> result = BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS>::zeroMatrix();
		unsigned int count = 0;
		for(int limit = NUM_INPUTS; limit > 0; --limit){
			result.setRow(count, Mt.getRow(NUM_INPUTS - limit));
			count += limit;
		}
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(result, BitVector<NUM_OUTPUTS>::zeroVector());
	}

/* Evaluation */

	/*
	 * Function: ()
	 * Return the evaluation result on a given BitVector.
	 */
	const BitVector<NUM_OUTPUTS> operator()(const BitVector<NUM_INPUTS> & input) const {
		BitVector<NUM_OUTPUTS> result = BitVector<NUM_OUTPUTS>::zeroVector();
		unsigned int count = 0;
		for(unsigned int i = 0; i < NUM_INPUTS; ++i){
			if(input[i]){
				result ^= _contributions.getRow(count);
				++count;
				for(unsigned int j = i+1; j < NUM_INPUTS; ++j){
					if(input[j])result ^= _contributions.getRow(count);
					++count;
				}
			} else count += (NUM_INPUTS - i);
		}
		return result ^ _constants;
	}

/* Composition with Linear Transformation */

	/*
	 * Operator: *(C)
	 * Returns the MultiQuadTuple as a result of left composition with a given linear transformation.
	 */
	template<unsigned int NUM_INNER_INPUTS>
	const MultiQuadTuple<NUM_INNER_INPUTS, NUM_OUTPUTS> operator*(const BitMatrix<NUM_INPUTS, NUM_INNER_INPUTS> & inner) const{
		BitMatrix<NUM_INPUTS> innerTranspose = inner.template transpose<NUM_INPUTS>();
		const unsigned int NUM_INNER_INPUT_MONOMIALS = MultiQuadTuple<NUM_INNER_INPUTS, NUM_OUTPUTS>::getInputMonomialCount();
		BitMatrix<NUM_INNER_INPUT_MONOMIALS, NUM_OUTPUTS> result = BitMatrix<NUM_INNER_INPUT_MONOMIALS, NUM_OUTPUTS>::zeroMatrix();
		unsigned int input_count = 0;
		unsigned int limit = NUM_INPUTS;
		for(int i = 0; i < NUM_INPUTS; ++i){
			BitMatrix<NUM_OUTPUTS> Ai = innerTranspose.template pMult<NUM_OUTPUTS>(_contributions, i, NUM_INPUTS - 1, input_count, input_count + limit - 1);
			input_count += limit;
			--limit;
			for(unsigned int j = 0; j < NUM_INNER_INPUTS; ++j){
				if(inner.get(i, j)){
					for(unsigned int k = 0; k < NUM_INNER_INPUTS; ++k){
						unsigned int newIndex = MultiQuadTuple<NUM_INNER_INPUTS, NUM_OUTPUTS>::getIndex(j, k);
						result.setRow(newIndex, result.getRow(newIndex) ^ Ai.getRow(k));
					}
				}
			}
		}
		return MultiQuadTuple<NUM_INNER_INPUTS, NUM_OUTPUTS>(result, _constants);
	}

	/*
	 * Function: rMult(C)
	 * Returns the MultiQuadTuple as a result of right composition with a given linear transformation.
	 */
	template<unsigned int NUM_OUTER_OUTPUTS>
	const MultiQuadTuple<NUM_INPUTS, NUM_OUTER_OUTPUTS> rMult(
		const BitMatrix<NUM_OUTER_OUTPUTS, NUM_OUTPUTS> & outer) const{
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTER_OUTPUTS>(_contributions * (outer.template transpose<NUM_OUTER_OUTPUTS>()), outer.template operator*<NUM_OUTER_OUTPUTS>(_constants));
	}

/* MultiQuadTuple-MultiQuadTuple Operations */

	/*
	 * Operator: ^
	 * Returns a MultiQuadTuple with values resulting from bitwise XOR
	 */
	const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> operator^(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> & rhs) const {
 		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(_contributions ^ rhs._contributions, _constants ^ rhs._constants);
	}

	/*
	 * Function: augV(f1, f2)
	 * Returns the matrix resulting from vertical augmentation of two given MultiQuadTuples
	 */
	template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2> //augV
	static const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1+NUM_OUTPUTS2> augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1> & f1, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2> & f2){
		BitMatrix<NUM_OUTPUTS1> C1 = f1.getContributions();
		BitMatrix<NUM_OUTPUTS2> C2 = f2.getContributions();
		BitVector<NUM_OUTPUTS1> c1 = f1.getConstants();
		BitVector<NUM_OUTPUTS2> c2 = f2.getConstants();
		const unsigned int NUM_OUTPUTS_SUM = NUM_OUTPUTS1 + NUM_OUTPUTS2;
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS_SUM>(BitMatrix<NUM_OUTPUTS_SUM>::augH(C1, C2), BitVector<NUM_OUTPUTS_SUM>::template vCat<NUM_OUTPUTS1, NUM_OUTPUTS2>(c1, c2));
	}

	/*
	 * Function: augV(f1, f2, f3)
	 * Returns the matrix resulting from vertical augmentation of three given MultiQuadTuples
	 */
	template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2, unsigned int NUM_OUTPUTS3> //augV
	static const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1+NUM_OUTPUTS2+NUM_OUTPUTS3> augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1> & f1, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2> & f2, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS3> & f3){
		BitMatrix<NUM_OUTPUTS1> C1 = f1.getContributions();
		BitMatrix<NUM_OUTPUTS2> C2 = f2.getContributions();
		BitMatrix<NUM_OUTPUTS2> C3 = f3.getContributions();
		BitVector<NUM_OUTPUTS1> c1 = f1.getConstants();
		BitVector<NUM_OUTPUTS2> c2 = f2.getConstants();
		BitVector<NUM_OUTPUTS2> c3 = f3.getConstants();
		const unsigned int NUM_OUTPUTS_SUM = NUM_OUTPUTS1 + NUM_OUTPUTS2 + NUM_OUTPUTS3;
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS_SUM>(BitMatrix<NUM_OUTPUTS_SUM>::augH(C1, C2, C3), BitVector<NUM_OUTPUTS_SUM>::vCat(c1, c2, c3));
	}

/* Shifter */

	/*
	 * Function: leftShift()
	 * Generates a shifter function such that g(x << 1) === f(x) for g = f.leftShift()
	 */
	const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> leftShift() const{
		BitMatrix<NUM_OUTPUTS> newContributions = MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>::zeroContributionMatrix();
		unsigned int oldCount = NUM_INPUTS; //totally skip the coefficients for x_1
		unsigned int newCount = 0;
		for(unsigned int i = 1; i < NUM_INPUTS; ++i){
			for(unsigned int j = i; j < NUM_INPUTS; ++j){
				newContributions.setRow(newCount, _contributions.getRow(oldCount));
				++oldCount;
				++newCount;
			}
			++newCount;
		}
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(newContributions, _constants/*.leftShift()*/);
	}


/* Access */

	/*
	 * Function: getInputMonomoialCount
	 * Returns the number of monomials (of the form x_i, x_jx_k (j != k)) of inputs of a MultiQuadTuple
	 */
	static const unsigned int getInputMonomialCount(){
		return NUM_INPUT_MONOMIALS;
	}

	/*
	 * Function: getTransposeContributionMatrix
	 * Returns the transposed contribution matrix of a MultiQuadTuple
	 */
	const BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS> getContributions() const{
		return _contributions;
	}

	/*
	 * Function: getConstants
	 * Returns the BitVector that represents the constant terms of a MultiQuadTuple
	 */
	const BitVector<NUM_OUTPUTS> getConstants() const{
		return _constants;
	}

	/*
	 * Function: getIndex
	 * Returns the index of x_{main}x_{aux} in the monomial list.
	 */
	static const unsigned int getIndex(unsigned int main, unsigned int aux){
		unsigned int index1 = min(main, aux);
		unsigned int index2 = max(main, aux);
		unsigned int toSubtract = (((NUM_INPUTS - index1) * (NUM_INPUTS - index1 + 1)) >> 1);
		unsigned int result = NUM_INPUT_MONOMIALS - toSubtract + (index2 - index1);
		return result;
	}
private:
	BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS> _contributions;
	BitVector<NUM_OUTPUTS> _constants; //constant terms

	const static BitMatrix<NUM_OUTPUTS> zeroContributionMatrix(){
		return BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS>::zeroMatrix();
	}
};
#endif