//
//  BitMatrix.h
//  krypto
//
//  Created by Matthew Tamayo on 1/27/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  Row-major order implementation of binary matrix.
//

#ifndef krypto_BitMatrix_h
#define krypto_BitMatrix_h

#include <vector>
#include <fstream>
#include <unordered_set>
#include "BitVector.h"
#include <algorithm>

#define DEBUG false

using namespace std;

/*
 * Template for BitMatrix
 * Bit values are stored in a vector of BitVectors<COLS>
 */
template<unsigned int ROWS = 128, unsigned int COLS = ROWS>
class BitMatrix {
public:
/* Generation */

    /*
     * Function: zeroMatrix()
     * Returns a zero-initialized BitMatrix with a given number of rows
     */
	static const BitMatrix<ROWS,COLS> zeroMatrix() {
		BitMatrix<ROWS,COLS> result;
		for (unsigned int i = 0; i < ROWS; ++i) { //TODO: is the for loop necessary? surely there is a faster way (memset or something)
			result._rows[i] = BitVector<COLS>::zeroVector();
		}
		return result;
	}

    /*
     * Function: randomMatrix(numRows)
     * Returns a random BitMatrix with a given number of rows
     */
	static const BitMatrix<ROWS,COLS> randomMatrix() {
		BitMatrix<ROWS,COLS> result;
		for (unsigned int i = 0; i < ROWS; ++i) {
			result._rows[i] = BitVector<COLS>::randomVector();
		}
        return result;
	}

    /*
     * Function: randomInvertibleMatrix()
     * Returns a random non-identity invertible BitMatrix
     * Uses modified version of the algorithm by D. Randall
     */
	static const BitMatrix<ROWS> randomInvertibleMatrix() {
		BitMatrix<ROWS> A = BitMatrix<ROWS>::zeroMatrix();
		BitMatrix<ROWS> T = BitMatrix<ROWS>::zeroMatrix();

		unordered_set<int> usedRows;
		unsigned int leadingZeroes = 0; //tracks the smallest unused r

		for (int minorIndex = 0; minorIndex < ROWS; ++minorIndex) {
			BitVector<ROWS> v = BitVector<ROWS>::randomVectorLeadingZeroes(leadingZeroes); //leading zeroes for performance
			int r = v.getFirstOne();
			while (usedRows.count(r) > 0 || r == -1) { //generate random nonzero vector v with unused first nonzero coord
				v = BitVector<ROWS>::randomVectorLeadingZeroes(leadingZeroes);
				r = v.getFirstOne();
			}

			A.set(minorIndex, r); //first row of minor set to e_r
			BitVector<ROWS> randomVector = BitVector<ROWS>::randomVector(); //used to extract random values
			for (int row = minorIndex + 1; row < ROWS; row++) {
				if (randomVector[row]) A.set(row, r); //sets random bits below e_r in r-th column
			}

			for (auto it = usedRows.cbegin(); it != usedRows.cend(); ++it) {
				v.clear(*it); //set intersection of row with prev cols to 0
			}
			T.setRow(r, v); //set rth row to v
			usedRows.insert(r);
			while (usedRows.count(leadingZeroes) > 0) ++leadingZeroes; //update leadingZeroes
		}
		return A*T;
	}

    /*
     * Function: identityMatrix()
     * Returns an identity BitMatrix
     */
	static const BitMatrix<ROWS> identityMatrix() {
        BitMatrix<ROWS> result = BitMatrix<ROWS>::zeroMatrix();
		for (unsigned int i = 0; i < ROWS; ++i) {
			result._rows[i].set(i);
		}
		return result;
    }

    /*
     * Function: directFromRows(v)
     * Return a matrix whose rows are the elements of the input BitVector vector
     */
    static const BitMatrix<ROWS, COLS> directFromRows(const vector<BitVector<COLS> > & v){
    	if(DEBUG) assert(v.size() == ROWS);
    	BitMatrix<ROWS, COLS> result = BitMatrix<ROWS, COLS>::zeroMatrix();
    	for(unsigned int i = 0; i < ROWS; ++i){
    		result.setRow(i, v.at(i));
    	}
    	return result;
    }

/* Operators */

    /*
     * Operator: []
     * Returns the row a given index
     */
	BitVector<COLS> & operator[](const unsigned int rowIndex) {
		if (DEBUG) assert(rowIndex >= 0 && rowIndex < ROWS);
		return _rows[rowIndex];
	}

    /*
     * Operator: []
     * Returns the row a given index. Allows accessing const BitVector<COLS> & from constant
     * BitMatrix<ROWS,COLS> & for use in mathematical operations.
     */
    const BitVector<COLS> & operator[](const unsigned int rowIndex) const {
        if (DEBUG) assert(rowIndex >= 0 && rowIndex < ROWS);
        return _rows[rowIndex];
    }


    const bool operator!=( const BitMatrix<ROWS,COLS> & rhs ) const {
        for( int i = 0 ; i < ROWS ; ++i ) {
            if( _rows[ i ] != rhs._rows[ i ] ){
                return true;
            }
        }
        return false;
    }

    const bool operator==( const BitMatrix<ROWS,COLS> & rhs ) const {
        for( int i = 0 ; i < ROWS ; ++i ) {
            if( _rows[ i ] != rhs._rows[ i ] ){
                return false;
            }
        }
        return true;
    }

