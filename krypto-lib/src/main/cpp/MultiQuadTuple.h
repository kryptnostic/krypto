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
//#include <emscripten/bind.h>

#define DEBUG false

template<unsigned int NUM_INPUTS, unsigned int NUM_OUTPUTS>
class MultiQuadTuple {
public:
/* Constructor */	

	/*
	 * Constructor
	 * Constructs a MultiQuadTuple with given contribution matrix and constant vector (which represents the constant terms).
	 */
	MultiQuadTuple(const BitMatrix<NUM_OUTPUTS> & contributionsT, const BitVector<NUM_OUTPUTS> & constants):
	_contributionsT(contributionsT), _constants(constants){}

/* Generation */

	/*
	 * Function: randomMultiQuadTuple()
	 * Generates a random MultiQuadTuple of the given numbers of input and output.
	 */
	const static MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> randomMultiQuadTuple(){
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(BitMatrix<NUM_OUTPUTS>::randomMatrix(numInputMonomials), BitVector<NUM_OUTPUTS>::randomVector());
	}

	/*
	 * Function: zeroMultiQuadTuple()
	 * Generates a zero MultiQuadTuple of the given numbers of input and output.
	 */
	const static MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> zeroMultiQuadTuple(){
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(BitMatrix<NUM_OUTPUTS>::zeroMatrix(numInputMonomials), BitVector<NUM_OUTPUTS>::zeroVector());
	}

	/*
	 * Function: getMultiQuadTuple()
	 * Generates a MultiQuadTuple equivalent to a given linear transformation.
	 */
	const static MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> getMultiQuadTuple(const BitMatrix<NUM_INPUTS> & M){
		if(DEBUG) assert(NUM_OUTPUTS << 6 == M.rowCount());
		BitMatrix<NUM_OUTPUTS> Mt = M.template T<NUM_OUTPUTS>();
		BitMatrix<NUM_OUTPUTS> result = BitMatrix<NUM_OUTPUTS>::zeroMatrix(numInputMonomials);
		size_t count = 0;
		for(size_t i = 0; i < numInputBits; ++i){
			result.setRow(count, Mt.getRow(i));
			count += (numInputBits - i);
		}
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(result, BitVector<NUM_OUTPUTS>::zeroVector());
	}	

/* Evaluation */

	/*
	 * Function: ()
	 * Return the evaluation result on a given BitVector.
	 */
	const BitVector<NUM_OUTPUTS> operator()(const BitVector<NUM_INPUTS> & input) const {
		BitVector<NUM_OUTPUTS> result;
		unsigned int count = 0;
		for(size_t i = 0; i < numInputBits; ++i){
			if(input[i]){
				result ^= _contributionsT.getRow(count);
				++count;
				for(size_t j = i+1; j < numInputBits; ++j){
					if(input[j])result ^= _contributionsT.getRow(count);
					++count;
				}	
			} else count += (numInputBits - i);
		}
		return result ^ _constants;
	}

/* Composition with Linear Transformation */

	/*
	 * Operator: *(C)
	 * Returns the MultiQuadTuple as a result of left composition with a given linear transformation.
	 */
	template<unsigned int NUM_INNERINPUTS> 
	const MultiQuadTuple<NUM_INNERINPUTS, NUM_OUTPUTS> operator*(const BitMatrix<NUM_INNERINPUTS> & C) const{
		if(DEBUG) assert(numInputBits == C.rowCount());
		BitMatrix<NUM_INPUTS> Ct = C.template T<NUM_INPUTS>();
		const unsigned int numInnerInputMonomials = MultiQuadTuple<NUM_INNERINPUTS, NUM_OUTPUTS>::getInputMonomialCount(); 
		const unsigned int numInnerInputBits = MultiQuadTuple<NUM_INNERINPUTS, NUM_OUTPUTS>::getInputCount();
		BitMatrix<NUM_OUTPUTS> result(numInnerInputMonomials);
		size_t input_count = 0;
		size_t subLength = numInputBits;
		for(size_t i = 0; i < numInputBits; ++i){ 
			BitMatrix<NUM_OUTPUTS> Ai = Ct.template pMult<NUM_OUTPUTS>(_contributionsT, i, numInputBits - 1, input_count, input_count + subLength - 1);
			input_count += subLength;
			--subLength;
			for(size_t j = 0; j < numInnerInputBits; ++j){
				if(C.get(i, j)){ 
					for(size_t k = 0; k < numInnerInputBits; ++k){
						size_t newIndex = MultiQuadTuple<NUM_INNERINPUTS, NUM_OUTPUTS>::getIndex(j, k); 
						result.setRow(newIndex, result.getRow(newIndex) ^ Ai.getRow(k));
					}
				}
			}
		}
		return MultiQuadTuple<NUM_INNERINPUTS, NUM_OUTPUTS>(result, _constants);
	}

	/*
	 * Function: rMult(C)
	 * Returns the MultiQuadTuple as a result of right composition with a given linear transformation.
	 */
	template<unsigned int NUM_OUTEROUTPUTS>
	const MultiQuadTuple<NUM_INPUTS, NUM_OUTEROUTPUTS> rMult(const BitMatrix<NUM_OUTPUTS> & C) {
		if(DEBUG) assert(C.rowCount() == NUM_OUTEROUTPUTS << 6); 
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTEROUTPUTS>(_contributionsT * (C.template T<NUM_OUTEROUTPUTS>()), C.template operator*<NUM_OUTEROUTPUTS>(_constants));
	}

/* MultiQuadTuple-MultiQuadTuple Operations */

