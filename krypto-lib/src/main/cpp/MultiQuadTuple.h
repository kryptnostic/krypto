//
//  MultiQuadTuple.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 7/29/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//

#ifndef krypto_MultiQuadTuple_h
#define krypto_MultiQuadTuple_h
#include "BitVector.h"
#include "BitMatrix.h"

#define NUM_INPUT_MONOMIALS ((NUM_INPUTS * (NUM_INPUTS + 1)) >> 1)

template<unsigned int NUM_INPUTS,unsigned int NUM_OUTPUTS, unsigned int LIMIT=NUM_INPUTS>
struct MultiQuadTuple {
    MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, LIMIT - 1U> next;
    BitMatrix<LIMIT, NUM_OUTPUTS> _matrix;

/* Generation of MultiQuadTuple */

    //Generates a random MultiQuadTuple
    void randomize() {
        _matrix = BitMatrix<LIMIT,NUM_OUTPUTS>::randomMatrix();
        next.randomize();
    }

    //Generates a zero MultiQuadTuple
    void zero() {
        _matrix = BitMatrix<LIMIT,NUM_OUTPUTS>::zeroMatrix();
        next.zero();
    }

    //Fill the MultiQuadTuple with the given components
    void setContributions(const BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS> &m, const BitVector<NUM_OUTPUTS> &v){
        zero();
        setSubContributions(0, m, v);
    }

    //Set the MultiQuadTuple to be one with only matrix(linear) component
    void setAsMatrix(const BitMatrix<NUM_OUTPUTS, NUM_INPUTS> &m){
        _matrix.zero();
        _matrix[0] = m.transpose().getRow(NUM_INPUTS - LIMIT);
        next.setAsMatrix(m);
    }

    //Set the MultiQuadTuple to be one with only constants component
    void setAsConstants(const BitVector<NUM_OUTPUTS> &v){
        _matrix = BitMatrix<LIMIT,NUM_OUTPUTS>::zeroMatrix();
        next.setAsConstants(v);
    }

    //Recursively calls next on super when limits are not equal
    template<unsigned int SUPER_INPUTS, unsigned int SUPER_LIMIT>
    void setToSubMQT( const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS, SUPER_LIMIT> & super ) {
        setToSubMQT<SUPER_INPUTS>( super.next );
    }

    //Recursively sets current MQT to super when at the same limit
    template<unsigned int SUPER_INPUTS>
    void setToSubMQT( const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS, LIMIT> & super ) {
        _matrix.copy( super._matrix );
        next.template setToSubMQT<SUPER_INPUTS>( super.next );
    }

/* Getters */

    //Returns the constants component of the MultiQuadTuple
    BitVector<NUM_OUTPUTS> getConstants() const{
        return next.getConstants();
    }

    //Returns the (NUM_INPUTS - STOP_ROWS)th coefficient matrix of the MultiQuadTuple
    template<unsigned int STOP_ROWS>
    BitMatrix<STOP_ROWS, NUM_OUTPUTS> getMatrixN( const BitVector<STOP_ROWS> & dummy) const{
        return next.getMatrixN( dummy );
    }

    //Returns the nth coefficient matrix of the MultiQuadTuple when LIMIT = STOP_ROWS
    BitMatrix<LIMIT, NUM_OUTPUTS> getMatrixN( const BitVector<LIMIT> & dummy) const {
        return _matrix;
    }

/* Evaluation */

    BitVector<NUM_OUTPUTS> operator() ( const BitVector<NUM_INPUTS> & input ) const {
        BitVector<NUM_OUTPUTS> result = BitVector<NUM_OUTPUTS>::zeroVector();
        if(input[NUM_INPUTS-LIMIT]){
            for( unsigned int i = 0; i < LIMIT; ++i ) {
                if( input[ i + (NUM_INPUTS-LIMIT) ]) {
                    result ^= _matrix[ i ];
                }
            }
        }
        result ^= next( input );
        return result;
    }