	/*
     * Operator: ^
     * Returns the result of bitwise XOR between matrices
     */
	const BitMatrix<ROWS,COLS> operator^(const BitMatrix<ROWS,COLS> & rhs) const{
		BitMatrix<ROWS,COLS> result = *this;
		for (size_t i = 0; i < ROWS; ++i) {
			result._rows[i] ^= rhs._rows[i];
		}
		return result;
	}

	/*
     * Operator: *
     * Returns the result of matrix * matrix multiplication
     */
	template <unsigned int NEWCOLS>
	const BitMatrix<ROWS,NEWCOLS> operator*(const BitMatrix<COLS,NEWCOLS> & rhs) const{
		BitMatrix<ROWS,NEWCOLS> result = BitMatrix<ROWS,NEWCOLS>::zeroMatrix();
		for (unsigned int i = 0; i < ROWS; ++i) {
			for (unsigned int j = 0; j < COLS ; ++j) {
				if (get(i, j)) {
                    result[i] ^= rhs[ j ];
				}
			}
		}
		return result;
	}

	/*
     * Operator: *
     * Returns the result of matrix * vector multiplication
     */
	BitVector<ROWS> operator*(const BitVector<COLS> &v) const{
		BitVector<ROWS> result = BitVector<ROWS>::zeroVector();
		for (unsigned int i = 0; i < ROWS; ++i) {
			BitVector<COLS> prod = _rows[i] & v;
			if (prod.parity()) {
				result.set(i);
			}
		}
		return result;
	}

	/*
     * Function: equals(rhs)
     * Returns whether the current BitMatrix has the same values
     * as a given BitMatrix
     */
	bool equals(const BitMatrix<ROWS,COLS> & rhs) const{ //untested!
		for (unsigned int i = 0; i < ROWS; ++i) {
			if (!_rows[i].equals(rhs._rows[i])) return false;
		}
		return true;
	}

/* Access and Modification */

    /*
     * Function: get(rowIndex, colIndex)
     * Returns the value at a given entry
     */
	inline bool get(unsigned int rowIndex, unsigned int colIndex) const{
		if (DEBUG) {
			assert(rowIndex >= 0 && rowIndex < ROWS);
			assert(colIndex >= 0 && colIndex < COLS);
		}
		return _rows[rowIndex].get(colIndex);
	}


    /*
     * Function: getRow(rowIndex)
     * Returns a given row as a BitVector
     */
	const BitVector<COLS> getRow(const unsigned int rowIndex) const{
		if (DEBUG) assert(rowIndex >= 0 && rowIndex < ROWS);
        return _rows[rowIndex];
	}

    /*
     * Function: getCol(colIndex)
     * Returns a given column as a BitVector
     */
	const BitVector<ROWS> getCol(const unsigned int colIndex) const{
		if (DEBUG) assert(colIndex >= 0 && colIndex < COLS);
		BitVector<ROWS> v = BitVector<ROWS>::zeroVector();
		for (unsigned int i = 0; i < ROWS; ++i) {
			if (get(i, colIndex)) v.set(i);
		}
		return v;
	}

    /*
     * Function: set(rowIndex, colIndex)
     * Sets the bit at a given index to 1
     */
	inline void set(unsigned int rowIndex, unsigned int colIndex) {
		if (DEBUG) {
			assert(rowIndex >= 0 && rowIndex < ROWS); //"rowIndex out of bound!"
			assert(colIndex >= 0 && colIndex < COLS);	//"colIndex out of bound!"
		}
		_rows[rowIndex].set(colIndex);
	}

    /*
     * Function: clear(rowIndex, colIndex)
     * Sets the bit at a given index to 0
     */
	inline void clear(unsigned int rowIndex, unsigned int colIndex) {
		if (DEBUG) {
			assert(rowIndex >= 0 && rowIndex < ROWS); //"rowIndex out of bound!"
			assert(colIndex >= 0 && colIndex < COLS);	//"colIndex out of bound!"
		}
		_rows[rowIndex].clear(colIndex);
	}

    /*
     * Function: set(rowIndex, colIndex, value)
     * Sets the bit at a given index to a given value
     */
	inline void set(unsigned int rowIndex, unsigned int colIndex, bool value) {
		if (DEBUG) {
			assert(rowIndex >= 0 && rowIndex < ROWS);
			assert(colIndex >= 0 && colIndex < COLS);
		}
		if (value) _rows[rowIndex].set(colIndex);
		else _rows[rowIndex].clear(colIndex);
	}

    /*
     * Function: setRow(rowIndex, v)
     * Sets a given row to a given BitVector
     */
	void setRow(unsigned int rowIndex, const BitVector<COLS> & v) {
		if (DEBUG) assert(rowIndex >= 0 && rowIndex < ROWS);
		_rows[rowIndex] = v;
	}

    /*
     * Function: setCol(colIndex, v)
     * Sets a given col to a given BitVector
     */
	void setCol(unsigned int colIndex, const BitVector<ROWS> & v) {
        if (DEBUG) {
            assert(colIndex >= 0 && colIndex < COLS);
        }

        for (int i = 0; i < ROWS; ++i) {
            set(i, colIndex, v[i]);
        }
	}

