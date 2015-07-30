#ifndef __krypto__MultiQuadTuple__
#define __krypto__MultiQuadTuple__

#include "BitMatrix.h"

using namespace std;
template<unsigned int NUM_INPUTS, unsigned int NUM_OUTPUTS>
class MultiQuadTuple {
public:
	MultiQuadTuple(const BitMatrix<NUM_OUTPUTS> & contributionsT):_contributionsT(contributionsT){}

	const static MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> randomMultiQuadTuple(){
		if(!inputNeedPadding) return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(BitMatrix<NUM_OUTPUTS>::randomMatrix(inputMonomialCount));
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(BitMatrix<NUM_OUTPUTS>::aug_v(BitMatrix<NUM_OUTPUTS>::randomMatrix(inputMonomialCount), BitMatrix<NUM_OUTPUTS>::zeroMatrix(32)));
	}

	/**Functional evaluation**/

    const BitVector<NUM_OUTPUTS> operator()(const BitVector<NUM_INPUTS> & input) const {
 		return _contributionsT.tMult(getTransformedInput(input));
	}

	/**Functional compositions**/

	/**
	 * Fuctional tuple evaluation composed with left matrix multiplication, e.g.:
	 * MultiQuadTuple<N, M> f = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	 * BitMatrix<L> C = BitMatrix<L>::randomMatrix(N << 6);
	 * MultiQuadTuple<L, M> fC = f*C; //not using f(C) to avoid confusion with contribution matrix input variable
	 */
	template<unsigned int NUM_INNERINPUTS> 
	const MultiQuadTuple<NUM_INNERINPUTS, NUM_OUTPUTS> operator*(const BitMatrix<NUM_INNERINPUTS> & C) const{
		assert(NUM_INPUTS << 6 == C.colCount());
		return MultiQuadTuple<NUM_INNERINPUTS, NUM_OUTPUTS>(getTransposedCompositionMatrix(C) * _contributionsT);
	}

	/**
	 * Composition from the right, TODO: make this more user intuitive
	 * f.rMult(C) := Cf(*)
	 */
	template<unsigned int NUM_OUTEROUTPUTS>
	const MultiQuadTuple<NUM_INPUTS, NUM_OUTEROUTPUTS> rMult(const BitMatrix<NUM_OUTPUTS> & C) {
		assert(numOutputBits == C.colCount());
		assert(!(C.rowCount() & 63)); 
		BitMatrix<NUM_OUTEROUTPUTS> Ct( C.template T<NUM_OUTEROUTPUTS>() );
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTEROUTPUTS>(_contributionsT * Ct); //return MultiQuadTuple<NUM_INPUTS, NUM_OUTEROUTPUTS>(_contributionsT * C.T<NUM_ROWS>());
	}	
	/**
	 * Vertically concatenates 2 function tuples
	 * MultiQuadTuple<1,1> f1 = MultiQuadTuple<1,1>::randomMultiQuadTuple();
	 * MultiQuadTuple<1,2> f2 = MultiQuadTuple<1,1>::randomMultiQuadTuple();
	 * MultiQuadTuple<1,3> f = aug_v(f1,f2);
	 */
	template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2>
	static const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1+NUM_OUTPUTS2> aug_v(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1> & f1, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2> & f2){
		BitMatrix<NUM_OUTPUTS1> C1 = f1.getTransposedContributionMatrix();
		BitMatrix<NUM_OUTPUTS2> C2 = f2.getTransposedContributionMatrix();
		const unsigned int NUM_OUTPUTS_SUM = NUM_OUTPUTS1 + NUM_OUTPUTS2;
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS_SUM>(BitMatrix<NUM_OUTPUTS_SUM>::aug_h(C1, C2));
	}

	/**Accessing various dimensions (make private later)**/

	const unsigned int getInputPaddedMonomialCount() const{
		return inputPaddedMonomialCount;
	}

	const unsigned int getOutputPaddedMonomialCount() const{ 
		return outputPaddedMonomialCount;
	}

