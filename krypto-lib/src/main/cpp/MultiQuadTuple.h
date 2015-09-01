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

/*
 * Template for MultiQuadTuple
 * Contains recursive struct definitions to next
 * Each level (or limit) contains the coefficient matrix for one variable
 * Contains constant vector at limit = 0
 */
template<unsigned int NUM_INPUTS,unsigned int NUM_OUTPUTS, unsigned int LIMIT=NUM_INPUTS>
struct MultiQuadTuple {
    MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, LIMIT - 1U> next;
    BitMatrix<LIMIT, NUM_OUTPUTS> _matrix;

/* Generation of MultiQuadTuple */

    /*
     * Function: randomize()
     * Generates a random MultiQuadTuple
     * Ends with a template specialization at limit = 0
     */
    void randomize() {
        _matrix = BitMatrix<LIMIT,NUM_OUTPUTS>::randomMatrix();
        next.randomize();
    }

    /*
     * Function: zero()
     * Generates a zero MultiQuadTuple
     * Ends with a template specialization at limit = 0
     */
    void zero() {
        _matrix.zero();
        next.zero();
    }

    /*
     * Function: setContributions(m, v)
     * Generates a MultiQuadTuple with coefficient matrices given by a large
     * concatenated coefficient matrix and a constant vector
     */
    void setContributions(const BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS> &m, const BitVector<NUM_OUTPUTS> &v){
        zero();
        setSubContributions(0, m, v);
    }

    /*
     * Function: setAsMatrix(m)
     * Generates a MultiQuadTuple equivalent to a given linear transformation
     */
    void setAsMatrix(const BitMatrix<NUM_OUTPUTS, NUM_INPUTS> &m){
        _matrix.zero();
        _matrix[0] = m.getCol(NUM_INPUTS - LIMIT);
        //cout << "NUM_INPUTS: " << NUM_INPUTS << endl;
        //cout << "LIMIT: " << LIMIT << endl;
        next.setAsMatrix(m);
    }

    void setAsMatrixTranspose(const BitMatrix<NUM_INPUTS, NUM_OUTPUTS> & m) {
        _matrix.zero();
        _matrix[0] = m.getRow(NUM_INPUTS - LIMIT);
        next.setAsMatrixTranspose(m);
    }

    /*
     * Function: setAsConstants(v)
     * Generates a MultiQuadTuple with coefficient matrices to a given constant vector
     */
    void setAsConstants(const BitVector<NUM_OUTPUTS> &v){
        _matrix.zero();
        next.setAsConstants(v);
    }

    /*
     * Function: setToSubMQT(super)
     * Generates a MultiQuadTuple with the last NUM_INPUTS coefficient matrices
     * of a given larger MultiQuadTuple in 3 functions
     * Ends with a template specialization at limit = 0
     * Unrolls template until the NUM_INPUT-th from last coefficient
     */
    template<unsigned int SUPER_INPUTS, unsigned int SUPER_LIMIT>
    void setToSubMQT(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS, SUPER_LIMIT> & super) {
        setToSubMQT<SUPER_INPUTS>(super.next);
    }

    /*
     * Function: setToSubMQT(super)
     * Generates a MultiQuadTuple with the last NUM_INPUTS coefficient matrices
     * of a given larger MultiQuadTuple
     * Ends with a template specialization at limit = 0
     * Copies the last NUM_INPUT coefficient matrices
     */
    template<unsigned int SUPER_INPUTS>
    void setToSubMQT(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS, LIMIT> & super) {
        _matrix.copy(super._matrix);
        next.template setToSubMQT<SUPER_INPUTS>(super.next);
    }

/* Getters */

    /*
     * Function: getConstants()
     * Returns the constants vector
     */
    const BitVector<NUM_OUTPUTS> & getConstants() const{
        return next.getConstants();
    }