	void setCol(unsigned int colIndex) {
        if (DEBUG) assert(colIndex >= 0 && colIndex < COLS);
        for (int i = 0; i < ROWS; ++i){
            set(i, colIndex);
        }
	}


	void clearCol(unsigned int colIndex) {
        if (DEBUG) assert(colIndex >= 0 && colIndex < COLS);
        for (int i = 0; i < ROWS; ++i) {
            clear(i, colIndex);
        }
	}

    /*
     * Function: setCol(colIndex, val)
     * Sets an entire given col to a given constant
     */
	void setCol(unsigned int colIndex, unsigned int val) {
        if (DEBUG) assert( ( colIndex >= 0 ) && ( colIndex < COLS ) );
        if (val) {
            setCol(colIndex);
        } else {
            clearCol(colIndex);
        }
	}

    /*
     * Function: xorRow(rowIndex, row)
     * XORs a given row with a given BitVector
     */
	inline void xorRow(unsigned int rowIndex, const BitVector<COLS> & row) {
		if (DEBUG) assert(rowIndex >= 0 && rowIndex < ROWS);
		_rows[rowIndex] ^= row;
	}

/* Elementary row operations */

	/*
     * Function: addRow(dstIndex, srcIndex)
     * Bitwise adds (XORs) the source row to the destination row
     */
	void addRow(unsigned int dstIndex, unsigned int srcIndex) {
		if (DEBUG) {
			assert(inRowBound(dstIndex) && inRowBound(srcIndex));
		}
		_rows[dstIndex] ^= _rows[srcIndex];
	}

	/*
     * Function: swapRow(firstIndex, secondIndex)
     * Swaps two specified rows
     */
	void swapRows(unsigned int firstIndex, unsigned int secondIndex) {
		if (DEBUG) {
			assert(inRowBound(firstIndex) && inRowBound(secondIndex));
		}
        swap(_rows[firstIndex] , _rows[secondIndex]);
	}

    /*
     * Function: zero()
     * Sets the current BitMatrix to a zero matrix
     */
	void zero() {
		for (size_t i = 0; i < ROWS; ++i)
			_rows[i].zero();
	}

	/*
     * Function: copy(m)
     * Copies a given matrix to the current one
     */
	void copy(const BitMatrix<ROWS, COLS> &m) {
		for (size_t i = 0; i < ROWS; ++i)
			(_rows[i]).copy(m.getRow(i));
	}

    template<unsigned int ROWS0,unsigned int ROWS1>
    void setSubMatrix( const BitMatrix<ROWS0,COLS> & lower, const BitMatrix<ROWS1,COLS> & upper ) {
        if(DEBUG) {
            assert( (ROWS0+ROWS1) <= ROWS );
        }

        for( int i = 0 ; i < ROWS0; ++i ) {
            _rows[ i ] = lower[ i ];
        }

        for( int i = 0 ; i < ROWS1; ++i ) {
            _rows[ i + ROWS0 ] = upper[ i ];
        }
    }

    template<unsigned int ROWS0,unsigned int ROWS1, unsigned int ROWS2>
    void setSubMatrix( BitMatrix<ROWS0,COLS> & lower, BitMatrix<ROWS1,COLS> & upper ) {
        if(DEBUG) {
            assert( (ROWS0+ROWS1) <= ROWS );
        }

        setSubMatrix(lower, upper);

        for( int i = 0 ; i < ROWS2; ++i ) {
            _rows[ i + ROWS0 + ROWS1 ] = upper[ i ];
        }
    }

    /*
     * Function: setSubMatrix(startRowIndex, M)
     * Set a portion of a matrix starting from a given row as given the matrix
     */
    template<unsigned int NEWROWS>
	void setSubMatrix(size_t startRowIndex, const BitMatrix<NEWROWS,COLS> & M) {
		if (DEBUG) assert(startRowIndex >= 0 && startRowIndex + NEWROWS < ROWS);
		for (size_t i = 0; i < NEWROWS; ++i) {
			_rows[i + startRowIndex] = M.getRow(i);
		}
	}

    /*
     * Function: setSubMatrix(M, startRowIndex, numRows)
     * Set a matrix to the submatrix of a given matrix starting from a given row
     */
    template<unsigned int NEWROWS>
	void setMatrix(const BitMatrix<NEWROWS,COLS> & M, unsigned int startRowIndex) {
		//if (DEBUG) {} TODO
		for (unsigned int i = 0; i < ROWS; ++i) {
			_rows[i] = M.getRow(i + startRowIndex);
		}
	}

    /*
     * Function: isIdentity()
     * Returns whether the current BitMatrix is an identity matrix
     */
	bool isIdentity() const {
		if (ROWS != COLS) return false;

		for (unsigned int i = 0; i < ROWS; ++i) {
			for (unsigned int j = 0; j < COLS; ++j) {
				if(i != j){
					if(_rows[i].get(j))
						return false;
				} else {
					if(!_rows[i].get(j))
						return false;
				}
			}
		}
		return true;
	}

	/*
	 * Function: det()
	 * Returns the determinant of a given (square) matrix,
	 * which is 1 iff the right bottom corner of its rref is 1.
	 *
	 * Determinant of a non-square matrix possible? http://math.stackexchange.com/questions/854180/determinant-of-a-non-square-matrix
	 * Read into this later: http://journals.cambridge.org/download.php?file=%2FBAZ%2FBAZ21_01%2FS0004972700011369a.pdf&code=1807973f2c6d49bc4579326df0a7aa58
	 */
	bool det() const{
		if (DEBUG) assert(ROWS == COLS);
		return rref().getRightBottomCorner();
	}

