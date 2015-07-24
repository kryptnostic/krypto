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

	const static MultivariateQuadraticFunctionTuple<NUM_INPUTS, NUM_OUTPUTS> randomMultivariateQuadraticFunctionTuple(){
		BitMatrix<NUM_OUTPUTS> contributions = BitMatrix<NUM_OUTPUTS>::randomMatrix(monomialCount);
		if(monomialCount == paddedMonomialCount) return contributions;
		BitMatrix<NUM_OUTPUTS> zero = BitMatrix<NUM_OUTPUTS>::zeroMatrix(32);
		return BitMatrix<NUM_OUTPUTS>::aug_v(contributions, zero);
	}
/*
	//implementation in progress
    const BitVector<NUM_OUTPUTS> operator()(const BitVector<NUM_INPUTS> & input) const {
		int TRANSFORMED_INPUT = paddedMonomialCount/64;
		BitVector<TRANSFORMED_INPUT> t = BitVector<TRANSFORMED_INPUT>::zeroVector();
		int count = 0;
		for(int i = 0; i < numInputBits; ++i){
			t[count] = input[i];//x_i
			++count;
			for(int j = i+1; j < numInputBits; ++j){
				t[count] = input[i]^input[j];
				++count;
			}
		}
		BitMatrix<
		return _paddedContributions ** t; //todo: implement ** (M**x := M^T() * x)
	}
*/
private:
	BitMatrix<NUM_OUTPUTS> _contributions;
	static const unsigned int numInputBits = (NUM_INPUTS << 6);
	static const unsigned int numOutputBits = (NUM_OUTPUTS << 6);
	static const unsigned int monomialCount = numInputBits + ((numInputBits * (numInputBits - 1)) >> 1);
	static const unsigned int paddedMonomialCount = (monomialCount % 64 == 0) ? monomialCount : monomialCount + 32;
};

#endif