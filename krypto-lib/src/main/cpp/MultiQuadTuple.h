//
//  MultiQuadTuple.h
//  krypto
//
//  Created by Matthew Tamayo-Rios on 8/14/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//

#ifndef krypto_MultiQuadTuple_h
#define krypto_MultiQuadTuple_h
#include "BitVector.h"
#include "BitMatrix.h"

#define NUM_INPUT_MONOMIALS ((NUM_INPUTS * (NUM_INPUTS+1)) >> 1)

template<unsigned int NUM_INPUTS,unsigned int NUM_OUTPUTS, unsigned int LIMIT=NUM_INPUTS>
struct MultiQuadTuple {
    MultiQuadTuple<NUM_INPUTS,NUM_OUTPUTS, LIMIT-1U> next;
    BitMatrix<LIMIT,NUM_OUTPUTS> _matrix;

    BitVector<NUM_OUTPUTS> operator() ( const BitVector<NUM_INPUTS> & input ) const {
        BitVector<NUM_OUTPUTS> result = BitVector<NUM_OUTPUTS>::zeroVector();
        if(input[NUM_INPUTS-LIMIT]){
            for( unsigned int i = 0; i < LIMIT; ++i ) {
                if( input[ i + (NUM_INPUTS-LIMIT) ]) {
                    result^=_matrix[ i ];
                }
            }
        }
        result ^= next( input );
        return result;
    }

    void randomize() {
        _matrix = BitMatrix<LIMIT,NUM_OUTPUTS>::randomMatrix();
        next.randomize();
    }

    void zero() {
        _matrix = BitMatrix<LIMIT,NUM_OUTPUTS>::zeroMatrix();
        next.zero();
    }

    void setContributions(const BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS> &m, const BitVector<NUM_OUTPUTS> &v){
        zero();
        setSubContributions(0, m, v);
    }

    void setSubContributions(unsigned int start, const BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS> &m, const BitVector<NUM_OUTPUTS> &v){
        _matrix.template setMatrix<NUM_INPUT_MONOMIALS>(m, start);
        next.setSubContributions(start + LIMIT, m, v);
    }

    void setAsConstants(const BitVector<NUM_OUTPUTS> &v){
        _matrix = BitMatrix<LIMIT,NUM_OUTPUTS>::zeroMatrix();
        next.setAsConstants(v);
    }

    const BitVector<NUM_OUTPUTS> getConstants() const{
        return next.getConstants();
    }

    template<unsigned int NUM_INNER_INPUTS>
    MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> operator*(  const BitMatrix<NUM_INPUTS, NUM_INNER_INPUTS> & inner ) const{
        MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> cs;
        BitVector<NUM_OUTPUTS> v = getConstants();
        cs.setAsConstants(v); //reset contribution matrix and force constants
        compose( cs, inner.transpose() );
        return cs;
    }

    void setConstants(const BitVector<NUM_OUTPUTS> &v) const{
        next.setConstants(v);
    }

    void printConstants() const{
        next.printConstants();
    }

    template<unsigned int NUM_INNER_INPUTS>
    void compose( MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> & root, const BitMatrix<NUM_INNER_INPUTS,NUM_INPUTS> &innerTranspose ) const {
        BitMatrix<NUM_INNER_INPUTS, NUM_OUTPUTS> composedCoefficientMatrix = innerTranspose .template pMult<LIMIT,NUM_OUTPUTS>(_matrix, NUM_INPUTS - LIMIT, NUM_INPUTS - 1, 0, LIMIT - 1);
        for( unsigned int i = 0; i < NUM_INNER_INPUTS ; ++i ) {
            if( innerTranspose[ i ][ NUM_INPUTS - LIMIT ] ) {
                for(unsigned int j = 0; j < i; ++j){
                    root.xorInMonomialContribution(j, i, composedCoefficientMatrix[j]);
                }
                for(unsigned int j = i; j < NUM_INNER_INPUTS; ++j){
                    root.xorInMonomialContribution(i, j, composedCoefficientMatrix[j]);
                }
            }
        }
        next.compose(root, innerTranspose);
    }

    void xorInMonomialContribution(unsigned int matrixIndex, unsigned int rowIndex, const BitVector<NUM_OUTPUTS> &v){
        if(matrixIndex == (NUM_INPUTS - LIMIT)){
            _matrix[rowIndex - matrixIndex] ^= v;
        } else {
            next.xorInMonomialContribution(matrixIndex, rowIndex, v);
        }
    }

    void setAsMatrix(const BitMatrix<NUM_OUTPUTS, NUM_INPUTS> &m){
        _matrix.zero();
        _matrix[0] = m.transpose().getRow(NUM_INPUTS - LIMIT);
        next.setAsMatrix(m);
    }

/* Functional behavior */

    /*
     * Multiplies every _matrix on the right by rhs, which should be the tranpose of the linear trasnformation
     * being applied as compose. We assume that callers have not called transpose on RHS.
     */
    template<unsigned int NEW_NUM_OUTPUTS>
    MultiQuadTuple<NUM_INPUTS,NEW_NUM_OUTPUTS> rMult(const BitMatrix<NEW_NUM_OUTPUTS, NUM_OUTPUTS> &  rhs ) const {
        MultiQuadTuple<NUM_INPUTS,NEW_NUM_OUTPUTS> cs;
        cs.multiplyAndSet(*this, rhs, rhs.transpose() );
        return cs;
    }

/* State modifiers */