	/*
	 * Function: transpose()
	 * Returns the transpose of a matrix.
	 */
	const BitMatrix<COLS,ROWS> transpose() const {
		BitMatrix<COLS,ROWS> Mt = BitMatrix<COLS,ROWS>::zeroMatrix();

        for (unsigned int i = 0; i < ROWS; ++i) {
			for (unsigned int j = 0; j < COLS; ++j) {
				if (get(i, j)) Mt.set(j, i);
			}
		}
		return Mt;
	}

	/*
	 * Function: trimAndTranspose
	 * Returns the transpose of the submatrix starting from row START_ROW.
	 */
    template<unsigned int START_ROW>
    const BitMatrix<COLS,ROWS-START_ROW> trimAndTranpose() const {
        BitMatrix<COLS,ROWS-START_ROW> Mt = BitMatrix<COLS,ROWS-START_ROW>::zeroMatrix();

        for (int i = 0; i < (ROWS-START_ROW); ++i) {
            for (int j = 0; j < COLS; ++j) {
                if ( get(i + START_ROW, j) ) {
                    Mt.set(j, i);
                }
            }
        }
        return Mt;
    }

	/*
     * Function: inv()
     * Returns the inverse of a square matrix
     */
	const BitMatrix<ROWS> inv() const{
   		if (DEBUG) assert(ROWS==COLS);
		bool invertible;
		return inv(invertible);
	}

	/*
     * Function: inv(invertible)
     * Returns the inverse of a square matrix if invertible
     * Returns a square zero matrix and prints error otherwise
     * Marks invertibility in boolean
     */
	const BitMatrix<ROWS> inv(bool & invertible) const{
		if (DEBUG) assert(ROWS==COLS);
		BitMatrix<ROWS> I = BitMatrix<ROWS>::identityMatrix();
		rref(I);
		return I;
	}

	const BitMatrix<COLS, ROWS> leftInverse() const{
		if(DEBUG)assert(ROWS >= COLS);

		BitMatrix<COLS, ROWS> T = transpose();
		BitMatrix<COLS> inverse = BitMatrix<COLS>::identityMatrix();
		const BitMatrix<COLS, ROWS> & workingSet = T.rref(inverse);
		vector<bool> basisInitialized(ROWS);
		fill(basisInitialized.begin(), basisInitialized.end(), false);
		BitVector<ROWS> basis[ROWS];
		for(int i = 0; i < ROWS; ++i){
			basis[i] = BitVector<ROWS>::zeroVector();
		}
		unsigned int firstNonZeroIndex[COLS] = {0};
		for(unsigned int i = 0; i < COLS; ++i){
			const BitVector<ROWS> & v = workingSet[i];
			int value = -1;
			for(unsigned int j = 0; j < ROWS; ++j){
				if(v.get(j)){
					if(value == -1){
						value = j;
						firstNonZeroIndex[i] = j;
					} else {
						if(!basisInitialized[j]){
							basis[j] = BitVector<ROWS>::zeroVector();
							basis[j].set(j);
							basisInitialized[j] = true;
						}
						basis[j].set(value);
					}
				}
			}
		}

		vector<BitVector<ROWS> > filtered;
		for(unsigned int i = 0; i < ROWS; ++i){
			if(basisInitialized[i]){
				filtered.push_back(basis[i]);
			}
		}

		if(filtered.size() != (ROWS - COLS)){
			cerr << "Error: matrix has no generalized left inverse" << endl;
			//assert(filtered.size() == (ROWS - COLS));
			return BitMatrix<COLS, ROWS>::zeroMatrix();
		}

		const BitMatrix<COLS> & inverseColumnar = inverse.transpose();
		BitMatrix<COLS, ROWS> constantBasis = BitMatrix<COLS, ROWS>::zeroMatrix();
		for(unsigned int i = 0; i < COLS; ++i){
			constantBasis.setRow(i, mapLeft(firstNonZeroIndex, inverseColumnar[i]));
		}

		const BitMatrix<COLS, ROWS - COLS> & randomizer = BitMatrix<COLS, ROWS - COLS>::randomMatrix();
		const BitMatrix<COLS, ROWS> & nullspan = randomizer.template operator*<ROWS>(BitMatrix<ROWS - COLS, ROWS>::directFromRows(filtered));

		return nullspan ^ constantBasis;
	}

	//helper function of leftInverse
	const BitVector<ROWS> mapLeft(unsigned int firstNonZeroIndex[], const BitVector<COLS> & bv) const{
        BitVector<ROWS> result = BitVector<ROWS>::zeroVector();
        for ( int i = 0; i < COLS; ++i ) {
            if ( bv.get( i ) ) {
                result.set( firstNonZeroIndex[ i ] );
            }
        }
        return result;
    }