    /*
     * Function: getMatrix(dummy)
     * Returns the (NUM_INPUTS - STOP_ROWS)th coefficient matrix
     * Recursive call until LIMIT = STOP_ROWS
     */
    template<unsigned int STOP_ROWS>
    const BitMatrix<STOP_ROWS, NUM_OUTPUTS> & getMatrix(const BitVector<STOP_ROWS> & dummy) const{
        return next.getMatrix(dummy);
    }

    /*
     * Function: getMatrix(dummy)
     * Returns the (NUM_INPUTS - STOP_ROWS)th coefficient matrix
     * Stops when LIMIT = STOP_ROWS
     */
    const BitMatrix<LIMIT, NUM_OUTPUTS> & getMatrix(const BitVector<LIMIT> & dummy) const {
        return _matrix;
    }

    /*
     * Function: getMQT(dummy)
     * Returns the subMQT corresponding to the (NUM_INPUTS - STOP_ROWS)th variable
     * Recursive call until LIMIT = STOP_ROWS
     */
    template<unsigned int STOP_ROWS>
    MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, STOP_ROWS> & getMQT(const BitVector<STOP_ROWS> & dummy) const{
        return next.getMQT(dummy);
    }

    /*
     * Function: getMQT(dummy)
     * Returns the subMQT corresponding to the (NUM_INPUTS - STOP_ROWS)th variable
     * Stops when LIMIT = STOP_ROWS
     */
    MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, LIMIT> & getMQT(const BitVector<LIMIT> & dummy) const {
        return *this;
    }

/* Evaluation */

    /*
     * Operator: (input)
     * Evaluates the MultiQuadTuple on an input vector
     */
    const BitVector<NUM_OUTPUTS> operator() (const BitVector<NUM_INPUTS> & input) const {
        BitVector<NUM_OUTPUTS> result = BitVector<NUM_OUTPUTS>::zeroVector();
        if(input[NUM_INPUTS-LIMIT]){
            for(unsigned int i = 0; i < LIMIT; ++i) {
                if(input[ i + (NUM_INPUTS-LIMIT) ]) {
                    result ^= _matrix[ i ];
                }
            }
        }
        result ^= next(input);
        return result;
    }

    /*
     * Function: partialEval(input)
     * Partially evaluates the MultiQuadTuple on an input vector
     * Returns MQT that results from evaluating f on subvector x' of x
     * Assumes PARTIAL_INPUTS > 0 and PARTIAL_INPUTS <= NUM_INPUTS
     */
    template<unsigned int PARTIAL_INPUTS>
    const MultiQuadTuple<NUM_INPUTS - PARTIAL_INPUTS, NUM_OUTPUTS> partialEval(const BitVector<PARTIAL_INPUTS> & input) const {
    	MultiQuadTuple<NUM_INPUTS - PARTIAL_INPUTS, NUM_OUTPUTS> result;
    	result.template setToSubMQT<NUM_INPUTS, NUM_INPUTS>(*this); //sets result to have the last PARTIAL_INPUTS coefficient matrices of current MQT

        //TODO: change below to use next instead of INDEX_I, INDEX_J
        result.updateCoefficients(*this, input);
        result.updateConstants(*this, input, BitVector<NUM_INPUTS - PARTIAL_INPUTS>());

        return result;
    }

    /*
     * Function: updateCoefficients(super, input)
     * In partial evaluation of super, updates the coefficients of result
     * Iterates through the coefficient matrices x_j of the result
     * and XORs appropriate rows from first PARTIAL_INPUTS coefficient matrices of super
     * Base case of NO-OP base case is in class template specialization with LIMIT = 0 (i.e. j = SUPER_INPUTS)
     */
    template<unsigned int SUPER_INPUTS, unsigned int PARTIAL_INPUTS>
    void updateCoefficients(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input) {
        //iterate through x_i's and update x_j's coeff matrix
        updateSingleCoefficient(super, input, BitVector<SUPER_INPUTS - PARTIAL_INPUTS>());

        next.updateCoefficients(super, input); //go to x_{j + 1}
    }

