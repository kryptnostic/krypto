//
//  MultivariatePolynomialFunction.h
//  krypto
//
//  Created by Matthew Tamayo on 12/12/14.
//  Copyright (c) 2014 Kryptnostic. All rights reserved.
//

//this seems like it is used for F_2^n[x1,...,xn]

#ifndef __krypto__MultivariatePolynomialFunction__
#define __krypto__MultivariatePolynomialFunction__

#include <vector>

#include "../../main/cpp/BitVector.h"

using namespace std;

template<unsigned int NUM_INPUTS, unsigned int NUM_OUTPUTS>
class MultivariatePolynomialFunction {
public:
	MultivariatePolynomialFunction(
			const vector<BitVector<NUM_INPUTS> > & monomials,
			const vector<BitVector<NUM_OUTPUTS> > & contributions) :
			_monomials(monomials), _contributions(contributions) {

	}

	MultivariatePolynomialFunction(
			const vector<BitVector<NUM_INPUTS>> && monomials,
			const vector<BitVector<NUM_OUTPUTS> > && contributions) :
			_monomials(monomials), _contributions(contributions) {

	}

	static MultivariatePolynomialFunction denseRandomMultivariateFunctions() {
		vector<BitVector<NUM_INPUTS> > _monomials(maxMonomialCount);
		vector<BitVector<NUM_OUTPUTS> > _contributions(maxMonomialCount);

		unsigned int index = 0;
		for (int i = 0; i < numInputBits; ++i) {
			for (int j = i; j < numInputBits; ++j) {
				BitVector<NUM_INPUTS> v;
				v.set(i).set(j);
				_monomials[index] = v;
				_contributions[index++] =
						BitVector<NUM_OUTPUTS>::randomVector();
			}

		}
		MultivariatePolynomialFunction<NUM_INPUTS, NUM_OUTPUTS> f(_monomials,
				_contributions);

		return f;
	}
      
    const BitVector<NUM_OUTPUTS> operator()(
			const BitVector<NUM_INPUTS> & input) {
		BitVector<NUM_OUTPUTS> result;
		for (int i = 0; i < maxMonomialCount; ++i) { 
			//_monomials (m) must have size maxMonomialCount = n + n(n-1)/2
			//_contributions (c) must have size maxMonomialCount = n + n(n-1)/2
			//In other words, m and c must have the same size     e size
			//This corresponds to the terms x_i and x_jx_k, for i in 1..n; (j<k) in 1..n 
			//Each entry in m (resp. c) corresponds to one of x_i or x_jx_k
			BitVector<NUM_INPUTS> inputMask = _monomials[i] & input;
			if (inputMask == input) { //the 'on'(1) bits of input is a subset of that of _monomials[i]
				result ^= _contributions[i]; //+ c[i] (this is c_i or c_jc_k)
			}
		}
		return result;
	}
    
    template<unsigned INNER_INPUT_LENGTH>
    const MultivariatePolynomialFunction<INNER_INPUT_LENGTH,NUM_OUTPUTS> operator()(
                                            const MultivariatePolynomialFunction<INNER_INPUT_LENGTH,NUM_INPUTS> & input) {
        BitVector<NUM_OUTPUTS> result;
        for (int i = 0; i < maxMonomialCount; ++i) {
            BitVector<NUM_INPUTS> inputMask = _monomials[i] & input;
            if (inputMask == input) {
                result ^= _contributions[i];
            }
        }
        return result;                                                                                                                                             
    }

private:
	vector<BitVector<NUM_INPUTS>> _monomials;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
	vector<BitVector<NUM_OUTPUTS>> _contributions;
	static const unsigned int numInputBits = (NUM_INPUTS << 6);
	static const unsigned int numOutputBits = (NUM_OUTPUTS << 6);
	static const unsigned int maxMonomialCount = numInputBits
			+ ((numInputBits * (numInputBits - 1)) >> 1);
};
#endif /* defined(__krypto__MultivariatePolynomialFunction__) */