    /*
     * Worker body for operator* that cascades down the multiplication.
     */
    template<unsigned int OLD_NUM_OUTPUTS>
    void multiplyAndSet( const MultiQuadTuple<NUM_INPUTS,OLD_NUM_OUTPUTS,LIMIT> & src, const BitMatrix<NUM_OUTPUTS, OLD_NUM_OUTPUTS> & m, const BitMatrix<OLD_NUM_OUTPUTS, NUM_OUTPUTS> & mT ) {
        _matrix = src._matrix * mT;
        next.multiplyAndSet( src.next , m, mT );
    }

    /*
     * In theory since the compiler will find setMatrix with BitMatrix<LIMIT, NUM_OUTPUTS> as a separate class with an existing body
     * the template generation won't get triggered.
     * TODO: Add test to make sure this behaves as expected.
     */
    void setMatrix( const BitMatrix<LIMIT, NUM_OUTPUTS> & m ) {
        _matrix =  m;
    }

    template<unsigned int MONOMIAL_INDEX>
    void setMatrix( const BitMatrix<MONOMIAL_INDEX, NUM_OUTPUTS> & m ) {
        next.setMatrix( m );
    }

    template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2>
    void augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1,LIMIT> & lower, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2,LIMIT> & upper ) {
        _matrix = BitMatrix<LIMIT, NUM_OUTPUTS>::augH(lower._matrix, upper._matrix);
        next.augV( lower.next, upper.next );
    }

    template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2, unsigned int NUM_OUTPUTS3>
    void augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1,LIMIT> & lower, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2,LIMIT> & mid, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS3,LIMIT> & upper ) {
        _matrix = BitMatrix<LIMIT, NUM_OUTPUTS>::augH(lower._matrix, mid._matrix, upper._matrix);
        next.augV( lower.next, mid.next, upper.next );
    }

    //Set the constant vector of rhs be the same as that of the current object.
    template<unsigned int NUM_INNER_INPUTS>
    void copyConstants(MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS,LIMIT> & rhs) const{
        next.template copyConstants<NUM_INNER_INPUTS>(rhs.next);
    }

    //BitWise XOR
    MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> operator^(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> & rhs) const {
        MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> result;
        result._matrix = _matrix ^ rhs._matrix;
        result.next = next ^ rhs.next;
        return result;
    }
};

template<unsigned NUM_INPUTS,unsigned int NUM_OUTPUTS>
struct MultiQuadTuple<NUM_INPUTS,NUM_OUTPUTS,0> {
    BitVector<NUM_OUTPUTS> _constants;

    BitVector<NUM_OUTPUTS> operator() ( const BitVector<NUM_INPUTS> & input ) const {
        return _constants;
    }

    void setConstants(const BitVector<NUM_OUTPUTS> &v) const{
        _constants = v; //TODO
    }

    void printConstants() const{
        _constants.print();
    }

    template<unsigned int NUM_INNER_INPUTS>
    void compose( MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> & root, const BitMatrix<NUM_INNER_INPUTS,NUM_INPUTS> & innerTranspose ) const {
        //no-op
    }

    void xorInMonomialContribution(unsigned int matrixIndex, unsigned int rowIndex, const BitVector<NUM_OUTPUTS> &v){
        //no-op
    }

    template<unsigned int NUM_INNER_INPUTS>
    MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> operator()(const BitMatrix<NUM_INPUTS, NUM_INNER_INPUTS> & inner ) const {
        MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> result;
        result.zero();
        copyConstants(result);
        return result;
    }

    template<unsigned int MONOMIAL_INDEX>
    void setMatrix( const BitMatrix<MONOMIAL_INDEX, NUM_OUTPUTS> & m  ) {
        //NO-OP base case should be elided under optimization.
    }

    //Set the constant vector of rhs be the same as that of the current object.
    template<unsigned int NUM_INNER_INPUTS>
    void copyConstants(MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS, 0> & rhs) const{
        rhs._constants = _constants;
    }

    void randomize() {
        _constants = BitVector<NUM_OUTPUTS>::randomVector();
    }

    void zero() {
        _constants = BitVector<NUM_OUTPUTS>::zeroVector();
    }

    void setSubContributions(unsigned int start, const BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS> &m, const BitVector<NUM_OUTPUTS> &v){
        _constants = v;
    }

    void setAsConstants(const BitVector<NUM_OUTPUTS> &v){
        _constants = v;
    }

    const BitVector<NUM_OUTPUTS> getConstants() const{
        return _constants;
    }

    template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2>
    void augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1, 0> & lower, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2, 0> & upper ) {
        _constants = BitVector<NUM_OUTPUTS>::vCat( lower._constants , upper._constants );
    }

    template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2, unsigned int NUM_OUTPUTS3>
    void augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1, 0> & lower, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2, 0> & mid, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS3, 0> & upper ) {
        _constants = BitVector<NUM_OUTPUTS>::vCat( lower._constants , mid._constants, upper._constants );
    }

    template<unsigned int OLD_NUM_OUTPUTS>
    void multiplyAndSet( const MultiQuadTuple<NUM_INPUTS,OLD_NUM_OUTPUTS,0> & src, const BitMatrix<NUM_OUTPUTS,OLD_NUM_OUTPUTS> & m ,const BitMatrix<OLD_NUM_OUTPUTS, NUM_OUTPUTS> & mT ) {
        _constants = m * src._constants;
    }

    void setAsMatrix(const BitMatrix<NUM_OUTPUTS, NUM_INPUTS> &m){
        _constants = BitVector<NUM_OUTPUTS>::zeroVector();
    }

    //BitWise XOR
    MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, 0> operator^(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, 0> & rhs) const {
        MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, 0> result;
        result._constants = _constants ^ rhs._constants;
        return result;
    }
};

#endif