    /*
     * Function: updateSingleCoefficient(super, input, dummy)
     * In partial evaluation of super, updates a single coefficient matrix of result at x_j
     * Iterates through the x_i's and XORs in the appropriate row
     * From i = 0 to i = PARTIAL_INPUTS
     */
    template<unsigned int SUPER_INPUTS, unsigned int PARTIAL_INPUTS, unsigned int SUPER_LIMIT>
    void updateSingleCoefficient(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS, SUPER_LIMIT> & super, const BitVector<PARTIAL_INPUTS> & input, const BitVector<SUPER_LIMIT - PARTIAL_INPUTS> & dummy) {
        const unsigned int index_i = SUPER_INPUTS - SUPER_LIMIT;

        if (input[index_i]) { //if x_i = 1
            //XOR first row of coeff matrix by appropriate row from super
            const unsigned int index_j = SUPER_INPUTS - LIMIT;
            const BitVector<NUM_OUTPUTS> & superCoeffRowIJ = (super._matrix)[index_j - index_i];
            xorInMonomialContribution(index_j - PARTIAL_INPUTS, index_j - PARTIAL_INPUTS, superCoeffRowIJ);
        }
        // alternate instead of xorInMon... = xorMatrixRowN(0, superCoeffRowIJ, BitVector< SUPER_INPUTS - INDEX_J >());

        updateSingleCoefficient(super.next, input, BitVector<SUPER_LIMIT - PARTIAL_INPUTS - 1>()); //go to x_{i - 1}
    }


    /*
     * Function: updateSingleCoefficient(super, input, dummy)
     * In partial evaluation of super, updates a single coefficient matrix of result at x_j
     * Iterates through the x_i's and XORs in the appropriate row
     * Triggered when i = PARTIAL_INPUTS
     */
    template<unsigned int SUPER_INPUTS, unsigned int PARTIAL_INPUTS, unsigned int SUPER_LIMIT>
    void updateSingleCoefficient(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS, SUPER_LIMIT> & super, const BitVector<PARTIAL_INPUTS> & input, const BitVector<0> & dummy) {
        //NO-OP base case
    }

    /*
     * Function: updateConstants(super, input, dummy)
     * In partial evaluation of super, updates the constants of result
     * Iterates through the coefficient matrices x_i of super
     * and XORs in the first PARTIAL_INPUTS rows into constants
     * From i = 0 to i = PARTIAL_INPUTS (i.e. SUPER_LIMIT = SUPER_INPUTS - PARTIAL_INPUTS)
     */
    template<unsigned int SUPER_INPUTS, unsigned int PARTIAL_INPUTS, unsigned int SUPER_LIMIT>
    void updateConstants(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS, SUPER_LIMIT> & super, const BitVector<PARTIAL_INPUTS> & input, const BitVector<SUPER_LIMIT + PARTIAL_INPUTS - SUPER_INPUTS> & dummy) {
        const unsigned int index_i = SUPER_INPUTS - SUPER_LIMIT;

        if (input[index_i]) { //if x_i = 1
            for (int index_j = index_i; index_j < PARTIAL_INPUTS; ++index_j) { //iterate through x_j's
                if (input[index_j]) { //if x_j = 1
                    //XOR constants by appropriate row from super
                    const BitVector<NUM_OUTPUTS> & superCoeffRowIJ = super._matrix[index_j - index_i];
                    xorConstants(superCoeffRowIJ);
                }
            }
        }
        updateConstants(super.next, input, BitVector<SUPER_LIMIT + PARTIAL_INPUTS - SUPER_INPUTS - 1>()); //go to x_{j - 1}
    }