    // For current MQT f(x), returns MQT that results from evaluating f on subvector x' of x
    // Assumes PARTIAL_INPUTS > 0 and PARTIAL_INPUTS <= NUM_INPUTS
    template<unsigned int PARTIAL_INPUTS>
    MultiQuadTuple<NUM_INPUTS - PARTIAL_INPUTS, NUM_OUTPUTS> partialEval(const BitVector<PARTIAL_INPUTS> & input) const {
    	MultiQuadTuple<NUM_INPUTS - PARTIAL_INPUTS, NUM_OUTPUTS> result;
    	result.template setToSubMQT<NUM_INPUTS, NUM_INPUTS>(*this); //sets result to have the last PARTIAL_INPUTS coefficient matrices of current MQT

        //result.template partialEvalUpdateCoefficients<NUM_INPUTS, PARTIAL_INPUTS, PARTIAL_INPUTS - 1, NUM_INPUTS - 1>(*this, input, BitMatrix<PARTIAL_INPUTS - 1, NUM_INPUTS - 1>());
        result.template updateCoefficientsAboveP<NUM_INPUTS, PARTIAL_INPUTS, PARTIAL_INPUTS - 1, NUM_INPUTS - 1>(*this, input, BitMatrix<PARTIAL_INPUTS - 1, NUM_INPUTS - 1>());

        //result.template updateConstants(*this, input, BitVector<PARTIAL_INPUTS - 1>());
        return result;
    }

    // //In partial evaluation of super, updates the coefficients of result
    // //Iterates through the coefficient matrices of the result
    // //and XORs appropriate rows from first PARTIAL_INPUTS coefficient matrices of super
    // template<unsigned int SUPER_INPUTS, unsigned int PARTIAL_INPUTS, unsigned int INDEX_J>
    // void updateCoefficients(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input, const BitVector<INDEX_J> & dummy) {
    //      //change getMatrixN to return by reference and by const reference

    //     if (input[INDEX_J]) { //if x_j = 1
    //         //iterate through x_i's and update x_j's coeff matrix
    //         updateSingleCoefficient(super, input, BitVector<PARTIAL_INPUTS - 1>(), INDEX_J);
    //     }
    //     updateCoefficients(super, input, BitVector<INDEX_J - 1>()); //go to x_{j - 1}
    // }

    // //In partial evaluation of super, updates the coefficients of result
    // //at j = PARTIAL_INPUTS + 1 in particular and then unrolls loop of updating coefficients
    // //Triggered when INDEX_J reaches PARTIAL_INPUTS + 1
    // template<unsigned int SUPER_INPUTS, unsigned int PARTIAL_INPUTS>
    // void updateCoefficients(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input, const BitVector<PARTIAL_INPUTS + 1> & dummy) {
    //     if (input[PARTIAL_INPUTS + 1]) { //if x_j = 1
    //         //iterate through x_i's and update x_j's coeff matrix
    //         updateSingleCoefficient(super, input, BitVector<PARTIAL_INPUTS - 1>(), PARTIAL_INPUTS + 1);
    //     }
    // }

    // //In partial evaluation of super, updates a single coefficient matrix of result at x_j
    // //Iterates through the x_i's and XORs in the appropriate row
    // template<unsigned int SUPER_INPUTS, unsigned int PARTIAL_INPUTS, unsigned int INDEX_I>
    // void updateSingleCoefficient(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input, const BitVector<INDEX_I> & dummy, const unsigned int index_j) {
    //     if (input[INDEX_I) { //if x_i = 1
    //         //XOR first row of coeff matrix by appropriate row from super
    //         BitVector<NUM_OUTPUTS> superCoeffRowIJ = super.getMatrixN(BitVector<SUPER_INPUTS - INDEX_I>())[index_j];
    //         xorInMonomialContribution(index_j - PARTIAL_INPUTS, index_j - PARTIAL_INPUTS, superCoeffRowIJ);
    //     }

    //     updateSingleCoefficient(super, input, BitVector<INDEX_I - 1>(), index_j); //go to x_{i - 1}
    // }

    // //In partial evaluation of super, updates a single coefficient matrix of result at x_j
    // //at i = 0 in particular and then unrolls loop of updating x_j
    // //Triggered when INDEX_I reaches 0
    // template<unsigned int SUPER_INPUTS, unsigned int PARTIAL_INPUTS>
    // void updateSingleCoefficient(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input, const BitVector<0> & dummy, const unsigned int index_j) {
    //     if (input[INDEX_I) { //if x_i = 1
    //         //XOR first row of coeff matrix by appropriate row from super
    //         BitVector<NUM_OUTPUTS> superCoeffRowIJ = super.getMatrixN(BitVector<SUPER_INPUTS - INDEX_I>())[index_j];
    //         xorInMonomialContribution(index_j - PARTIAL_INPUTS, index_j - PARTIAL_INPUTS, superCoeffRowIJ);
    //     }
    // }