	const unsigned int getInputPaddedMonomialMultiple() const{//act as a template argument for BitMatrix
		return NUM_INPUT_MONOMIALS;
	}

	const unsigned int getOutputPaddedMonomialMultiple() const{//act as a template argument for BitMatrix
		return NUM_OUTPUT_MONOMIALS;
	}

	const BitMatrix<NUM_OUTPUTS> getTransposedContributionMatrix() const{
		return _contributionsT;
	}

private: 
	BitMatrix<NUM_OUTPUTS> _contributionsT;
	static const bool inputNeedPadding = NUM_INPUTS & 1; //note: 64 divides (64n)*(64n+1)/2 iff n===0(mod2)
	static const bool outputNeedPadding = NUM_OUTPUTS & 1; ////note: 64 divides (64n)*(64n+1)/2 iff n===0(mod2)
	static const unsigned int numInputBits = (NUM_INPUTS << 6);
	static const unsigned int numOutputBits = (NUM_OUTPUTS << 6);
	static const unsigned int inputMonomialCount = ((numInputBits * (numInputBits + 1)) >> 1);
	static const unsigned int outputMonomialCount = ((numOutputBits * (numOutputBits + 1)) >> 1);
	static const unsigned int inputPaddedMonomialCount = inputNeedPadding ? inputMonomialCount + 32 : inputMonomialCount;
	static const unsigned int outputPaddedMonomialCount = outputNeedPadding ? outputMonomialCount + 32: outputMonomialCount;
	static const unsigned int NUM_INPUT_MONOMIALS = inputPaddedMonomialCount >> 6; 
	static const unsigned int NUM_OUTPUT_MONOMIALS = outputPaddedMonomialCount >> 6;

	//returns (padded) transformed vector \tilde{x} given vector x
	const BitVector<NUM_INPUT_MONOMIALS> getTransformedInput(const BitVector<NUM_INPUTS> & x) const{
		BitVector<NUM_INPUT_MONOMIALS> t = BitVector<NUM_INPUT_MONOMIALS>::zeroVector();
		int count = 0;
		for(int i = 0; i < numInputBits; ++i){ //building (padded) \tilde{x}
			for(int j = i; j < numInputBits; ++j){
				t.set(count, x[i]&x[j]);
				++count;
			}
		}
		return t;
	}

	//returns matrix CC_T given matrix C
	template <unsigned int NUM_INNERINPUTS>
	const BitMatrix<NUM_INPUT_MONOMIALS> getTransposedCompositionMatrix(const BitMatrix<NUM_INNERINPUTS> & C) const{
		const unsigned int numInnerInputBits = NUM_INNERINPUTS << 6;
		const unsigned int innerInputMonomialCount = ((numInnerInputBits * (numInnerInputBits + 1)) >> 1);
		const unsigned int innerInputPaddedMonomialCount = (NUM_INNERINPUTS & 1) ? innerInputMonomialCount + 32 : innerInputMonomialCount; 
		BitMatrix<NUM_INPUT_MONOMIALS> CC_T = BitMatrix<NUM_INPUT_MONOMIALS>::zeroMatrix(innerInputPaddedMonomialCount); 
		int count_i = 0;
		for(size_t i = 0; i < numInputBits; ++i){ //across the rows of CC (columns of CC_T)
			for(size_t ii = i; ii < numInputBits; ++ii){
				int count_j = 0;
				for(size_t j = 0; j < numInnerInputBits; ++j){
					for(size_t jj = j; jj < numInnerInputBits; ++jj){
						if(i == ii){
							if(j == jj) CC_T.set(count_j, count_i, C.get(i, j)); //else the bit is zero
						} else {
							if(j == jj) CC_T.set(count_j, count_i, C.get(i, j) & C.get(ii, j));
							else CC_T.set(count_j, count_i, (C.get(i, j) & C.get(ii, jj)) ^ (C.get(i, jj) & C.get(ii, j)));
						}
						++count_j;						
					}
				}
				++count_i;
			}
		}
		return CC_T;
	}
};

#endif