    /*
     * Function: updateConstants(super, input, dummy)
     * In partial evaluation of super, updates the constants of result
     * Iterates through the coefficient matrices x_i of super
     * and XORs in the first PARTIAL_INPUTS rows into constants
     * Triggered when i = PARTIAL_INPUTS (i.e. SUPER_LIMIT = SUPER_INPUTS - PARTIAL_INPUTS)
     */
    template<unsigned int SUPER_INPUTS, unsigned int PARTIAL_INPUTS, unsigned int SUPER_LIMIT>
    void updateConstants(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS, SUPER_LIMIT> & super, const BitVector<PARTIAL_INPUTS> & input, const BitVector<0> & dummy) {
        // NO-OP base case
    }

/* Composition */

    /*
     * Operator: * inner
     * Evaluates the MultiQuadTuple composed with a given matrix (e.g. f \circ inner)
     */
    template<unsigned int NUM_INNER_INPUTS>
    const MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> operator*( const BitMatrix<NUM_INPUTS, NUM_INNER_INPUTS> & inner) const{
        MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> cs;
        cs.setAsConstants(getConstants()); //reset contribution matrix and force constants
        compose(cs, inner.transpose());
        return cs;
    }

    /*
     * Function: rMult(rhs)
     * Evaluates the MultiQuadTuple pre-composed with a given matrix (e.g. rhs \circ f)
     * TODO: rename
     */
    template<unsigned int NEW_NUM_OUTPUTS>
    const MultiQuadTuple<NUM_INPUTS,NEW_NUM_OUTPUTS> rMult(const BitMatrix<NEW_NUM_OUTPUTS, NUM_OUTPUTS> & rhs) const {
        MultiQuadTuple<NUM_INPUTS,NEW_NUM_OUTPUTS> cs;
        cs.multiplyAndSet(*this, rhs, rhs.transpose());
        return cs;
    }

/* State modifiers */

    /*
     * Function: setConstants(v)
     * Sets the constants to a given vector
     * Calls next until template specialization at LIMIT = 0
     */
    void setConstants(const BitVector<NUM_OUTPUTS> &v) {
        next.setConstants(v);
    }

    /*
     * Function: setConstants(v)
     * Xors the constants with a given vector
     * Calls next until template specialization at LIMIT = 0
     */
    void xorConstants(const BitVector<NUM_OUTPUTS> &v){
        next.xorConstants(v);
    }

    /*
     * Function: setMatrix(m)
     * Sets the NUM_INPUTS - STOP_ROWS matrix to a given matrix
     * Calls on next until LIMIT = MONOMIAL_INDEX
     */
    template<unsigned int STOP_ROWS>
    void setMatrix(const BitMatrix<STOP_ROWS, NUM_OUTPUTS> & m) {
        next.setMatrix(m);
    }

    /*
     * Function: setMatrix(m)
     * Sets the NUM_INPUTS - STOP_ROWS matrix to a given matrix
     * Sets when LIMIT = MONOMIAL_INDEX
     */
    void setMatrix(const BitMatrix<LIMIT, NUM_OUTPUTS> & m) {
        _matrix =  m;
    }

    /*
     * Function: xorMatrixN(rowIndex, input, dummy)
     * Xors the rowIndex row of the NUM_INPUTS - STOP_ROWS matrix with the given vector
     * Calls on next until LIMIT = STOP_ROWS
     */
    template<unsigned int STOP_ROWS>
    void xorMatrixRowN(const unsigned int rowIndex, const BitVector<NUM_OUTPUTS> & input, const BitVector<STOP_ROWS> & dummy) {
        next.xorMatrixRowN(rowIndex, input, dummy);
    }

    /*
     * Function: xorMatrixN(rowIndex, input, dummy)
     * Xors the rowIndex row of the NUM_INPUTS - STOP_ROWS matrix with the given vector
     * Xors when LIMIT = STOP_ROWS
     */
    void xorMatrixRowN(const unsigned int rowIndex, const BitVector<NUM_OUTPUTS> & input, const BitVector<LIMIT> & dummy) {
        _matrix[rowIndex] ^= input;
    }

/* Concatenations */

