#ifndef __krypto__MultivariateQuadraticFunctionTuple__
#define __krypto__MultivariateQuadraticFunctionTuple__

#include "BitMatrix.h"

using namespace std;
template<unsigned int NUM_INPUTS, unsigned int NUM_OUTPUTS>
class MultivariateQuadraticFunctionTuple {
public:
	//f(x) = A^T\tilde{x} (did not do A\tilde{x} because if that's the case we cannot assure that 
	//the number of cols in A has a multiple of 64; in particular, 64|A.numCols() iff 2|NUM_INPUTS)
	MultivariateQuadraticFunctionTuple(const BitMatrix<NUM_OUTPUTS> & contributions):
	_contributions(contributions){
	}

	//contributions A =  F_2{(n_i+1)C2 \times n_o}, so that f(x) = A^T\tilde{x}
	const static MultivariateQuadraticFunctionTuple<NUM_INPUTS, NUM_OUTPUTS> randomMultivariateQuadraticFunctionTuple(){
		MultivariateQuadraticFunctionTuple<NUM_INPUTS, NUM_OUTPUTS> f(BitMatrix<NUM_OUTPUTS>::randomMatrix(monomialCount));
		return f;
	}

	const BitMatrix<NUM_OUTPUTS> getPaddedContribution() const{
		if(!needPadding) return _contributions;
		return BitMatrix<NUM_OUTPUTS>::aug_v(_contributions, BitMatrix<NUM_OUTPUTS>::zeroMatrix(32));
	}

    const BitVector<NUM_OUTPUTS> operator()(const BitVector<NUM_INPUTS> & input) const {
		const unsigned int TRANSFORMED_INPUT = paddedMonomialCount/64;
		BitVector<TRANSFORMED_INPUT> t = BitVector<TRANSFORMED_INPUT>::zeroVector();
		int count = 0;
		for(int i = 0; i < numInputBits; ++i){ //building (padded) \tilde{x}
			t[count] = input[i];//x_i
			++count;
			for(int j = i+1; j < numInputBits; ++j){
				t[count] = input[i]^input[j];
				++count;
			}
		}
		BitMatrix<NUM_OUTPUTS> paddedContributions = getPaddedContribution();
		return paddedContributions.tMult(t); 
	}

private:
	BitMatrix<NUM_OUTPUTS> _contributions;
	static const bool needPadding = (NUM_INPUTS % 2 == 0);
	static const unsigned int numInputBits = (NUM_INPUTS << 6);
	static const unsigned int numOutputBits = (NUM_OUTPUTS << 6);
	static const unsigned int monomialCount = numInputBits + ((numInputBits * (numInputBits - 1)) >> 1);
	static const unsigned int paddedMonomialCount = needPadding ? monomialCount : monomialCount + 32;
};

#endif