    // //In partial evaluation of super, updates constants of result
    // //Iterates through x_i's and XORs in the first PARTIAL_INPUTS rows into constants
    // template<unsigned int SUPER_INPUTS, unsigned int PARTIAL_INPUTS, unsigned int INDEX_I>
    // void updateConstants(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input, const BitVector<INDEX_I> & dummy) {
    //     if (input[INDEX_I]) { //if x_i = 1
    //         //iterate through x_j's and update constants
    //         updateSingleConstant(super, input, BitVector<PARTIAL_INPUTS - 1>(), INDEX_I);
    //     }
    //     updateConstants(super, input, BitVector<INDEX_I - 1>()); //go to x_{j - 1}
    // }

    // //In partial evaluation of super, updates constants of result
    // //at i = 0 in particular and then unrolls loop of updating constants
    // //Triggered when INDEX_I reaches 0
    // template<unsigned int SUPER_INPUTS, unsigned int PARTIAL_INPUTS>
    // void updateConstants(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input, const BitVector<0> & dummy) {
    //     if (input[0]) { //if x_i = 1
    //         //iterate through x_j's and update constants
    //         updateSingleConstant(super, input, BitVector<PARTIAL_INPUTS - 1>(), 0);
    //     }
    // }

    // //In partial evaluation of super, updates the constants of result from a single coefficient matrix of super at x_i
    // //Iterates through the x_j's and XORs in the appropriate row
    // template<unsigned int SUPER_INPUTS, unsigned int PARTIAL_INPUTS, unsigned int INDEX_J>
    // void updateSingleConstant(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input, const BitVector<INDEX_J> & dummy, const unsigned int index_i) {
    //     if (input[INDEX_J) { //if x_i = 1
    //         //XOR constants by appropriate row from super
    //         BitVector<NUM_OUTPUTS> superCoeffRowIJ = super.getMatrixN(BitVector<SUPER_INPUTS - INDEX_I>())[index_j];
    //         xorConstants(superCoeffRowIJ);
    //     }

    //     updateSingleConstant(super, input, BitVector<INDEX_J - 1>(), index_i); //go to x_{j - 1}
    // }

    // //In partial evaluation of super, updates the constants of result from a single coefficient matrix of super at x_i
    // //at j = 0 in particular and then unrolls loop of updating x_i
    // //Triggered when INDEX_J reaches 0
    // template<unsigned int SUPER_INPUTS, unsigned int PARTIAL_INPUTS>
    // void updateSingleConstant(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input, const BitVector<0> & dummy, const unsigned int index_i) {
    //     if (input[0]) { //if x_i = 1
    //         //XOR constants by appropriate row from super
    //         BitVector<NUM_OUTPUTS> superCoeffRowIJ = super.getMatrixN(BitVector<SUPER_INPUTS - INDEX_I>())[index_j];
    //         xorConstants(superCoeffRowIJ);
    //     }
    // }

    // Used primarily for partialEval
    // Unrolls double for-loop over each coefficient and updates coefficient matrices
    // When 0 < i < PARTIAL_INPUTS < j
    template<unsigned int SUPER_INPUTS, unsigned PARTIAL_INPUTS, unsigned int INDEX_I, unsigned int INDEX_J>
    void updateCoefficientsAboveP(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input, const BitMatrix<INDEX_I, INDEX_J> & dummy) {
        BitMatrix<SUPER_INPUTS - INDEX_I, NUM_OUTPUTS> coeffMatrix = super.getMatrixN(BitVector<SUPER_INPUTS - INDEX_I>());

        bool first = input[INDEX_I]; //x_i
        if (first) {
            //add row of x_i x_j's to first row of coeff matrix of x_j
            xorMatrixRowN(0, coeffMatrix[INDEX_J - INDEX_I], BitVector< SUPER_INPUTS - INDEX_J >());
        }

        updateCoefficientsAboveP(super, input, BitMatrix<INDEX_I, INDEX_J - 1>()); //decrement j
    }