    /*
     * Function: augV(lower, upper)
     * Vertically concatenates two MultiQuadTuples on the same input (e.g. f(x) || g(x))
     */
    template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2>
    void augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1,LIMIT> & lower, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2,LIMIT> & upper) {
        _matrix = BitMatrix<LIMIT, NUM_OUTPUTS>::augH(lower._matrix, upper._matrix);
        next.augV(lower.next, upper.next);
    }

    /*
     * Function: augV(lower, mid, upper)
     * Vertically concatenates three MultiQuadTuples on the same input (e.g. f(x) || g(x) || h(x))
     */
    template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2, unsigned int NUM_OUTPUTS3>
    void augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1,LIMIT> & lower, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2,LIMIT> & mid, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS3,LIMIT> & upper) {
        _matrix = BitMatrix<LIMIT, NUM_OUTPUTS>::augH(lower._matrix, mid._matrix, upper._matrix);
        next.augV(lower.next, mid.next, upper.next);
    }

/* Inter-MultiQuadTuple Arithmetic Operations */

    /*
     * Operator: ^ rhs
     * Xors the matrices and constants of two MultiQuadTuples
     * Calls through each coefficient matrix
     */
    const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, LIMIT> operator^(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, LIMIT> & rhs) const {
        MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, LIMIT> result;
        result._matrix = _matrix ^ rhs._matrix;
        result.next = next ^ rhs.next;
        return result;
    }

/* Print */

    /*
     * Function: printConstants()
     * Prints the constants vector
     */
    void printConstants() const{
        next.printConstants();
    }