	/*
	 * Function: right inverse
	 * Returns the inverse of a matrix of any dimension
	 * Assume matrix is full rank
	 */
	const BitMatrix<COLS, ROWS> rightInverse() const{
		if(DEBUG)assert(COLS >= ROWS);

		BitMatrix<ROWS> inverse = BitMatrix<ROWS>::identityMatrix();
		const BitMatrix<ROWS, COLS> & workingSet = rref(inverse);
		vector<bool> basisInitialized(COLS);
		fill(basisInitialized.begin(), basisInitialized.end(), false);
		BitVector<COLS> basis[COLS];
		for(int i = 0; i < COLS; ++i){
			basis[i] = BitVector<COLS>::zeroVector();
		}
		unsigned int firstNonZeroIndex[ROWS] = {0};
		for(unsigned int i = 0; i < ROWS; ++i){
			const BitVector<COLS> & v = workingSet[i];
			int value = -1;
			for(unsigned int j = 0; j < COLS; ++j){
				if(v.get(j)){
					if(value == -1){
						value = j;
						firstNonZeroIndex[i] = j;
					} else {
						if(!basisInitialized[j]){
							basis[j] = BitVector<COLS>::zeroVector();
							basis[j].set(j);
							basisInitialized[j] = true;
						}
						basis[j].set(value);
					}
				}
			}
		}

		vector<BitVector<COLS> > filtered;
		for(unsigned int i = 0; i < COLS; ++i){
			if(basisInitialized[i]){
				filtered.push_back(basis[i]);
			}
		}

		if(filtered.size() != (COLS - ROWS)){
			cerr << "Error: matrix has no generalized right inverse" << endl;
			//assert(filtered.size() == (COLS - ROWS));
			return BitMatrix<COLS, ROWS>::zeroMatrix();
		}

		const BitMatrix<ROWS> & inverseColumnar = inverse.transpose();
		BitMatrix<ROWS, COLS> constantBasis = BitMatrix<ROWS, COLS>::zeroMatrix();
		for(unsigned int i = 0; i < ROWS; ++i){
			constantBasis.setRow(i, mapRight(firstNonZeroIndex, inverseColumnar[i]));
		}

		const BitMatrix<ROWS, COLS - ROWS> & randomizer = BitMatrix<ROWS, COLS - ROWS>::randomMatrix();
		const BitMatrix<ROWS, COLS> & nullspan = randomizer.template operator*<COLS>(BitMatrix<COLS - ROWS, COLS>::directFromRows(filtered));

		return (nullspan ^ constantBasis).transpose();
	}

	//helper function of rightInverse
	BitVector<COLS> mapRight(unsigned int firstNonZeroIndex[], const BitVector<ROWS> & bv) const{
        BitVector<COLS> result = BitVector<COLS>::zeroVector();
        for ( int i = 0; i < ROWS; ++i ) {
            if ( bv.get( i ) ) {
                result.set( firstNonZeroIndex[ i ] );
            }
        }
        return result;
    }

	/*
     * Function: rref()
     * Returns the reduced row-echelon form of a given matrix by Gaussian elimination
     */
	const BitMatrix<ROWS,COLS> rref() const{
		int row = 0;
		BitMatrix<ROWS,COLS> A = *this;
		int limit = min(ROWS, COLS);
		for(int col = 0; col < COLS; ++col){
			int suitableRow = getSuitableRow(A, col, row);
			if(suitableRow >= 0){
				A.swapRows(row, suitableRow);
	            const BitVector<COLS> & currentRow = A[row];
	            for ( int candidateIndex = 0; candidateIndex < limit; ++candidateIndex ) {
	                if ( candidateIndex != row ) {
	                    BitVector<COLS> candidateRow = A[candidateIndex];
	                    if ( candidateRow.get( col ) ) {
	                    	A.xorRow(candidateIndex, currentRow);
	                    }
	                }
				}
				++row;
			}
		}
		return A;
	}

	/*
     * Function: rref(rhs)
     * Returns the reduced row-echelon form of a given matrix by Gaussian elimination
     */
	const BitMatrix<ROWS, COLS> rref(BitMatrix<ROWS> & rhs) const{
		int row = 0;
		BitMatrix<ROWS,COLS> A = *this;
		int limit = min(ROWS, COLS);
		for(int col = 0; col < COLS; ++col){
			int suitableRow = getSuitableRow(A, col, row);
			if(suitableRow >= 0){
				A.swapRows(row, suitableRow);
				rhs.swapRows(row, suitableRow);
	            const BitVector<COLS> & currentRow = A[row];
	            for ( int candidateIndex = 0; candidateIndex < limit; ++candidateIndex ) {
	                if ( candidateIndex != row ) {
	                    BitVector<COLS> candidateRow = A[candidateIndex];
	                    if ( candidateRow.get( col ) ) {
	                    	A.xorRow(candidateIndex, currentRow);
	                        rhs.addRow( candidateIndex, row );
	                    }
	                }
				}
				++row;
			}
		}
		return A;
	}

	//helper function for RREF
	const int getSuitableRow(const BitMatrix<ROWS, COLS> & input, int col, int startRow) const{
		for(int i = startRow; i < ROWS; ++i){
			if(input[i][col]){
				return i;
			}
		}
		return -1;
	}

	/* Matrix-vector Operations */

	/*
     * Function: tMult(v)
     * Returns the vector result of multiplication by the transpose of the current matrix
     * Ex. A.tMult(v) := A^T * v in F_2^{m}
     */
	const BitVector<COLS> tMult(const BitVector<ROWS> & v) const {
		BitVector<COLS> result = BitVector<COLS>::zeroVector();
		for (unsigned int i = 0; i < ROWS; ++i) {
			if (v[i]) result ^= _rows[i];
		}
		return result;
	}