    // Used primarily for partialEval
    // Unrolls double for-loop over each coefficient and updates coefficient matrices
    // When 0 < i < PARTIAL_INPUTS = j
    template<unsigned int SUPER_INPUTS, unsigned PARTIAL_INPUTS, unsigned int INDEX_I>
    void updateCoefficientsAboveP(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input, const BitMatrix<INDEX_I, PARTIAL_INPUTS> & dummy) {
        BitMatrix<SUPER_INPUTS - INDEX_I, NUM_OUTPUTS> coeffMatrix = super.getMatrixN(BitVector<SUPER_INPUTS - INDEX_I>());

        bool first = input[INDEX_I]; //x_i
        if (first) {
            //add row of x_i x_j's to first row of coeff matrix of x_j
            xorMatrixRowN(0, coeffMatrix[PARTIAL_INPUTS - INDEX_I], BitVector< SUPER_INPUTS - PARTIAL_INPUTS >());
        }

        updateCoefficientsBelowP(super, input, BitMatrix<INDEX_I, PARTIAL_INPUTS - 1>()); //decrement j below PARTIAL_INPUTS
    }

    // Used primarily for partialEval
    // Unrolls double for-loop over each coefficient and updates coefficient matrices
    // When 0 < i < j < PARTIAL_INPUTS
    template<unsigned int SUPER_INPUTS, unsigned PARTIAL_INPUTS, unsigned int INDEX_I, unsigned int INDEX_J>
    void updateCoefficientsBelowP(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input, const BitMatrix<INDEX_I, INDEX_J> & dummy) {
        BitMatrix<SUPER_INPUTS - INDEX_I, NUM_OUTPUTS> coeffMatrix = super.getMatrixN(BitVector<SUPER_INPUTS - INDEX_I>());

        bool first = input[INDEX_I]; //x_i
        if (first) {
            bool second = input[INDEX_J];
            if (second) {
                //add row of x_i x_j's to constant vector for i, j < PARTIAL_INPUTS
                xorConstants(coeffMatrix[INDEX_J - INDEX_I]);
            }
        }

        updateCoefficientsBelowP(super, input, BitMatrix<INDEX_I, INDEX_J - 1>()); //decrement j
    }

    // Used primarily for partialEval
    // Unrolls double for-loop over each coefficient and updates coefficient matrices
    // When 0 < i = j
    template<unsigned int SUPER_INPUTS, unsigned PARTIAL_INPUTS, unsigned int INDEX_I>
    void updateCoefficientsBelowP(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input, const BitMatrix<INDEX_I, INDEX_I> & dummy) {
        BitMatrix<SUPER_INPUTS - INDEX_I, NUM_OUTPUTS> coeffMatrix = super.getMatrixN(BitVector<SUPER_INPUTS - INDEX_I>());
        if (input[INDEX_I]) { //x_i
            //add row of x_i's to constant vector for i, j < PARTIAL_INPUTS
            xorConstants(coeffMatrix[0]);
        }

        updateCoefficientsAboveP(super, input, BitMatrix<INDEX_I - 1, SUPER_INPUTS - 1>()); //decrement i, reset j to max
    }

    // Used primarily for partialEval
    // Unrolls double for-loop over each coefficient and updates coefficient matrices
    // When 0 = i = j
    template<unsigned int SUPER_INPUTS, unsigned PARTIAL_INPUTS>
    void updateCoefficientsBelowP(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input, const BitMatrix<0, 0> & dummy) {
        BitMatrix<SUPER_INPUTS, NUM_OUTPUTS> coeffMatrix = super.getMatrixN(BitVector<SUPER_INPUTS>());
        if (input[0]) { //x_i
            //add row of x_0's to constant vector for i, j < PARTIAL_INPUTS
            xorConstants(coeffMatrix[0]);
        }
    }



/* Composition */

    //Left composition on a matrix
    template<unsigned int NUM_INNER_INPUTS>
    MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> operator*(  const BitMatrix<NUM_INPUTS, NUM_INNER_INPUTS> & inner ) const{
        MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> cs;
        BitVector<NUM_OUTPUTS> v = getConstants();
        cs.setAsConstants(v); //reset contribution matrix and force constants
        compose( cs, inner.transpose() );
        return cs;
    }

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