/* Helper Functions */

    /*
     * Function: setSubContributions(start, m, v)
     * Helper function for setContributions()
     * Sets each coefficient matrix as a submatrix of the given larger matrix
     * TODO: needs to be updated (args for setMatrix not correct)
     */
    void setSubContributions(unsigned int start, const BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS> &m, const BitVector<NUM_OUTPUTS> &v){
        _matrix.template setMatrix<NUM_INPUT_MONOMIALS>(m, start);
        next.setSubContributions(start + LIMIT, m, v);
    }

    /*
     * Function: compose(root, innerTranspose)
     * Helper function for * operator
     * TODO: needs to be described
     */
    template<unsigned int NUM_INNER_INPUTS>
    void compose(MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> & root, const BitMatrix<NUM_INNER_INPUTS,NUM_INPUTS> &innerTranspose) const {
        const BitMatrix<NUM_INNER_INPUTS, NUM_OUTPUTS> & composedCoefficientMatrix = innerTranspose .template pMult<LIMIT,NUM_OUTPUTS>(_matrix, NUM_INPUTS - LIMIT, 0, LIMIT);
        for(unsigned int i = 0; i < NUM_INNER_INPUTS ; ++i) {
            if(innerTranspose[ i ][ NUM_INPUTS - LIMIT ]) {
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

    /*
     * Function: xorInMonomialContribution(matrixIndex, rowIndex, v)
     * Xors a given vector into the row corresponding to x_matrixIndex x_rowIndex
     */
    void xorInMonomialContribution(unsigned int matrixIndex, unsigned int rowIndex, const BitVector<NUM_OUTPUTS> &v){
        if(matrixIndex == (NUM_INPUTS - LIMIT)){
            _matrix[rowIndex - matrixIndex] ^= v;
        } else {
            next.xorInMonomialContribution(matrixIndex, rowIndex, v);
        }
    }

    /*
     * Function: multiplyAndSet(src, m, mT)
     * Helper function for rMult
     * Set the corresponding matrix of the respective indices
     */
    template<unsigned int OLD_NUM_OUTPUTS>
    void multiplyAndSet(const MultiQuadTuple<NUM_INPUTS, OLD_NUM_OUTPUTS, LIMIT> & src, const BitMatrix<NUM_OUTPUTS, OLD_NUM_OUTPUTS> & m, const BitMatrix<OLD_NUM_OUTPUTS, NUM_OUTPUTS> & mT) {
        _matrix = src._matrix * mT;
        next.multiplyAndSet(src.next, m, mT);
    }

    /*
     * Function: copyConstants(rhs)
     * Sets constants to a given same-sized MultiQuadTuple to the current constants
     */
    template<unsigned int NUM_INNER_INPUTS>
    void copyConstants(MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS,LIMIT> & rhs) const{
        next.template copyConstants<NUM_INNER_INPUTS>(rhs.next);
    }
};


/*
 * Template specialization for MultiQuadTuple at limit = 0
 * Contains constant vector
 * Defines various base case functions for template unrolling
 */
template<unsigned NUM_INPUTS,unsigned int NUM_OUTPUTS>
struct MultiQuadTuple<NUM_INPUTS,NUM_OUTPUTS,0> {
    BitVector<NUM_OUTPUTS> _constants;

/* Base case of MQT Generation */

    /*
     * Function: randomize()
     * Last step of generating a random MultiQuadTuple
     * Sets constants to be a random vector
     */
    void randomize() {
        _constants = BitVector<NUM_OUTPUTS>::randomVector();
    }

    /*
     * Function: zero()
     * Last step of generating a zero MultiQuadTuple
     * Sets constants to be a zero vector
     */
    void zero() {
        _constants = BitVector<NUM_OUTPUTS>::zeroVector();
    }

    /*
     * Function: setAsMatrix(m)
     * Last step of generating a MultiQuadTuple equivalent to a given linear transformation
     * Sets constants to be a zero vector
     */
    void setAsMatrix(const BitMatrix<NUM_OUTPUTS, NUM_INPUTS> &m){
        _constants = BitVector<NUM_OUTPUTS>::zeroVector();
    }

    void setAsMatrixTranspose(const BitMatrix<NUM_INPUTS, NUM_OUTPUTS> &m){
        _constants = BitVector<NUM_OUTPUTS>::zeroVector();
    }

    /*
     * Function: setAsConstants(v)
     * Last step of generating a MultiQuadTuple equivalent to a constant
     * Sets constants to be a given vector
     */
    void setAsConstants(const BitVector<NUM_OUTPUTS> &v){
        _constants = v;
    }

/* Evaluation */

    /*
     * Operator(input)
     * Last step of evaluating on an input vector
     * Returns constants
     */
    const BitVector<NUM_OUTPUTS> & operator() (const BitVector<NUM_INPUTS> & input) const {
        return _constants;
    }

/* State modifiers */

    /*
     * Function: setConstants(v)
     * Sets constants to a given vector
     */
    void setConstants(const BitVector<NUM_OUTPUTS> &v) {
        _constants = v;
    }

    /*
     * Function: xorConstants(v)
     * Xors constants with a given vector
     */
    void xorConstants(const BitVector<NUM_OUTPUTS> &v) {
        _constants ^= v;
    }

    /*
     * Function: setMatrix(m)
     * No-op when setting a non-existent coefficient matrix
     */
    template<unsigned int STOP_ROWS>
    void setMatrix(const BitMatrix<STOP_ROWS, NUM_OUTPUTS> & m ) {
        //NO-OP base case
    }

    /*
     * Function: setToSubMQT(super)
     * Generates a MultiQuadTuple with the last NUM_INPUTS coefficient matrices
     * of a given larger MultiQuadTuple
     * Ends with a template specialization at limit = 0
     * Sets the constants
     */
    template<unsigned int SUPER_INPUTS>
    void setToSubMQT(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS, 0> & super) {
        setConstants(super._constants);
    }

    /*
     * Function: updateCoefficients(super, input)
     * In partial evaluation of super, updates the coefficients of result
     * Base case of NO-OP
     */
    template<unsigned int SUPER_INPUTS, unsigned int PARTIAL_INPUTS>
    void updateCoefficients(const MultiQuadTuple<SUPER_INPUTS, NUM_OUTPUTS> & super, const BitVector<PARTIAL_INPUTS> & input) {
        //NO-OP base case
    }

/* Print */

    /*
     * Function: printConstants()
     * Prints the constants vector
     */
    void printConstants() const{
        _constants.print();
    }

/* Getters */

    /*
     * Function: getConstants()
     * Returns the constants vector
     */
    const BitVector<NUM_OUTPUTS> & getConstants() const{
        return _constants;
    }

/* Concatenations */

    /*
     * Function: augV(lower, upper)
     * Vertically concatenates the constants of two MultiQuadTuples on the same input (e.g. f(x) || g(x))
     */
    template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2>
    void augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1, 0> & lower, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2, 0> & upper) {
        _constants = BitVector<NUM_OUTPUTS>::vCat(lower._constants , upper._constants);
    }

    /*
     * Function: augV(lower, mid, upper)
     * Vertically concatenates the constants of three MultiQuadTuples on the same input (e.g. f(x) || g(x) || h(x))
     */
    template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2, unsigned int NUM_OUTPUTS3>
    void augV(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1, 0> & lower, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2, 0> & mid, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS3, 0> & upper) {
        _constants = BitVector<NUM_OUTPUTS>::vCat(lower._constants , mid._constants, upper._constants);
    }

/* Inter-MultiQuadTuple Arithmetic Operations */

    /*
     * Operator: ^ rhs
     * Xors the constants of two MultiQuadTuples
     */
    const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, 0> operator^(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, 0> & rhs) const {
        MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS, 0> result;
        result._constants = _constants ^ rhs._constants;
        return result;
    }

/* Helper Functions */

    /*
     * Operator: copyConstants(rhs)
     * Sets the constants of a given same-sized MultiQuadTuple to the current constants
     * TODO: flip order?
     */
    template<unsigned int NUM_INNER_INPUTS>
    void copyConstants(MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS, 0> & rhs) const{
        rhs._constants = _constants;
    }

    /*
     * Operator: setSubContributions(m, v)
     * Final step in process of generating a MultiQuadTuple from a large coefficient matrix and a vector
     * Sets constants to a given vector
     */
    void setSubContributions(unsigned int start, const BitMatrix<NUM_INPUT_MONOMIALS, NUM_OUTPUTS> &m, const BitVector<NUM_OUTPUTS> &v){
        _constants = v;
    }

    /*
     * Operator: compose(root, innerTranspose)
     * Helper function for * operator
     * NO-OP base case when LIMIT = 0
     */
    template<unsigned int NUM_INNER_INPUTS>
    void compose(MultiQuadTuple<NUM_INNER_INPUTS,NUM_OUTPUTS> & root, const BitMatrix<NUM_INNER_INPUTS,NUM_INPUTS> & innerTranspose) const {
        //NO-OP base case
    }

    /*
     * Operator: xorInMonomialContribtion(matrixIndex, rowIndex, v)
     * Helper function for xorInMonomialContribution
     * NO-OP base case when LIMIT = 0
     */
    void xorInMonomialContribution(unsigned int matrixIndex, unsigned int rowIndex, const BitVector<NUM_OUTPUTS> &v){
        //NO-OP base case
    }

    /*
     * Operator: multiplyAndSet(src, m, v)
     * Helper function for multiplyAndSet (in rMult)
     * Base case when LIMIT = 0
     */
    template<unsigned int OLD_NUM_OUTPUTS>
    void multiplyAndSet(const MultiQuadTuple<NUM_INPUTS,OLD_NUM_OUTPUTS,0> & src, const BitMatrix<NUM_OUTPUTS,OLD_NUM_OUTPUTS> & m ,const BitMatrix<OLD_NUM_OUTPUTS, NUM_OUTPUTS> & mT) {
        _constants = m * src._constants;
    }
};

#endif