	/*
     * Function: solve(rhs)
     * Returns the vector result of multiplication by the inverse of the current matrix
     * Ex. A.solve(v) = A^{-1} v
     * Assumes current matrix is invertible
     */
	const BitVector<ROWS> solve (const BitVector<ROWS> & rhs) const{
		bool solvable;
		return solve(rhs, solvable);
	}

	/*
     * Function: solve(rhs)
     * Returns the vector result of multiplication by the inverse of the current matrix if invertible
     * Returns zero vector and prints error otherwise
     * Ex. A.solve(v) = A^{-1} v
     * Marks if the current matrix is invertible
     */
	const BitVector<ROWS> solve (const BitVector<ROWS> & rhs, bool & solvable) const{
		if (DEBUG) assert(ROWS == COLS);
		BitMatrix<ROWS> A = *this;
		BitVector<ROWS> b = rhs;
		for (int k = 0; k < ROWS; ++k) {
			int pos = -1, i = k;
			while(i < ROWS) { //find the first pos >= k with A[pos,k] == 1
				if (A.get(i, k)) {pos = i; break;}
				++i;
			}
			if (pos != -1) {
				if (pos != k) {
					A.swapRows(pos, k);
					b.swap(pos, k);
				}
				for (int i = k+1; i < ROWS; ++i) if (A.get(i, k)) {
					A.setRow(i, A[i] ^ A[k]);
					b.set(i, b[i]^b[k]);
				}
			} else {
				cerr << "Error: solving system of a singular matrix!" << endl;
				solvable = false;
				return BitVector<ROWS>::zeroVector(); //this is when A is singular
			}
		}
		BitVector<ROWS> x = BitVector<ROWS>::zeroVector();
		for (int i = ROWS-1; i >= 0; --i) x.set(i, x.dot(A[i]) ^ b[i]);
		solvable = true;
		return x;
	}

/* Multi-Matrix Functions */

	/*
     * Function: pMult(rhs, startCol, startRow, range)
     * Returns the result of multiplication of a given matrix
     * by a submatrix of the current matrix as specified by
     * given rows and columns
     */
	template<unsigned int RHS_ROWS, unsigned int RHS_COLS>
	const BitMatrix<ROWS, RHS_COLS> pMult(const BitMatrix<RHS_ROWS, RHS_COLS> & rhs,
		int startCol, int startRow, int range) const{
		if (DEBUG) {
			assert(startCol >= 0 && startCol + range <= COLS);
			assert(startRow >= 0 && startRow + range <= RHS_ROWS);
		}
		BitMatrix<ROWS, RHS_COLS> result = BitMatrix<ROWS, RHS_COLS>::zeroMatrix();

		for (int j = 0; j < range; ++j) {
			int rhsRow = startRow + j;
			for (int i = 0; i < ROWS; ++i) {
				if (get(i, startCol + j)) {
					result[i] ^= rhs[rhsRow];
				}
			}
		}
		return result;
	}

	template<unsigned int RHS_ROWS, unsigned int RHS_COLS>
	const BitMatrix<ROWS, RHS_COLS> pMult(const BitMatrix<RHS_ROWS, RHS_COLS> & rhs, int startRow) const{
		return pMult<RHS_ROWS, RHS_COLS>(rhs, 0, startRow, COLS);
	}

	/*
     * Function: pMult(v, startCol, startRow, range)
     * Returns the result of multiplication of a given vector
     * by a submatrix of the current matrix as specified by
     * given rows and columns
     */
	//const BitVector<ROWS> pMult(const BitVector<COLS> & v, unsigned int startCol,
	//	unsigned int endCol, unsigned int startIndex, unsigned int endIndex) const{
	const BitVector<ROWS> pMult(const BitVector<COLS> & v, int startCol, int startIndex, int range) const{
		if (DEBUG) {
			assert(startCol >= 0 && startCol + range <= COLS);
			assert(startIndex >= 0 && startIndex + range <= COLS);
		}
		BitVector<ROWS> result = BitVector<ROWS>::zeroVector();

		for (int j = 0; j < range; ++j) {
			size_t rhsIndex = startIndex + j;
			for (size_t i = 0; i < ROWS; ++i) {
				if (get(i, startCol + j)) {
					result.set(i, result.get(i) ^ v.get(rhsIndex));
				}
			}
		}
	}

	/*
     * Function: augH(lhs, rhs)
     * Returns the matrix resulting from horizontal augmentation
     * of two given matrices
     * Assumes the given matrixes have the same number of rows
     */
	template <unsigned int COLS1, unsigned int COLS2> //TODO: optimize
	static const BitMatrix<ROWS,COLS1 + COLS2> augH (const BitMatrix<ROWS,COLS1> & lhs, const BitMatrix<ROWS,COLS2> & rhs) {
		BitMatrix<ROWS, COLS1+COLS2> augmented;
        for (unsigned int i = 0; i < ROWS; ++i) {
			const BitVector<COLS1> & lv = lhs.getRow(i);
			const BitVector<COLS2> & rv = rhs.getRow(i);
            augmented.setRow(i, BitVector<COLS1+COLS2>::vCat(lv, rv));
		}
        return augmented;
	}