    //Set the constant components of the MultiQuadTuple to given BitVector
    void setConstants(const BitVector<NUM_OUTPUTS> &v) {
        next.setConstants(v);
    }

    //Xor the constant components of the MultiQuadTuple with given BitVector
    void xorConstants(const BitVector<NUM_OUTPUTS> &v){
        next.xorConstants(v);
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

    //Returns the nth coefficient matrix of the MultiQuadTuple when LIMIT = STOP_ROWS
    //Xors a given row of the matrix with a given input vector
    template<unsigned int STOP_ROWS>
    BitMatrix<STOP_ROWS, NUM_OUTPUTS> xorMatrixRowN( const unsigned int rowIndex, const BitVector<NUM_OUTPUTS> input, const BitVector<STOP_ROWS> & dummy) {
        return next.xorMatrixRowN( rowIndex, input, dummy );
    }

    //Returns the nth coefficient matrix of the MultiQuadTuple when LIMIT = STOP_ROWS
    //Xors a given row of the matrix with a given input vector
    BitMatrix<LIMIT, NUM_OUTPUTS> xorMatrixRowN( const unsigned int rowIndex, const BitVector<NUM_OUTPUTS> input, const BitVector<LIMIT> & dummy) {
        _matrix[rowIndex] ^= input;
        return _matrix;
    }

/* Concatenations */

    //Vertical concatenation of 2 MultiQuadTuples
    template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2>
    void augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1,LIMIT> & lower, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2,LIMIT> & upper ) {
        _matrix = BitMatrix<LIMIT, NUM_OUTPUTS>::augH(lower._matrix, upper._matrix);
        next.augV( lower.next, upper.next );
    }

    //Vertical concatenation of 2 MultiQuadTuples
    template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2, unsigned int NUM_OUTPUTS3>
    void augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1,LIMIT> & lower, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2,LIMIT> & mid, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS3,LIMIT> & upper ) {
        _matrix = BitMatrix<LIMIT, NUM_OUTPUTS>::augH(lower._matrix, mid._matrix, upper._matrix);
        next.augV( lower.next, mid.next, upper.next );
    }

/* Inter-MultiQuadTuple Arithmetic Operations */

    //BitWise XOR
    MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> operator^(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> & rhs) const {
        MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> result;
        result._matrix = _matrix ^ rhs._matrix;
        result.next = next ^ rhs.next;
        return result;
    }

/* Print */

    //Print the constant component of the MultiQuadTuple
    void printConstants() const{
        next.printConstants();
    }

/* Helper Funtions (Not called by other classes) */

    //Worker body for setContributions (not called by other classes)
    void setSubContributions(unsigned int start, const BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS> &m, const BitVector<NUM_OUTPUTS> &v){
        _matrix.template setMatrix<NUM_INPUT_MONOMIALS>(m, start);
        next.setSubContributions(start + LIMIT, m, v);
    }

    //Worker body for left compose function (not called by other classes)
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

    //XORs a given vector into the row corresponding to coefficients of x_matrixIndex x_rowIndex
    void xorInMonomialContribution(unsigned int matrixIndex, unsigned int rowIndex, const BitVector<NUM_OUTPUTS> &v){
        if(matrixIndex == (NUM_INPUTS - LIMIT)){
            _matrix[rowIndex - matrixIndex] ^= v;
        } else {
            next.xorInMonomialContribution(matrixIndex, rowIndex, v);
        }
    }

    //Worker body for rMult that cascades down the multiplication.
    template<unsigned int OLD_NUM_OUTPUTS>
    void multiplyAndSet( const MultiQuadTuple<NUM_INPUTS,OLD_NUM_OUTPUTS,LIMIT> & src, const BitMatrix<NUM_OUTPUTS, OLD_NUM_OUTPUTS> & m, const BitMatrix<OLD_NUM_OUTPUTS, NUM_OUTPUTS> & mT ) {
        _matrix = src._matrix * mT;
        next.multiplyAndSet( src.next , m, mT );
    }

    //Set the constant vector of rhs be the same as that of the current object.
    template<unsigned int NUM_INNER_INPUTS>
    void copyConstants(MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS,LIMIT> & rhs) const{
        next.template copyConstants<NUM_INNER_INPUTS>(rhs.next);
    }
};




