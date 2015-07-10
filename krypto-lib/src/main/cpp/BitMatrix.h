//
//  BitMatrix.h
//  krypto
//
//  Created by Matthew Tamayo on 1/27/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//

#ifndef krypto_BitMatrix_h
#define krypto_BitMatrix_h

#include <vector>
#include <fstream>
#include "../../main/cpp/BitVector.h"

using namespace std;

template<unsigned int COLS>
class BitMatrix {
//friend class BitMatrixTest;
public:
	BitMatrix(int numRows) :
			_rows(numRows) {
	}

	BitMatrix(const vector<BitVector<COLS>> & rows) :
			_rows(rows) {
	}

	static const BitMatrix squareZeroMatrix(){
		int numRows = COLS << 6;
		vector<BitVector<COLS>> rows(numRows);
		for(int i = 0; i < numRows; ++i){
			BitVector<COLS> v = BitVector<COLS>::zeroVector();
		}
		return BitMatrix(rows);
	}

	static const BitMatrix zeroMatrix(const int numRows){
		vector<BitVector<COLS>> rows(numRows);
		for(int i = 0; i < numRows; ++i){
			BitVector<COLS> v = BitVector<COLS>::zeroVector();
		}
		return BitMatrix(rows);
	}

	static const BitMatrix randomMatrix(const int numRows) {
		vector<BitVector<COLS>> rows(numRows);
		for (int i = 0; i < numRows; ++i) {
			rows[i] = BitVector<COLS>::randomVector();
		}
		return BitMatrix(rows);
	}

	static const BitMatrix randomInvertibleMatrix(const int numRows) {
		BitMatrix<COLS> R = BitMatrix<COLS>::randomMatrix(numRows);
		while(!R.det()){
			R = BitMatrix<COLS>::randomMatrix(numRows);
		}
		return R;		
	}

	static const BitMatrix squareIdentityMatrix(){
		int numRows = COLS << 6;
		vector<BitVector<COLS>> rows(numRows);
		for(int i = 0; i < numRows; ++i){
			BitVector<COLS> e = BitVector<COLS>::zeroVector();
			e.set(i);
			rows[i] = e;
		}
		return BitMatrix(rows);
	}

	/**
	 * Returns the determinant of a given (square) matrix, which is 1 iff the right bottom corner of its rref is 1. 
	 * Determinant of a non-square matrix possible? http://math.stackexchange.com/questions/854180/determinant-of-a-non-square-matrix
	 * Read into this later: http://journals.cambridge.org/download.php?file=%2FBAZ%2FBAZ21_01%2FS0004972700011369a.pdf&code=1807973f2c6d49bc4579326df0a7aa58
	 */
	bool det() const{
		ASSERT(rowCount() == colCount(), "Matrix dimension mismatch!");				
		return rref().getRightBottomCorner();
	} 

	bool get(int rowIndex, int colIndex) const{
		ASSERT(rowIndex >= 0 && rowIndex < rowCount(), "rowIndex out of bound!");
		ASSERT(colIndex >= 0 && colIndex < colCount(), "colIndex out of bound!");
		return _rows[rowIndex].get(colIndex);
	}

	void set(int rowIndex, int colIndex){
		assert(rowIndex >= 0 && rowIndex < rowCount());
		assert(colIndex >= 0 && colIndex < colCount());	
		_rows[rowIndex].set(colIndex);	
	}

	void clear(int rowIndex, int colIndex){
		assert(rowIndex >= 0 && rowIndex < rowCount());
		assert(colIndex >= 0 && colIndex < colCount());	
		_rows[rowIndex].clear(colIndex);		
	}

/*
	bool & operator==(const BitMatrix<COLS> & rhs){
		int numRows = rowCount();
		for(int i = 0; i < numRows; ++i){
			if(_rows[i] != rhs[i]) return false;
		}
		return true;
	}
*/

	BitVector<COLS> & operator[](const int rowIndex) {
		return _rows[rowIndex];
	}

	const BitVector<COLS> operator*(const BitVector<COLS> & v) {
		//const BitVector<COLS> operator*(const BitVector<COLS> & v) const
		BitVector<COLS> result;
		size_t numRows = _rows.size();
		for (size_t i = 0; i < numRows; ++i) {
			BitVector<COLS> prod = _rows[i] & v;
			if (prod.parity()) {
				result.set(i);
			}
		}

		return result;
	}

	const BitMatrix<COLS> operator*(const BitMatrix<COLS> & rhs) const {
		size_t numRows = rowCount();
		const size_t numCols = colCount();

		ASSERT(numCols == rhs.rowCount(), "Matrix dimension mismatch!");

		BitMatrix<COLS> result(numRows);

		for (size_t row = 0; row < numRows; row++) {
			for (size_t col = 0; col < numCols; ++col) {
				if (_rows[row].getBit(col)) {
					result._rows[row] ^= rhs._rows[col];
				}
			}
		}

		return result;
	}

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

	void zero() {
		for (size_t i = 0; i < rowCount(); i++)
			_rows[i].zero();
	}

	const size_t rowCount() const {
		return _rows.size();
	}

	const size_t colCount() const {
		return COLS << 6;
	}   

	//for generalized matrix
	BitMatrix<COLS> rref() const{
		size_t n = rowCount(), m = colCount();
		int l = 0;
		BitMatrix<COLS> A = *this;
		for(int k = 0; k < m && l < n; ++k){
			int pos = -1, i = 1;
			while(i < n){
				if(A.get(i, k)){pos = i; break;}
				++i;
			}
			if(pos != -1){
				if(pos != l){
					A.swapRow(pos, l);
					b.swap(pos, l);					
				}
				for(int i = l+1; i < n; ++i) if(A.get(i, k)){
					A.setRow(i, A[i] ^ A[k]);
				}
				++l;
			}
		}
		return A; //TODO
	}

