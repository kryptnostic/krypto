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
#include <pthread.h>
#include "../../main/cpp/BitVector.h"
//#include "../../../contrib/gtest/gtest.h"

using namespace std;

#define DEBUG false
#define NUM_THREADS 5

/*
 * Template for BitMatrix
 * Bit values are stored in a vector of BitVectors<COLS>
 */
template<unsigned int COLS>
class BitMatrix {
public:

/* Constructors */

	/*
     * Constructor
     * Constructs a zero-initialized BitMatrix with a given number of rows 
     */
	BitMatrix(int numRows) :
			_rows(numRows) {
	}

	/*
     * Constructor
     * Constructs a BitMatrix with a given vector of BitVectors
     */
	BitMatrix(const vector< BitVector<COLS> > & rows) :
			_rows(rows) {
	}

	/*
     * Copy Constructor
     * Constructs a copy of an input BitMatrix
     */
	BitMatrix(const BitMatrix<COLS> & m) : _rows( m._rows ){}

/* Generation */

    /*
     * Function: zeroMatrix(numROws)
     * Returns a zero-initialized BitMatrix with a given number of rows
     */
	static const BitMatrix zeroMatrix(const int numRows){
		vector< BitVector<COLS> > rows(numRows);
		for(int i = 0; i < numRows; ++i){ //TODO: is the for loop necessary? surely there is a faster way (memset or something)
			rows[i] = BitVector<COLS>::zeroVector();
		}
		return BitMatrix(rows);
	}

    /*
     * Function: squareZeroMatrix()
     * Returns a zero-initialized square BitMatrix
     */
	static const BitMatrix squareZeroMatrix(){
		return zeroMatrix(COLS << 6);
	}

    /*
     * Function: randomMatrix(numRows)
     * Returns a random BitMatrix with a given number of rows
     */
	static const BitMatrix randomMatrix(const int numRows) {
		vector< BitVector<COLS> > rows(numRows);
		for (int i = 0; i < numRows; ++i) {
			rows[i] = BitVector<COLS>::randomVector();
		}
		return BitMatrix(rows);
	}

    /*
     * Function: squareRandomMatrix()
     * Returns a random square BitMatrix
     */
	static const BitMatrix squareRandomMatrix(){
		return randomMatrix(COLS << 6);
	}