/* Base case of the recursive struct: the constants - cliets do not need to read this part */
template<unsigned NUM_INPUTS,unsigned int NUM_OUTPUTS>
struct MultiQuadTuple<NUM_INPUTS,NUM_OUTPUTS,0> {
    BitVector<NUM_OUTPUTS> _constants;

/* Base case of MQT Generation */

    void randomize() {
        _constants = BitVector<NUM_OUTPUTS>::randomVector();
    }

    void zero() {
        _constants = BitVector<NUM_OUTPUTS>::zeroVector();
    }

    void setAsMatrix(const BitMatrix<NUM_OUTPUTS, NUM_INPUTS> &m){
        _constants = BitVector<NUM_OUTPUTS>::zeroVector();
    }

    void setAsConstants(const BitVector<NUM_OUTPUTS> &v){
        _constants = v;
    }

/* Evaluation */

    BitVector<NUM_OUTPUTS> operator() ( const BitVector<NUM_INPUTS> & input ) const {
        return _constants;
    }

/* State modifiers */

    void setConstants(const BitVector<NUM_OUTPUTS> &v) {
        _constants = v;
    }

    void xorConstants(const BitVector<NUM_OUTPUTS> &v) {
        _constants ^= v;
    }

    template<unsigned int MONOMIAL_INDEX>
    void setMatrix( const BitMatrix<MONOMIAL_INDEX, NUM_OUTPUTS> & m  ) {
        //NO-OP base case should be elided under optimization.
    }

    //Recursively sets current MQT to super when at the limit 0
    template<unsigned int SUPER_INPUTS>
    void setToSubMQT( const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS, 0> & super ) {
        setConstants(super._constants);
    }

/* Print */

    void printConstants() const{
        _constants.print();
    }

/* Base case of composition */

    template<unsigned int NUM_INNER_INPUTS>
    MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> operator()(const BitMatrix<NUM_INPUTS, NUM_INNER_INPUTS> & inner ) const {
        MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> result;
        result.zero();
        copyConstants(result);
        return result;
    }

/* Getters */
    BitVector<NUM_OUTPUTS> getConstants() const{
        return _constants;
    }

/* Concatenations */

    template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2>
    void augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1, 0> & lower, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2, 0> & upper ) {
        _constants = BitVector<NUM_OUTPUTS>::vCat( lower._constants , upper._constants );
    }

    template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2, unsigned int NUM_OUTPUTS3>
    void augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1, 0> & lower, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2, 0> & mid, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS3, 0> & upper ) {
        _constants = BitVector<NUM_OUTPUTS>::vCat( lower._constants , mid._constants, upper._constants );
    }

/* Inter-MultiQuadTuple Arithmetic Operations */

    //BitWise XOR
    MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, 0> operator^(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, 0> & rhs) const {
        MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, 0> result;
        result._constants = _constants ^ rhs._constants;
        return result;
    }

/* Helper Functions */

    //Set the constant vector of rhs be the same as that of the current object.
    template<unsigned int NUM_INNER_INPUTS>
    void copyConstants(MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS, 0> & rhs) const{
        rhs._constants = _constants;
    }

    void setSubContributions(unsigned int start, const BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS> &m, const BitVector<NUM_OUTPUTS> &v){
        _constants = v;
    }

    template<unsigned int NUM_INNER_INPUTS>
    void compose( MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> & root, const BitMatrix<NUM_INNER_INPUTS,NUM_INPUTS> & innerTranspose ) const {
        //no-op
    }

    void xorInMonomialContribution(unsigned int matrixIndex, unsigned int rowIndex, const BitVector<NUM_OUTPUTS> &v){
        //no-op
    }

    template<unsigned int OLD_NUM_OUTPUTS>
    void multiplyAndSet( const MultiQuadTuple<NUM_INPUTS,OLD_NUM_OUTPUTS,0> & src, const BitMatrix<NUM_OUTPUTS,OLD_NUM_OUTPUTS> & m ,const BitMatrix<OLD_NUM_OUTPUTS, NUM_OUTPUTS> & mT ) {
        _constants = m * src._constants;
    }
};

#endif