	//Input: A; Output: A^-1 or NULL if A is singular
	//By Gaussian elimination
	//assume square matrix for now, generalize later
	BitMatrix<COLS> inv() const{
		size_t n = rowCount();
		ASSERT(n != colCount(), "Matrix dimension mismatch!");
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
					A.swapRow(pos, k);
					b.swap(pos, k);
				}
				for(int i = k+1; i < n; ++i) if(A.get(i, k)){
					A.setRow(i, A[i] ^ A[k]);
					I.setRow(i, I[i] ^ I[k]);
				}
			} else return NULL; //this is when A is singular 
		}
		BitVector<COLS> x = BitVector<COLS>::zeroVector();
		BitMatrix<COLS> X = BitMatrix<COLS>::squareZeroMatrix();
		for(int j = 0; j < n; ++j){
			for(int i = n-1; n >= 0; --n){
				x[i] = x.dot(A[i]) ^ I.get(i,j);
			}
			X.setCol(j, x);
		}
		return X;
	}

	/**
	 * Input: v; Output: A^-1*v;
	 * Usage: x = A.solve(v); means Ax = v
	 */
	BitVector<COLS> solve (const BitVector<COLS> & rhs) const{
		size_t n = rowCount();
		ASSERT(n != colCount(), "Matrix dimension mismatch!");
		BitMatrix<COLS> A = *this;
		BitVector<COLS> b = rhs;
		for(int k = 0; k < n; ++k){
			int pos = -1, i = k;
			while(i < n){ //find the first pos >= k with A[pos,k] == 1
				if(A.get(i, k)){pos = i; break;} 
				++i;
			}
			if(pos != -1){
				if(pos != k) {
					A.swapRow(pos, k);
					b.swap(pos, k);
				}
				for(int i = k+1; i < n; ++i) if(A.get(i, k)){
					A.setRow(i, A[i] ^ A[k]); 
					(b[i]^b[k]) ? b.set(i) : b.clear(i);
				}
			} else return NULL; //this is when A is singular
		}
		BitVector<COLS> x = BitVector<COLS>::zeroVector();
		for(int i = n-1; i >= 0; --i){
			x[i] = x.dot(A[i]) + b[i]; //TODO: implement dot
		}
		return x;
	}

	//BitVector<rowCount()> T() const{return the transpose}

	/* Functions below will be shifted to the private section after tested */

	/***Elementary row operations***/

	void addRow(int dstIndex, int srcIndex){
		ASSERT(dstIndex >= 0 && dstIndex < rowCount(), "dstIndex out of bound!");
		ASSERT(srcIndex >= 0 && srcIndex < rowCount(), "srcIndex out of bound!");
		_rows[dstIndex] ^= _rows[srcIndex];
	}

	void swapRow(int firstIndex, int secondIndex){
		ASSERT(firstIndex >= 0 && firstIndex < rowCount(), "firstIndex out of bound!");
		ASSERT(secondIndex >= 0 && secondIndex < rowCount(), "secondIndex out of bound!");
		BitVector<COLS> tmp = _rows[firstIndex];
		_rows[firstIndex] = _rows[secondIndex];
		_rows[secondIndex] = tmp;
	}	

	/***Acecss/Modify individual cols/rows***/

	void setRow(int rowIndex, BitVector<COLS> v){
		ASSERT(rowIndex >= 0 && rowIndex < rowCount(), "rowIndex out of bound!");
		_rows[rowIndex] = v;
	}

	void setCol(int colIndex, BitVector<rowCount()> v){ //TODO: check the usage of rowCount() here
		ASSERT(colIndex >= 0 && colIndex < colCount(), "colIndex out of bound!");
		int numRows = rowCount();
		for(int i = 0; i < numRows; ++i){
			v[i] ? v.set(i, colIndex) : v.clear(i, colIndex);
		}
	}

	/***File/terminal input/output***/

	void printRow(int rowIndex) const {
		const int n = rowCount();
		ASSERT(rowIndex >= 0 && rowIndex < n, "rowIndex out of bound!");
		//TODO
	}

	void printCol(int colIndex) const {
		const int m = colCount();
		ASSERT(colIndex >= 0 && colIndex < m, "colIndex out of bound!");
		//TODO
	}	

	//write matrix into a file for checking against magma results
	void writeMatrix(const string filename) const{
		int n = rowCount(), m = colCount();
		ofstream ofs;
		ofs.open(filename);
		for(int i = 0; i < n; ++i){
			ofs << get(i, 0);
			for(int j = 1; j < m; ++j){
				ofs << get(i, j) << " ";
			}
			ofs << endl;
		}
		ofs.close();
	}

private:
	vector<BitVector<COLS>> _rows;
	//static const _rowCount = _rows.size(); (check if it is possible to do so)
	//static const _colCount = COLS << 6; (check if it is possible to do so)
	bool getRightBottomCorner(){
		return get(rowCount()-1, colCount()-1);
	}

	//adapted from: https://www.cs.umd.edu/~gasarch/TOPICS/factoring/fastgauss.pdf
	//assume square matrix for now, generalize later
	BitMatrix<COLS> rrefFastGauss() const{
		BitMatrix<COLS> A = *this;
		size_t n = rowCount();
		ASSERT(colCount() == n, "Matrix dimension mismatch!");
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
