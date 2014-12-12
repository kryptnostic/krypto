//
//  MultivariatePolynomialFunction.h
//  krypto
//
//  Created by Matthew Tamayo on 12/12/14.
//  Copyright (c) 2014 Kryptnostic. All rights reserved.
//

#ifndef __krypto__MultivariatePolynomialFunction__
#define __krypto__MultivariatePolynomialFunction__

#include<vector>
#include "BitVector.h"

using namespace std;

template<unsigned int NUM_INPUTS, unsigned int NUM_OUTPUTS>
class MultivariatePolynomialFunction {
public:
    MultivariatePolynomialFunction( const vector<BitVector<NUM_INPUTS> > & monomials , const vector<BitVector<NUM_OUTPUTS> > & contributions ): _monomials( monomials ),_contributions( contributions ) {
        
    }
    
    MultivariatePolynomialFunction( const vector<BitVector<NUM_INPUTS >> && monomials , const vector<BitVector<NUM_OUTPUTS> > && contributions ): _monomials( monomials ),_contributions( contributions ) {

    }
    
    static MultivariatePolynomialFunction denseRandomMultivariateFunctions() {
        vector<BitVector<NUM_INPUTS> > _monomials( maxMonomialCount );
        vector<BitVector<NUM_OUTPUTS> > _contributions( maxMonomialCount );
        
        unsigned int index = 0;
        for( int i = 0 ; i <  numInputBits ; ++i ) {
            for( int j = i ; j < numInputBits; ++j ) {
                BitVector<NUM_INPUTS> v;
                v.set(i).set(j);
                _monomials[ index ] = v;
                _contributions[ index ++ ] = BitVector<NUM_OUTPUTS>::randomVector();
            }
            
        }
        MultivariatePolynomialFunction<NUM_INPUTS,NUM_OUTPUTS> f( _monomials,_contributions );
        
        return f;
    }
    
    const BitVector<NUM_OUTPUTS> operator()( const BitVector<NUM_INPUTS> & input ) {
        BitVector<NUM_OUTPUTS> result;
        for( int i = 0 ; i < maxMonomialCount; ++i ) {
            BitVector<NUM_INPUTS> inputMask = _monomials[ i ] & input;
            if( inputMask == input ) {
                result^=_contributions[ i ];
            }
        }
        return result;
    }
    
private:
    vector<BitVector<NUM_INPUTS>> _monomials;
    vector<BitVector<NUM_OUTPUTS>> _contributions;
    static const unsigned int numInputBits = (NUM_INPUTS << 6);
    static const unsigned int numOutputBits = (NUM_OUTPUTS << 6);
    static const unsigned int maxMonomialCount = numInputBits + ((numInputBits*(numInputBits-1))>>1);
};
#endif /* defined(__krypto__MultivariatePolynomialFunction__) */