    /*
     * Function: randomInvertibleMatrix()
     * Returns a random non-identity invertible BitMatrix
     * Uses modified version of the algorithm by D. Randall
     */
	static const BitMatrix randomInvertibleMatrix() {
		int numRows = COLS << 6;
		BitMatrix<COLS> A = BitMatrix<COLS>::squareZeroMatrix();
		BitMatrix<COLS> T = BitMatrix<COLS>::squareZeroMatrix();

		unordered_set<int> usedRows;
		unsigned int leadingZeroes = 0; //tracks the smallest unused r
		//bool usedRows[numRows];
		//memset (&usedRows, 0, numRows * sizeof(bool));

		for(int minorIndex = 0; minorIndex < numRows; ++minorIndex) {
			BitVector<COLS> v = BitVector<COLS>::randomVectorLeadingZeroes(leadingZeroes); //leading zeroes for performance
			int r = v.getFirstOne();
			//while (usedRows[r] && !v.isZero()) { //generate random nonzero vector v with unused first nonzero coord
			while (usedRows.count(r) > 0 || r == -1) { //generate random nonzero vector v with unused first nonzero coord
				v = BitVector<COLS>::randomVectorLeadingZeroes(leadingZeroes);
				r = v.getFirstOne();
			}

			A.set(minorIndex, r); //first row of minor set to e_r

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
     * Function: squareIdentityMatrix()
     * Returns a square identity BitMatrix
     */
	static const BitMatrix squareIdentityMatrix(){
		size_t numRows = _colCount;
		vector< BitVector<COLS> > rows(numRows);
		for(int i = 0; i < numRows; ++i){
			BitVector<COLS> e = BitVector<COLS>::zeroVector();
			e.set(i);
			rows[i] = e;
		}
		return BitMatrix(rows);
		
	}

/* Operators */

    /*
     * Operator: []
     * Returns the row a given index
     */
	BitVector<COLS> & operator[](const int rowIndex){
		if(DEBUG) assert(rowIndex >= 0 && rowIndex < rowCount()); //"rowIndex out of bound!"
		return _rows[rowIndex];
	}

	/*
     * Operator: *
     * Returns the result of matrix * matrix multiplication
     */
	template<unsigned int NEWCOLS>
	const BitMatrix<NEWCOLS> operator*(const BitMatrix<NEWCOLS> & rhs) const{
		const size_t numCols = colCount();
		if(DEBUG) assert(numCols == rhs.rowCount());
		size_t numRows = rowCount();
		BitMatrix<NEWCOLS> result(numRows);
		for(size_t i = 0; i < numRows; ++i){
			for(size_t j = 0; j < numCols; ++j){
				if(get(i, j)){
					result.setRow(i, result.getRow(i) ^ rhs.getRow(j));
				}
			}
		}
		return result;
	}

	/*
     * Operator: *
     * Returns the result of matrix * vector multiplication
     */
	template <unsigned int NEWCOLS>
	const BitVector<NEWCOLS> operator*(const BitVector<COLS> & v) const{
		BitVector<NEWCOLS> result;
		size_t numRows = _rows.size();
		for (size_t i = 0; i < numRows; ++i) {
			BitVector<COLS> prod = _rows[i] & v;
			if (prod.parity()) {
				result.set(i);
			}
		}
		return result;
	}

	/*
     * Operator: *
     * Returns the result of bitwise XOR between matrices
     */
	const BitMatrix<COLS> operator^(const BitMatrix<COLS> & rhs) const{
		BitMatrix<COLS> result = *this;
		const unsigned int numRows = rowCount();
		if(DEBUG) assert(numRows == rhs.rowCount());
		for(size_t i = 0; i < numRows; ++i){
			result._rows[i] ^= rhs._rows[i];
		}
		return result;
	}

	/*
     * Function: equals(rhs)
     * Returns whether the current BitMatrix has the same values
     * as a given BitMatrix
     */
	bool equals(const BitMatrix<COLS> & rhs) const{ //untested!
		unsigned int n = rowCount();
		if(DEBUG) assert(n == rhs.rowCount());
		for(size_t i = 0; i < n; ++i){
			if(!_rows[i].equals(rhs._rows[i])) return false;
		}
		return true;
	}

/* Access and Modification */

    /*
     * Function: get(rowIndex, colIndex)
     * Returns the value at a given entry
     */
	inline bool get(int rowIndex, int colIndex) const{ 
		if(DEBUG){
			assert(rowIndex >= 0 && rowIndex < rowCount()); //"rowIndex out of bound!"
			assert(colIndex >= 0 && colIndex < colCount()); //"colIndex out of bound!"
		}
		return _rows[rowIndex].get(colIndex);
	}


    /*
     * Function: getRow(rowIndex)
     * Returns a given row as a BitVector
     */
	const inline BitVector<COLS> & getRow(const int rowIndex) const{
		if(DEBUG) assert(rowIndex >= 0 && rowIndex < rowCount()); //"rowIndex out of bound!"
		return _rows[rowIndex];
	}

    /*
     * Function: getCol(colIndex)
     * Returns a given column as a BitVector
     */
	template<unsigned int ROWS>
	const BitVector<ROWS> & getCol(const int colIndex) const{
		if(DEBUG){
			assert(colIndex >= 0 && colIndex < colCount());
			int numRows = rowCount();
			assert(numRows == ROWS << 6);			
		}
		BitVector<ROWS> v = BitVector<ROWS>::zeroVector();
		for(int i = 0; i < COLS; ++i){
			if(get(i, colIndex)) v.set(i); 
		}
		return v;
	}

    /*
     * Function: set(rowIndex, colIndex)
     * Sets the bit at a given index to 1
     */
	inline void set(int rowIndex, int colIndex){
		if(DEBUG){
			assert(rowIndex >= 0 && rowIndex < rowCount()); //"rowIndex out of bound!"
			assert(colIndex >= 0 && colIndex < colCount());	//"colIndex out of bound!"
		}
		_rows[rowIndex].set(colIndex);	
	}

    /*
     * Function: clear(rowIndex, colIndex)
     * Sets the bit at a given index to 0
     */
	inline void clear(int rowIndex, int colIndex){
		if(DEBUG){
			assert(rowIndex >= 0 && rowIndex < rowCount()); //"rowIndex out of bound!"
			assert(colIndex >= 0 && colIndex < colCount());	//"colIndex out of bound!"
		}
		_rows[rowIndex].clear(colIndex);		
	}

    /*
     * Function: set(rowIndex, colIndex, value)
     * Sets the bit at a given index to a given value
     */
	inline void set(int rowIndex, int colIndex, bool value){
		if(DEBUG){
			assert(rowIndex >= 0 && rowIndex < rowCount()); //"rowIndex out of bound!"
			assert(colIndex >= 0 && colIndex < colCount());	//"colIndex out of bound!"
		}
		if (value) _rows[rowIndex].set(colIndex);	
		else _rows[rowIndex].clear(colIndex);
	}

    /*
     * Function: setRow(rowIndex, v)
     * Sets a given row to a given BitVector
     */
	void setRow(int rowIndex, BitVector<COLS> v){ 
		if(DEBUG) assert(rowIndex >= 0 && rowIndex < rowCount());
		_rows[rowIndex] = v;
	}

    /*
     * Function: setCol(colIndex, v)
     * Sets a given col to a given BitVector
     */
	template <unsigned int ROWS>
	void setCol(int colIndex, const BitVector<ROWS> & v){
		if(DEBUG) assert(colIndex >= 0 && colIndex < colCount());
		int numRows = ROWS << 6;
		for(int i = 0; i < numRows; ++i) set(i, colIndex, v[i]);
	}

	template <unsigned int ROWS>
	void setCol(int colIndex){
		if(DEBUG) assert(colIndex >= 0 && colIndex < colCount());
		int numRows = ROWS << 6;
		for(int i = 0; i < numRows; ++i) set(i, colIndex);
	}

	template <unsigned int ROWS>
	void clearCol(int colIndex){
		if(DEBUG) assert(colIndex >= 0 && colIndex < colCount());
		int numRows = ROWS << 6;
		for(int i = 0; i < numRows; ++i) clear(i, colIndex);
	}

    /*
     * Function: setCol(colIndex, val)
     * Sets an entire given col to a given constant
     */
	template <unsigned int ROWS>
	void setCol(int colIndex, unsigned int val){
		if(DEBUG) assert(colIndex >= 0 && colIndex < colCount());
		int numRows = ROWS << 6;
		if(val) setCol<ROWS>(colIndex);
		else clearCol<ROWS>(colIndex);
		//for(int i = 0; i < numRows; ++i) set(i, colIndex, val);
	}

    /*
     * Function: xorRow(rowIndex, row)
     * XORs a given row with a given BitVector
     */
	inline void xorRow(int rowIndex, const BitVector<COLS> & row){
		if(DEBUG) assert(rowIndex >= 0 && rowIndex < rowCount());
		_rows[rowIndex] ^= row;
	}

    /*
     * Function: zero()
     * Sets the current BitMatrix to a zero matrix
     */
	void zero() { 
		for (size_t i = 0; i < rowCount(); i++)
			_rows[i].zero();
	}

    /*
     * Function: setSubMatrix(startRowIndex, M)
     * Set a portion of a matrix starting from a given row as given the matrix
     */
	void setSubMatrix(size_t startRowIndex, const BitMatrix<COLS> & M){ 
		unsigned int newNumRows = M.rowCount();
		if(DEBUG) assert(startRowIndex >= 0 && startRowIndex + newNumRows < rowCount());
		for(size_t i = 0; i < newNumRows; ++i){
			_rows[i + startRowIndex] = M._rows[i];
		}
	}

    /*
     * Function: isIdentity()
     * Returns whether the current BitMatrix is an identity matrix
     */
	bool isIdentity() const {
		size_t numRows = rowCount();
		const size_t numCols = colCount();

		if (numRows != numCols) {
			return false;
		}

		for (size_t i = 0; i < numRows; i++) {
			for (size_t j = 0; j < numCols; j++) {
				if (_rows[i].get(j) && i != j)
					return false;
				else if (i == j && !_rows[i].get(j))
					return false;

			}
		}
		return true;
	}

    /*
     * Function: rowCount()
     * Returns the number of rows
     */
	const inline size_t rowCount() const {
		return _rows.size();
	}

    /*
     * Function: colCount()
     * Returns the number of columns
     */
	const inline size_t colCount() const {
		return COLS << 6;
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
		if(DEBUG) assert(rowCount() == colCount());
		return rref().getRightBottomCorner();
	} 

	/*
	 * Function: T()
	 * Returns the transpose of a matrix
	 * Usage: BitMatrix<K> Mt = M.template operator<K>(); //M is a matrix of dimension (K << 6) * (L * 6) for some K, L >= 0
	 */
	template<unsigned int ROWS>
	const BitMatrix<ROWS> T() const{
		if(DEBUG) assert(_rows.size() == ROWS << 6);
		BitMatrix<ROWS> Mt = BitMatrix<ROWS>::zeroMatrix(COLS << 6);
		int numRows = rowCount();
		int numCols = colCount();
		for(int i = 0; i < numRows; ++i){
			for(int j = 0; j < numCols; ++j){
				if(get(i, j)) Mt.set(j, i);
			}
		}
		return Mt;
	}

	/*
     * Function: inv()
     * Returns the inverse of a square matrix
     */
	const BitMatrix<COLS> inv() const{
		bool invertible;
		return inv(invertible);
	}

	/*
     * Function: inv(invertible)
     * Returns the inverse of a square matrix if invertible
     * Returns a square zero matrix and prints error otherwise
     * Marks invertibility in boolean
     */
	const BitMatrix<COLS> inv(bool & invertible) const{
		size_t n = rowCount();
		if(DEBUG) assert(n == colCount());
		BitMatrix<COLS> A = *this;
		BitMatrix<COLS> I = BitMatrix<COLS>::squareIdentityMatrix();
		for(int k = 0; k < n; ++k){
			int pos = -1, i = k;
			while(i < n){//find the first pos >= k when A[pos, k] == 1
				if(A.get(i, k)){pos = i; break;}
				++i;
			}
			if(pos != -1){
				if(pos != k) {
					A.swapRows(pos, k);
					I.swapRows(pos, k);
				}
				for(int i = k+1; i < n; ++i) if(A.get(i, k)){
					A.setRow(i, A[i] ^ A[k]);
					I.setRow(i, I[i] ^ I[k]);
				}
			} else {
				cerr << "Error: inversing a singular matrix!" << endl;
				invertible = false;
				return BitMatrix<COLS>::squareZeroMatrix(); 
			}
		}
		BitVector<COLS> x;
		BitMatrix<COLS> X = BitMatrix<COLS>::squareZeroMatrix();
		for(int j = 0; j < n; ++j){
			x.zero();
			for(int i = n-1; i >= 0; --i) x.set(i, x.dot(A[i]) ^ I.get(i,j));
			X.setCol(j, x);
		}
		invertible = true;
		return X;
	}

	/*
     * Function: rref()
     * Returns the reduced row-echelon form of a given matrix by Gaussian elimination
     */
	const BitMatrix<COLS> rref() const{
		size_t n = rowCount(), m = colCount();
		int l = 0;
		BitMatrix<COLS> A = *this;
		for(int k = 0; k < m && l < n; ++k){
			int pos = -1, i = l;
			while(i < n){
				if(A.get(i, k)){pos = i; break;}
				++i;
			}
			if(pos != -1){
				if(pos != l){
					A.swapRows(pos, l);
				}
				for(int i = l+1; i < n; ++i) if(A.get(i, k)){
					A.setRow(i, A[i] ^ A[k]);
				}
				++l;
			}
		}
		return A; 
	}

	/* Vector Operations */

	/*
     * Function: tMult(v)
     * Returns the vector result of multiplication by the transpose of the current matrix
     * Ex. A.tMult(v) := A^T * v in F_2^{m}
     */
	template<unsigned int ROWS>
	const BitVector<COLS> tMult(const BitVector<ROWS> & v) const {
		size_t numRows = _rows.size(); //n
 		if(DEBUG) assert(numRows == ROWS << 6);
		BitVector<COLS> result = BitVector<COLS>::zeroVector();
		for(int i = 0; i < numRows; ++i){
			if(v[i]) result ^= getRow(i);
		}
		return result;
	}

	/*
     * Function: solve(rhs)
     * Returns the vector result of multiplication by the inverse of the current matrix
     * Ex. A.solve(v) = A^{-1} v
     * Assumes current matrix is invertible
     */
	const BitVector<COLS> solve (const BitVector<COLS> & rhs) const{
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
	const BitVector<COLS> solve (const BitVector<COLS> & rhs, bool & solvable) const{ //// assert->ASSERT
		size_t m = rowCount();
		if(DEBUG) assert(m == colCount());
		BitMatrix<COLS> A = *this;
		BitVector<COLS> b = rhs;
		for(int k = 0; k < m; ++k){
			int pos = -1, i = k;
			while(i < m){ //find the first pos >= k with A[pos,k] == 1
				if(A.get(i, k)){pos = i; break;} 
				++i;
			}
			if(pos != -1){
				if(pos != k) {
					A.swapRows(pos, k);
					b.swap(pos, k);
				}
				for(int i = k+1; i < m; ++i) if(A.get(i, k)){
					A.setRow(i, A[i] ^ A[k]); 
					b.set(i, b[i]^b[k]);
				}
			} else {
				cerr << "Error: solving system of a singular matrix!" << endl;
				solvable = false;
				return BitVector<COLS>::zeroVector(); //this is when A is singular
			}
		}
		BitVector<COLS> x = BitVector<COLS>::zeroVector();
		for(int i = m-1; i >= 0; --i) x.set(i, x.dot(A[i]) ^ b[i]);
		solvable = true;
		return x;
	}

/* Multi-Matrix Functions */

	/*
     * Function: pMult(rhs, startCol, endCol, startRow, endRow)
     * Returns the result of multiplication of a given matrix
     * by a submatrix of the current matrix as specified by
     * given rows and columns
     */
	template<unsigned int NEWCOLS> 
	const BitMatrix<NEWCOLS> pMult(const BitMatrix<NEWCOLS> & rhs, 
		unsigned int startCol, unsigned int endCol, unsigned int startRow, unsigned int endRow) const{		
		if(DEBUG){
			const size_t numCols = colCount();
			const size_t rhsRows = rhs.rowCount();
			assert(startCol >= 0 && endCol < numCols);
			assert(endCol >= startCol);
			assert(startRow >= 0 && endRow < rhsRows);
			assert(endRow >= startRow);
			assert(startCol + endRow == startRow + endCol);			
		}
		size_t numRows = rowCount();
		BitMatrix<NEWCOLS> result(numRows);
		for(size_t j = startCol; j <= endCol; ++j){
			size_t rhsRow = startRow + (j - startCol);		
			for(size_t i = 0; i < numRows; ++i){
				if(get(i, j)){
					result.setRow(i, result.getRow(i) ^ rhs.getRow(rhsRow));
				}
			}
		}
		return result;
	}

	template<unsigned int NEWCOLS>
	const BitMatrix<NEWCOLS> pMult(const BitMatrix<NEWCOLS> & rhs,
		unsigned int startRow, unsigned int endRow) const{
		return pMult<NEWCOLS>(rhs, 0, _colCount-1, startRow, endRow);
	}

	/*
     * Function: pMult(v, startCol, endCol, startRow, endRow)
     * Returns the result of multiplication of a given vector
     * by a submatrix of the current matrix as specified by
     * given rows and columns
     */
	template <unsigned int NEWCOLS>
	const BitMatrix<NEWCOLS> pMult(const BitVector<COLS> & v, unsigned int startCol,
		unsigned int endCol, unsigned int startIndex, unsigned int endIndex) const{
		const size_t numCols = colCount();
		const size_t rhsLength = v.length();
		if(DEBUG){
			assert(startCol >= 0 && endCol < numCols);
			assert(endCol >= startCol);
			assert(startIndex >= 0 && endIndex < rhsLength);
			assert(endIndex >= startIndex);
			assert(startCol + endIndex == startIndex + endCol);
		}
		size_t numRows = rowCount();
		BitVector<NEWCOLS> result;
		for(size_t j = startCol; j <= endCol; ++j){
			size_t rhsIndex = startIndex + (j-startCol);
			for(size_t i = 0; i < numRows; ++i){
				if(get(i, j)){
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
	static const BitMatrix<COLS1 + COLS2> augH (const BitMatrix<COLS1> & lhs, const BitMatrix<COLS2> & rhs){
		unsigned int l_rows = lhs.rowCount();
		unsigned int r_rows = rhs.rowCount();
		if(DEBUG) assert(l_rows == r_rows); //same height 
		//if(COLS1 == 0) return rhs; (why did this give dim disagreement?!?!)
		//if(COLS2 == 0) return lhs; (why did this give dim disagreement?!?!)
		vector< BitVector<COLS> > rows(l_rows);		
		for(size_t i = 0; i < l_rows; ++i){
			BitVector<COLS1> lv = lhs.getRow(i);
			BitVector<COLS2> rv = rhs.getRow(i);
			rows[i] = BitVector<COLS>::vCat(lv, rv);
		}
		return BitMatrix<COLS>(rows);
	}

	/*
     * Function: augH(lhs, mid, rhs)
     * Returns the matrix resulting from horizontal augmentation
     * of three given matrices
     * Assumes the given matrixes have the same number of rows
     */
	template <unsigned int COLS1, unsigned int COLS2, unsigned int COLS3> //TODO: optimize
	static const BitMatrix<COLS1 + COLS2 + COLS3> augH(const BitMatrix<COLS1> & lhs, const BitMatrix<COLS2> & mid, const BitMatrix<COLS3> & rhs){
		unsigned int l_rows = lhs.rowCount();
		if(DEBUG) { //same height 
			assert(l_rows == mid.rowCount());
			assert(l_rows == rhs.rowCount()); 
		}
		vector< BitVector<COLS> > rows(l_rows);		
		for(size_t i = 0; i < l_rows; ++i){
			BitVector<COLS1> lv = lhs.getRow(i);
			BitVector<COLS2> mv = mid.getRow(i);
			BitVector<COLS3> rv = rhs.getRow(i);
			rows[i] = BitVector<COLS>::vCat(lv, mv, rv);
		}
		return BitMatrix<COLS>(rows);
	}

	/*
     * Function: augV(top, bot)
     * Returns the matrix resulting from vertical augmentation
     * of two given matrices
     * Assumes the given matrixes have the same number of rows
     */
	static const BitMatrix<COLS> augV(const BitMatrix<COLS> & top, const BitMatrix<COLS> & bot){ //TODO: optimize
		unsigned int t_rows = top.rowCount();
		unsigned int b_rows = bot.rowCount();
		vector< BitVector<COLS> > rows;
		rows.reserve(t_rows + b_rows);
		rows.insert(rows.end(), top._rows.begin(), top._rows.end());
		rows.insert(rows.end(), bot._rows.begin(), bot._rows.end());
		return BitMatrix<COLS>(rows);
	}

	/*
     * Function: augV(top, mid, bot)
     * Returns the matrix resulting from vertical augmentation
     * of two given matrices
     * Assumes the given matrixes have the same number of rows
     */
	static const BitMatrix<COLS> augV(const BitMatrix<COLS> & top, const BitMatrix<COLS> & mid, const BitMatrix<COLS> & bot){ //TODO: optimize
		unsigned int t_rows = top.rowCount();
		unsigned int m_rows = mid.rowCount();
		unsigned int b_rows = bot.rowCount();
		vector< BitVector<COLS> > rows;
		rows.reserve(t_rows + m_rows + b_rows);
		rows.insert(rows.end(), top._rows.begin(), top._rows.end());
		rows.insert(rows.end(), mid._rows.begin(), mid._rows.end());
		rows.insert(rows.end(), bot._rows.begin(), bot._rows.end());
		return BitMatrix<COLS>(rows);
	}

	/*
     * Function: splitH2(index)
     * Returns the left or right half of the current BitMatrix
     * Assumes the the number of columns is divisible by 2
     */
	//TODO: unify 2 functions below if needed
	const BitMatrix<COLS/2> splitH2(int index) const{
		if(DEBUG) assert(index >= 0 && index < 2); //index not OB
		const int SUBCOLS = COLS / 2;
		int numRows = rowCount();
		BitMatrix<SUBCOLS> result = BitMatrix<SUBCOLS>::zeroMatrix(numRows);//squareZeroMatrix();
		for(int i = 0; i < numRows; ++i){
			BitVector<SUBCOLS> sv = getRow(i).proj2(index);
			result.setRow(i, sv);
		}
		return result;
	}

	/*
     * Function: splitH3(index)
     * Returns a specified horizontal third of the current BitMatrix
     * Assumes the the number of columns is divisible by 3
     */
	const BitMatrix<COLS/3> splitH3(int index) const{
		if(DEBUG) assert(index >= 0 && index < 3); //index not OB
		const int SUBCOLS = COLS / 3;
		int numRows = rowCount();
		BitMatrix<SUBCOLS> result = BitMatrix<SUBCOLS>::zeroMatrix(numRows);//squareZeroMatrix();
		for(int i = 0; i < numRows; ++i){
			BitVector<SUBCOLS> sv = getRow(i).proj3(index);
			result.setRow(i, sv);
		}
		return result;
	}

	/*
     * Function: splitV2(index)
     * Returns the top or bottom half of the current BitMatrix
     * Assumes the the number of rows is divisible by 2
     */
	const BitMatrix<COLS> splitV2 (int index) const{
		if(DEBUG) assert(index >= 0 && index < 2);
		const unsigned int sub_rowCount = _rows.size() / 2;
		unsigned int start = index * sub_rowCount;
		vector<BitVector<COLS> > subrows(&_rows[start], &_rows[start + sub_rowCount]);
		return BitMatrix<COLS>(subrows);	
	}

	/*
     * Function: splitV3(index)
     * Returns a specified vertical third of the current BitMatrix
     * Assumes the the number of rows is divisible by 3
     */
	const BitMatrix<COLS> splitV3 (int index) const{
		if(DEBUG) assert(index >= 0 && index < 3);
		const unsigned int sub_rowCount = _rows.size() / 3;
		unsigned int start = index * sub_rowCount;
		vector<BitVector<COLS> > subrows(&_rows[start], &_rows[start + sub_rowCount]);
		return BitMatrix<COLS>(subrows);		
	}

	/*
     * Function: projMatrix(start, end)
     * Return a projection matrix for the coordinates between start and end inclusive
     * The projection matrix has 1s in the diagonal between the given indices inclusive
     */
	const static BitMatrix<COLS> projMatrix (int start, int end) {
		if(DEBUG) assert(start >= 0 && start < COLS && end >= 0 && end < COLS && start <= end);
		BitMatrix<COLS> result = BitMatrix<COLS>::squareZeroMatrix();
		
		for (int i = start; i <= end; ++i) {
			result.set(i, i);
		}
		return result;
	}

	/* Print */

    /*
     * Function: printRow()
     * Prints the values of a given row
     */
	void printRow(int rowIndex) const { 
		if(DEBUG) assert(rowIndex >= 0 && rowIndex < rowCount());
		const int m = colCount();
		cout << get(rowIndex, 0);
		for(int i = 1; i < m; ++i){
			cout << " " << get(rowIndex, i);
		}
		cout << endl;
	}

    /*
     * Function: printLastRow()
     * Prints the values of the last row (for debugging)
     */
	void printLastRow() const{
		int last = rowCount() -1;
		_rows[last].print();
	}

    /*
     * Function: printCol()
     * Prints the values of a given column
     */
	void printCol(int colIndex) const { 
		if(DEBUG) assert(colIndex >= 0 && colIndex < colCount());
		const int n = rowCount();
		cout << get(0, colIndex);
		for(int i = 1; i < n; ++i){
			cout << " " << get(i, colIndex);
		}
		cout << endl;
	}	

    /*
     * Function: print()
     * Prints the values of the matrix row-by-row
     */
	void print() const {
		const int n = rowCount();
		for(int i = 0; i < n; ++i){
			printRow(i);
		}
	}

    /*
     * Function: writeRow()
     * Writes the values of the matrix into a file
     */
	void writeMatrix(const string filename) const{
		int n = rowCount(), m = colCount();
		ofstream ofs;
		ofs.open(filename);
		for(int i = 0; i < n; ++i){
			ofs << get(i, 0);
			for(int j = 1; j < m; ++j){
				ofs << " " << get(i, j);
			}
			ofs << endl;
		}
		ofs.close();
	}

private:
	vector< BitVector<COLS> > _rows;
	static const unsigned int _colCount = COLS << 6;

    /*
     * Function: getRightBottomCorner()
     * Returns the value of the bottom right corner entry
     */
	bool getRightBottomCorner() const{
		return get(rowCount()-1, colCount()-1);
	}

/* Elementary row operations */

	/*
     * Function: addRow(dstIndex, srcIndex)
     * Bitwise adds (XORs) the source row to the destination row
     */
	void addRow(int dstIndex, int srcIndex){ 
		if(DEBUG){
			assert(dstIndex >= 0 && dstIndex < rowCount());
			assert(srcIndex >= 0 && srcIndex < rowCount());
		}
		_rows[dstIndex] ^= _rows[srcIndex];
	}

	/*
     * Function: swapRow(firstIndex, secondIndex)
     * Swaps two specified rows
     */
	void swapRows(int firstIndex, int secondIndex){ 
		if(DEBUG){
			assert(firstIndex >= 0 && firstIndex < rowCount());
			assert(secondIndex >= 0 && secondIndex < rowCount());
		}
		BitVector<COLS> tmp = _rows[firstIndex];
		_rows[firstIndex] = _rows[secondIndex];
		_rows[secondIndex] = tmp;
	}	


	/*
     * Function: rrefFastGauss()
     * Returns the reduced row-echelon form of a given matrix by Gaussian elimination
     * Uses the method from: https://www.cs.umd.edu/~gasarch/TOPICS/factoring/fastgauss.pdf
     * Assumes the current matrix is square
     */
	BitMatrix<COLS> rrefFastGauss() const{
		BitMatrix<COLS> A = *this;
		size_t n = rowCount();
		if(DEBUG) assert(colCount() == n);
		for(int j = 0; j < n; ++j){
			int i = -1;
			while(i < n){
				if(A.get(i,j)) break;
				++i;
			}
			if(i != -1){
				for(int k = 0; k < n; ++k){
					if(k != j){
						A.set(i, k);
						A.addRow(k, j);
					}
				}
			}
		}
		return A;
	} 
};

#endif
