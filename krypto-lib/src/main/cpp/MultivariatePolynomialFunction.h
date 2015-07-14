//
//  MultivariatePolynomialFunction.h
//  krypto
//
//  Created by Matthew Tamayo on 12/12/14.
//  Copyright (c) 2014 Kryptnostic. All rights reserved.
//

#ifndef __krypto__MultivariatePolynomialFunction__
#define __krypto__MultivariatePolynomialFunction__

#include <vector>

#include "../../main/cpp/BitVector.h"

using namespace std;

template<unsigned int NUM_INPUTS, unsigned int NUM_OUTPUTS>
class MultivariatePolynomialFunction {
public:
	MultivariatePolynomialFunction( 
			const vector<BitVector<NUM_OUTPUTS> > & contributions) :
			_monomials(getMonomials()), _contributions(contributions) {
	}

	/**
	 * The constant monomial matrix
	 * e.g. if there are 3 variables (x_1,x_2,x_3), i.e. numInputBits = 3
	 * The resulting matrix would be
	 * [1 0 0] : x_1
	 * [1 1 0] : x_1x_2
	 * [1 0 1] : x_1x_3
	 * [0 1 0] : x_2
	 * [0 1 1] : x_2x_3
	 * [0 0 1] : x_3
	 */
	const vector<BitVector<NUM_INPUTS> > getMonomials(){
		vector<BitVector<NUM_INPUTS> > monomials(maxMonomialCount);
		int index = 0;
		for (int i = 0; i < numInputBits; ++i) {
			for (int j = i; j < numInputBits; ++j) {
				BitVector<NUM_INPUTS> v;
				v.set(i).set(j);
				monomials[index++] = v;
			}
		}	
		return monomials;	
	}

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

	//added
	//TODO: thorough test
	const MultivariatePolynomialFunction<NUM_INPUTS, NUM_OUTPUTS> operator^
		(const MultivariatePolynomialFunction<NUM_INPUTS, NUM_OUTPUTS> & rhs){
		MultivariatePolynomialFunction<NUM_INPUTS, NUM_OUTPUTS> result = rhs;
		result.xorContributions(_contributions);
		return result;
	}

	const MultivariatePolynomialFunction<NUM_INPUTS, NUM_OUTPUTS> operator^=
		(MultivariatePolynomialFunction<NUM_INPUTS, NUM_OUTPUTS> rhs){
		for (unsigned int i = 0; i < maxMonomialCount; ++i) {
			_contributions[i] ^= rhs.getContribution(i);
		}
		return *this;
	}
      
    const BitVector<NUM_OUTPUTS> operator()(
			const BitVector<NUM_INPUTS> & input) {
		BitVector<NUM_OUTPUTS> result;
		for (int i = 0; i < maxMonomialCount; ++i) { 
			BitVector<NUM_INPUTS> inputMask = _monomials[i] & input;
			if (inputMask == input) { 
			//if(inputMask == _monomials[i]){
				result ^= _contributions[i]; 
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
            //if(inputMask == _monomials[i]){
                result ^= _contributions[i];
            }
        }
        return result;                                                                                                                                             
    }

    static vector<BitVector<NUM_OUTPUTS>> getRandomContributions(){
    	vector<BitVector<NUM_OUTPUTS>> result(maxMonomialCount);
    	for(int i = 0; i < maxMonomialCount; ++i){
    		result[i] = BitVector<NUM_OUTPUTS>::randomVector();
    	}
    	return result;
    }

    static int getMaxQuadraticCount(){
    	return maxQuadraticCount;
    }

    static int getMaxMonomialCount(){
    	return maxMonomialCount;
    }

    vector<BitVector<NUM_OUTPUTS>> getContributions(){
    	return _contributions;
    }

    BitVector<NUM_OUTPUTS> getContribution(int index){
    	return _contributions[index];
    }

    void xorContributions(vector<BitVector<NUM_OUTPUTS>> c){
    	for(unsigned int i = 0; i < maxMonomialCount; ++i){
    		_contributions[i] ^= c[i];
    	}
    }


private:
	vector<BitVector<NUM_INPUTS>> _monomials;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
	vector<BitVector<NUM_OUTPUTS>> _contributions;
	static const unsigned int numInputBits = (NUM_INPUTS << 6);
	static const unsigned int numOutputBits = (NUM_OUTPUTS << 6);
	static const unsigned int maxMonomialCount = numInputBits + ((numInputBits * (numInputBits - 1)) >> 1);
	static const unsigned int maxQuadraticCount = (numInputBits * (numInputBits - 1)) >> 1;
};
#endif /* defined(__krypto__MultivariatePolynomialFunction__) */