	/*
     * Function: augH(lhs, mid, rhs)
     * Returns the matrix resulting from horizontal augmentation
     * of three given matrices
     * Assumes the given matrixes have the same number of rows
     */
	template <unsigned int COLS1, unsigned int COLS2, unsigned int COLS3> //TODO: optimize
	static const BitMatrix<ROWS, COLS1 + COLS2 + COLS3> augH(const BitMatrix<ROWS, COLS1> & lhs, const BitMatrix<ROWS, COLS2> & mid, const BitMatrix<ROWS, COLS3> & rhs) {
        BitMatrix<ROWS, COLS1+COLS2+COLS3> augmented;
        for (unsigned int i = 0; i < ROWS ; ++i) {
            const BitVector<COLS1> & lv = lhs.getRow(i);
            const BitVector<COLS2> & mv = mid.getRow(i);
            const BitVector<COLS3> & rv = rhs.getRow(i);
            augmented.setRow(i, BitVector<COLS1+COLS2+COLS3>::vCat( lv, mv, rv ));
		}
        return augmented;
	}

	/*
     * Function: augV(top, bot)
     * Returns the matrix resulting from vertical augmentation
     * of two given matrices
     * Assumes the given matrixes have the same number of rows
     */
    template <unsigned int ROWST, unsigned int ROWSB >
	static const BitMatrix<ROWST+ROWSB, COLS> augV(const BitMatrix<ROWST, COLS> & top, const BitMatrix<ROWSB, COLS> & bot) { //TODO: optimize
        BitMatrix<ROWST+ROWSB, COLS> result;

        unsigned int i;

        for(i = 0 ; i < ROWST ; ++i ) {
            result.setRow(i, top.getRow(i));
        }

        for(i = 0 ; i < ROWSB ; ++i ) {
            result.setRow(i + ROWST, bot.getRow(i));
        }

		return result;
	}

	/*
     * Function: augV(top, mid, bot)
     * Returns the matrix resulting from vertical augmentation
     * of two given matrices
     * Assumes the given matrixes have the same number of rows
     */
    template <unsigned int ROWST, unsigned int ROWSM, unsigned int ROWSB >
	static const BitMatrix<ROWST+ROWSM + ROWSB, COLS> augV(const BitMatrix<ROWST,COLS> & top, const BitMatrix<ROWSM,COLS> & mid, const BitMatrix<ROWSB,COLS> & bot) { //TODO: optimize
        BitMatrix<ROWST+ROWSM + ROWSB, COLS> result;

        unsigned int i;
        unsigned int count = 0;
        //for( BitVector<COLS> v : top._rows ){
        for(i = 0; i < ROWST; ++i){
            result.setRow(i, top.getRow(i));
            ++count;
        }

        //for( BitVector<COLS> v : mid._rows ){
        for(i = 0; i < ROWSM; ++i){
            result.setRow(count, mid.getRow(i));
            ++count;
        }

        //for( BitVector<COLS> v : bot._rows ){
        for(i = 0; i < ROWSB; ++i){
            result.setRow(count, bot.getRow(i));
            ++count;
        }

        return result;
	}

	/*
     * Function: splitH2(index)
     * Returns the left or right half of the current BitMatrix
     * Assumes the the number of columns is divisible by 2
     */
	//TODO: unify 2 functions below if needed
	const BitMatrix<ROWS, COLS/2> splitH2(int index) const{
        //TODO: use #define
		if (DEBUG) assert(index >= 0 && index < 2); //index not OB
		BitMatrix<ROWS,COLS / 2> result;
		for (int i = 0; i < ROWS; ++i) {
			BitVector<COLS / 2> sv = getRow(i).proj2(index);
			result.setRow(i, sv);
		}
		return result;
	}

	/*
     * Function: splitH3(index)
     * Returns a specified horizontal third of the current BitMatrix
     * Assumes the the number of columns is divisible by 3
     */
	const BitMatrix<ROWS,COLS/3> splitH3(int index) const{
		if (DEBUG) assert(index >= 0 && index < 3); //index not OB
		BitMatrix<ROWS,COLS/3> result = BitMatrix<ROWS,COLS/3>::zeroMatrix();
		for (int i = 0; i < ROWS; ++i) {
			result.setRow(i, getRow(i).proj3(index));
		}
		return result;
	}

	/*
     * Function: splitV2(index)
     * Returns the top or bottom half of the current BitMatrix
     * Assumes the the number of rows is divisible by 2
     */
	const BitMatrix<ROWS/2,COLS> splitV2 (int index) const {
        if( index == 0 ) {
            return topHalf();
        } else {
            return bottomHalf();
        }
	}

    const BitMatrix<ROWS/2, COLS> topHalf() const {
        if (DEBUG) {
            assert( ROWS%2 == 0 );
        }

        BitMatrix<ROWS/2,COLS> result;

        for( int i = 0 ; i < (ROWS>>1) ; ++i ) {
            result.setRow(i, _rows[i]);
        }
        return result;
    }

    const BitMatrix<ROWS/2, COLS> bottomHalf() const {
        if (DEBUG) {
            assert( ROWS%2 == 0 );
        }
        const unsigned int SUBROWS = ROWS >> 1;
        BitMatrix<ROWS/2,COLS> result;

        for( unsigned int i = 0 ; i < SUBROWS ; ++i ) {
            result.setRow(i, _rows[i + SUBROWS]);
        }
        return result;
    }

