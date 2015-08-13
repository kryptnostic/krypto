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

#include "test_BitMatrix.h"

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
     * Function: zeroMultiQuadTuple()
     * Generates a zero MultiQuadTuple of the given numbers of input and output.
     */
    const static MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> zeroMultiQuadTuple(){
        return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(BitMatrix<NUM_OUTPUTS>::zeroMatrix(numInputMonomials), BitVector<NUM_OUTPUTS>::zeroVector());
    }

/* Composition with Linear Transformation */

    /*
     * Function: rMult(C)
     * Returns the MultiQuadTuple as a result of right composition with a given linear transformation.
     */
    template<unsigned int NUM_OUTEROUTPUTS>
    const MultiQuadTuple<NUM_INPUTS, NUM_OUTEROUTPUTS> rMult(const BitMatrix<NUM_OUTPUTS> & C) {
        return MultiQuadTuple<NUM_INPUTS, NUM_OUTEROUTPUTS>(_contributionsT * (C.template T<NUM_OUTEROUTPUTS>()), C.template operator*<NUM_OUTEROUTPUTS>(_constants));
    }

/* MultiQuadTuple-MultiQuadTuple Operations */

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

/* Access */

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

private:
    BitMatrix<NUM_OUTPUTS> _contributionsT;
    BitVector<NUM_OUTPUTS> _constants; //constant term
    static const unsigned int numInputBits = (NUM_INPUTS << 6);
    static const unsigned int numInputMonomials = ((numInputBits * (numInputBits + 1)) >> 1);
};
#endif