	/*
	 * Operator: ^
	 * Returns a MultiQuadTuple with values resulting from bitwise XOR
	 */
	const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> operator^(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> & rhs) const {
 		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(_contributionsT ^ rhs._contributionsT, _constants ^ rhs._constants);
	}

	/*
	 * Function: augV(f1, f2)
	 * Returns the matrix resulting from vertical augmentation of two given MultiQuadTuples
	 */
	template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2> //augV
	static const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1+NUM_OUTPUTS2> augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1> & f1, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2> & f2){
		BitMatrix<NUM_OUTPUTS1> C1 = f1.getTransposedContributionMatrix();
		BitMatrix<NUM_OUTPUTS2> C2 = f2.getTransposedContributionMatrix();
		BitVector<NUM_OUTPUTS1> c1 = f1.getConstantTerms();
		BitVector<NUM_OUTPUTS2> c2 = f2.getConstantTerms(); 
		const unsigned int NUM_OUTPUTS_SUM = NUM_OUTPUTS1 + NUM_OUTPUTS2;
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS_SUM>(BitMatrix<NUM_OUTPUTS_SUM>::augH(C1, C2), BitVector<NUM_OUTPUTS_SUM>::template vCat<NUM_OUTPUTS1, NUM_OUTPUTS2>(c1, c2));
	}	

	/*
	 * Function: augV(f1, f2, f3)
	 * Returns the matrix resulting from vertical augmentation of three given MultiQuadTuples
	 */
	template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2, unsigned int NUM_OUTPUTS3> //augV
	static const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1+NUM_OUTPUTS2+NUM_OUTPUTS3> augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1> & f1, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2> & f2, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS3> & f3){
		BitMatrix<NUM_OUTPUTS1> C1 = f1.getTransposedContributionMatrix();
		BitMatrix<NUM_OUTPUTS2> C2 = f2.getTransposedContributionMatrix();
		BitMatrix<NUM_OUTPUTS2> C3 = f3.getTransposedContributionMatrix();
		BitVector<NUM_OUTPUTS1> c1 = f1.getConstantTerms();
		BitVector<NUM_OUTPUTS2> c2 = f2.getConstantTerms();
		BitVector<NUM_OUTPUTS2> c3 = f3.getConstantTerms(); 
		const unsigned int NUM_OUTPUTS_SUM = NUM_OUTPUTS1 + NUM_OUTPUTS2 + NUM_OUTPUTS3;
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS_SUM>(BitMatrix<NUM_OUTPUTS_SUM>::augH(C1, C2, C3), BitVector<NUM_OUTPUTS_SUM>::vCat(c1, c2, c3));
	}

/* Shifter */
	
	/*
	 * Function: leftShift()
	 * Generates a shifter function such that g(x << 1) === f(x) for g = f.leftShift()
	 */
	const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> leftShift() const{
		BitMatrix<NUM_OUTPUTS> newContributionT = MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>::zeroContributionMatrix();
		unsigned int oldCount = numInputBits; //totally skip the coefficients for x_1
		unsigned int newCount = 0;
		for(size_t i = 1; i < numInputBits; ++i){

			for(size_t j = i; j < numInputBits; ++j){
				newContributionT.setRow(newCount, _contributionsT.getRow(oldCount));
				++oldCount; 
				++newCount;
			}		
			++newCount;
		}
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(newContributionT, _constants/*.leftShift()*/);
	}
	

/* Access */

	/*
	 * Function: getInputCount
	 * Returns the dimension of inputs of a MultiQuadTuple
	 */
	static const unsigned int getInputCount(){
		return numInputBits;
	}

	/*
	 * Function: getInputMonomoialCount
	 * Returns the number of monomials (of the form x_i, x_jx_k (j != k)) of inputs of a MultiQuadTuple
	 */
	static const unsigned int getInputMonomialCount(){
		return numInputMonomials;
	}	

	/*
	 * Function: getTransposeContributionMatrix
	 * Returns the transposed contribution matrix of a MultiQuadTuple
	 */
	const BitMatrix<NUM_OUTPUTS> getTransposedContributionMatrix() const{
		return _contributionsT;
	}

	/*
	 * Function: getConstantTerms
	 * Returns the BitVector that represents the constant terms of a MultiQuadTuple
	 */
	const BitVector<NUM_OUTPUTS> getConstantTerms() const{
		return _constants;
	}

	/*
	 * Function: getIndex
	 * Returns the index of x_{main}x_{aux} in the monomial list.
	 */
	static const unsigned int getIndex(unsigned int main, unsigned int aux){
		size_t index1 = min(main, aux);
		size_t index2 = max(main, aux);
		unsigned int toSubtract = (((numInputBits - index1) * (numInputBits - index1 + 1)) >> 1);
		unsigned int result = numInputMonomials - toSubtract + (index2 - index1);
		return result;
	}		
private:
	BitMatrix<NUM_OUTPUTS> _contributionsT;
	BitVector<NUM_OUTPUTS> _constants; //constant term
	static const unsigned int numInputBits = (NUM_INPUTS << 6);
	static const unsigned int numInputMonomials = ((numInputBits * (numInputBits + 1)) >> 1);

	const static BitMatrix<NUM_OUTPUTS> zeroContributionMatrix(){
		return BitMatrix<NUM_OUTPUTS>::zeroMatrix(numInputMonomials);	
	}
};
#endif