	/*
     * Function: splitV3(index)
     * Returns a specified vertical third of the current BitMatrix
     * Assumes the the number of rows is divisible by 3
     */
	const BitMatrix<ROWS/3,COLS> splitV3 (int index) const{
        if (DEBUG) {
            assert( index >= 0 && index < 3 );
            assert( (ROWS%3) == 0 );
        }
        BitMatrix<ROWS/3,COLS> result;
   		const unsigned int SUBROWS = ROWS / 3;
        const unsigned int start = index*SUBROWS;
        for( unsigned int i = 0; i < SUBROWS ; ++i ) {
            result.setRow(i, _rows [ i + start ]);
        }
        return result;
	}

	/*
     * Function: projMatrix(start, end)
     * Return a projection matrix for the coordinates between start and end inclusive
     * The projection matrix has 1s in the diagonal between the given indices inclusive
     */
	const static BitMatrix<ROWS,COLS> projMatrix (unsigned int start, unsigned int end) {
		if (DEBUG) assert(inColBound(start) && inColBound(end) && start <= end);
		BitMatrix<ROWS,COLS> result = BitMatrix<ROWS,COLS>::zeroMatrix();

		for (unsigned int i = start; i <= end; ++i) {
			result.set(i, i);
		}
		return result;
	}

	/* Print */

    /*
     * Function: printRow()
     * Prints the values of a given row
     */
	void printRow(unsigned int rowIndex) const {
		if (DEBUG) assert(inRowBound(rowIndex));
		cout << "|" << get(rowIndex, 0);
		for (unsigned int i = 1; i < COLS; ++i) {
			cout << " " << get(rowIndex, i);
		}
		cout << "|" << endl;
	}

	void printRows(unsigned int startRowIndex, unsigned int endRowIndex) const{
		if (DEBUG) {
			assert(startRowIndex <= endRowIndex);
			assert(inRowBound(startRowIndex));
			assert(endRowIndex < ROWS);
		}
		for (unsigned int i = startRowIndex; i <= endRowIndex; ++i) {
			printRow(i);
		}
	}

    /*
     * Function: printLastRow()
     * Prints the values of the last row (for debugging)
     */
	void printLastRow() const{
		unsigned int last = ROWS - 1;
		_rows[last].print();
	}

    /*
     * Function: printCol()
     * Prints the values of a given column
     */
	void printCol(unsigned int colIndex) const {
		if (DEBUG) assert(inColBound(colIndex));
		cout << get(0, colIndex);
		for (unsigned int i = 1; i < ROWS; ++i) {
			cout << " " << get(i, colIndex);
		}
		cout << endl;
	}

    /*
     * Function: print()
     * Prints the values of the matrix row-by-row
     */
	void print() const {
		for (unsigned int i = 0; i < ROWS; ++i) {
			printRow(i);
		}
	}

    /*
     * Function: writeRow()
     * Writes the values of the matrix into a file
     */
	void writeMatrix(const string filename) const{
		ofstream ofs;
		ofs.open(filename);
		for (unsigned int i = 0; i < ROWS; ++i) {
			ofs << get(i, 0);
			for (unsigned int j = 1; j < COLS; ++j) {
				ofs << " " << get(i, j);
			}
			ofs << endl;
		}
		ofs.close();
	}

    // Returns the number of rows
	const inline unsigned int rowCount() const {
		return ROWS;
	}

    // Returns the number of columns
	const inline unsigned int colCount() const {
		return COLS;
	}

private:
	BitVector<COLS> _rows[ROWS];

    /*
     * Function: getRightBottomCorner()
     * Returns the value of the bottom right corner entry
     */
	inline bool getRightBottomCorner() const{
		return get(ROWS-1, COLS-1);
	}

	static inline bool inRowBound(unsigned int rowIndex){
		return (rowIndex >= 0 && rowIndex < ROWS);
	}

	static inline bool inColBound(unsigned int colIndex){
		return (colIndex >= 0 && colIndex < COLS);
	}
};

/*
 * Special template for 0-sized BitMatrix that ensures that the
 * size in memory is at least 1 byte
 */
template<unsigned int ROWS>
class BitMatrix<ROWS, 0> {
private:
    unsigned char x;
};

/*
 * Special template for 0-sized BitMatrix that ensures that the
 * size in memory is at least 1 byte
 */
template<unsigned int COLS>
class BitMatrix<0, COLS> {
private:
    unsigned char x;
};

/*
 * Special template for 0-sized BitMatrix that ensures that the
 * size in memory is at least 1 byte
 */
template<>
class BitMatrix<0, 0> {
private:
    unsigned char x;
};

/*
 * Function: hash<BitMatrix<M, N>>()(id)
 * Hashes a BitMatrix by taking the sum of the standard hashes of its components
 */

template<unsigned int N, unsigned int M>
struct std::hash< BitMatrix<M, N> >
{
    size_t operator()(BitMatrix<M, N> const& matrix) const
    {
    	size_t sum = 0;
    	for (int i = 0; i < M; ++i) {
    		size_t const h ( hash<BitVector<N> >()(matrix.getRow(i)) );
    		sum ^= h;
    	}
        return sum;
    }
};

#endif