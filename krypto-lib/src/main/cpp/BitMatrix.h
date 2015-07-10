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
#include "../../../contrib/gtest/gtest.h"

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
	bool det() const{ //assert->ASSERT
		//ASSERT(rowCount() == colCount(), "determinant not applicable to non-square matrix!");				
		assert(rowCount() == colCount());
		return rref().getRightBottomCorner();
	} 

	bool get(int rowIndex, int colIndex) const{ //assert->ASSERT
		assert(rowIndex >= 0 && rowIndex < rowCount()); //"rowIndex out of bound!"
		assert(colIndex >= 0 && colIndex < colCount()); //"colIndex out of bound!"
		return _rows[rowIndex].get(colIndex);
	}

	void set(int rowIndex, int colIndex){ //assert->ASSERT
		assert(rowIndex >= 0 && rowIndex < rowCount()); //"rowIndex out of bound!"
		assert(colIndex >= 0 && colIndex < colCount());	//"colIndex out of bound!"
		_rows[rowIndex].set(colIndex);	
	}

	void clear(int rowIndex, int colIndex){ //assert->ASSERT
		assert(rowIndex >= 0 && rowIndex < rowCount()); //"rowIndex out of bound!"
		assert(colIndex >= 0 && colIndex < colCount());	//"colIndex out of bound!"
		_rows[rowIndex].clear(colIndex);		
	}

	BitVector<COLS> & operator[](const int rowIndex) { //assert->ASSERT
		assert(rowIndex >= 0 && rowIndex < rowCount()); //"rowIndex out of bound!"
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

		//ASSERT(numCols == rhs.rowCount(), "Matrix dimension mismatch!");
		assert(numCols == rhs.rowCount());
		BitMatrix<COLS> result(numRows);

		for (size_t row = 0; row < numRows; row++) {
			for (size_t col = 0; col < numCols; ++col) {
				if(get(row, col)){
				//if (_rows[row].getBit(col)) {
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

	//Input: A; Output: A^-1, here, A must be invertible
	//By Gaussian elimination
	//assume square matrix for now, generalize later
	BitMatrix<COLS> inv() const{ //assert->ASSERT
		size_t n = rowCount();
		//ASSERT(n == colCount(), "Matrix dimension mismatch!");
		assert(n == colCount());
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
				cerr << "Error: inversing a nonsingular matrix!" << endl;
				return BitMatrix<COLS>::squareZeroMatrix(); //this is when A is singular, and really shouldn't happen 
			}
		}
		BitVector<COLS> x = BitVector<COLS>::zeroVector();
		BitMatrix<COLS> X = BitMatrix<COLS>::squareZeroMatrix();
		for(int j = 0; j < n; ++j){
			x.zero();
			for(int i = n-1; i >= 0; --i){
				bool b = x.dot(A[i]) ^ I.get(i,j);
				b ? x.set(i) : x.clear(i);
			}
			X.setCol(j, x);
		}
		return X;
	}

	//Finding the inverse of A if possible, and if not, reflect that in the invertible variable
	BitMatrix<COLS> inv(bool & invertible) const{ //assert->ASSERT
		size_t n = rowCount();
		//ASSERT(n == colCount(), "Matrix dimension mismatch!");
		assert(n == colCount());
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
				cerr << "Error: inversing a nonsingular matrix!" << endl;
				invertible = false;
				return BitMatrix<COLS>::squareZeroMatrix(); 
			}
		}
		BitVector<COLS> x = BitVector<COLS>::zeroVector();
		BitMatrix<COLS> X = BitMatrix<COLS>::squareZeroMatrix();
		for(int j = 0; j < n; ++j){
			x.zero();
			for(int i = n-1; i >= 0; --i){
				bool b = x.dot(A[i]) ^ I.get(i,j);
				b ? x.set(i) : x.clear(i);
			}
			X.setCol(j, x);
		}
		invertible = true;
		return X;
	}

	/**
	 * A should be invertible in this case. This can be ensured by inializing an inverible matrix.
	 * Input: v; Output: A^-1*v;
	 * Usage: x = A.solve(v, solvable); means Ax = v
	 */
	BitVector<COLS> solve (const BitVector<COLS> & rhs) const{ //assert->ASSERT
		size_t n = rowCount();
		//ASSERT(n == colCount(), "Matrix dimension mismatch!");
		assert(n == colCount());
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
					A.swapRows(pos, k);
					b.swap(pos, k);
				}
				for(int i = k+1; i < n; ++i) if(A.get(i, k)){
					A.setRow(i, A[i] ^ A[k]); 
					(b[i]^b[k]) ? b.set(i) : b.clear(i);
				}
			} else {
				cerr << "Error: solving system of a nonsingular matrix!" << endl;
				return BitVector<COLS>::zeroVector(); //this is when A is singular
			}
		}
		BitVector<COLS> x = BitVector<COLS>::zeroVector();
		for(int i = n-1; i >= 0; --i){
			bool f = x.dot(A[i]) ^ b[i];
			f ? x.set(i) : x.clear(i);
		}
		return x;
	}

	/**
	 * Input: v; Output: A^-1*v;
	 * Usage: x = A.solve(v, solvable); means Ax = v
	 */
	BitVector<COLS> solve (const BitVector<COLS> & rhs, bool & solvable) const{ //assert->ASSERT
		size_t n = rowCount();
		//ASSERT(n == colCount(), "Matrix dimension mismatch!");
		assert(n == colCount());
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
					A.swapRows(pos, k);
					b.swap(pos, k);
				}
				for(int i = k+1; i < n; ++i) if(A.get(i, k)){
					A.setRow(i, A[i] ^ A[k]); 
					(b[i]^b[k]) ? b.set(i) : b.clear(i);
				}
			} else {
				cerr << "Error: solving system of a nonsingular matrix!" << endl;
				solvable = false;
				return BitVector<COLS>::zeroVector(); //this is when A is singular
			}
		}
		BitVector<COLS> x = BitVector<COLS>::zeroVector();
		for(int i = n-1; i >= 0; --i){
			bool f = x.dot(A[i]) ^ b[i];
			f ? x.set(i) : x.clear(i);
		}
		solvable = true;
		return x;
	}

	/* Functions below will be shifted to the private section after tested */

	/***Elementary row operations***/

	void addRow(int dstIndex, int srcIndex){ //assert->ASSERT
		//ASSERT(dstIndex >= 0 && dstIndex < rowCount(), "dstIndex out of bound!");
		//ASSERT(srcIndex >= 0 && srcIndex < rowCount(), "srcIndex out of bound!");
		assert(dstIndex >= 0 && dstIndex < rowCount());
		assert(srcIndex >= 0 && srcIndex < rowCount());
		_rows[dstIndex] ^= _rows[srcIndex];
	}

	void swapRows(int firstIndex, int secondIndex){ //assert->ASSERT
		//ASSERT(firstIndex >= 0 && firstIndex < rowCount(), "firstIndex out of bound!");
		//ASSERT(secondIndex >= 0 && secondIndex < rowCount(), "secondIndex out of bound!");
		assert(firstIndex >= 0 && firstIndex < rowCount());
		assert(secondIndex >= 0 && secondIndex < rowCount());
		BitVector<COLS> tmp = _rows[firstIndex];
		_rows[firstIndex] = _rows[secondIndex];
		_rows[secondIndex] = tmp;
	}	

	/***Acecss/Modify individual cols/rows***/

	void setRow(int rowIndex, BitVector<COLS> v){ //assert->ASSERT
		//ASSERT(rowIndex >= 0 && rowIndex < rowCount(), "rowIndex out of bound!");
		assert(rowIndex >= 0 && rowIndex < rowCount());
		_rows[rowIndex] = v;
	}

	//TODO: enable COLS to be a variable, for now, it is just COLS to be able to executed by the compiler
	void setCol(int colIndex, BitVector<COLS> v){ //assert->ASSERT
		//ASSERT(colIndex >= 0 && colIndex < colCount(), "colIndex out of bound!");
		int numRows = COLS << 6;//rowCount();
		for(int i = 0; i < numRows; ++i){
			v[i] ? set(i, colIndex) : clear(i, colIndex);
		}
	}

	/***File/terminal input/output***/

	void printRow(int rowIndex) const { //assert->ASSERT
		const int n = rowCount();
		//ASSERT(rowIndex >= 0 && rowIndex < n, "rowIndex out of bound!");
		assert(rowIndex >= 0 && rowIndex < n);
		const int m = colCount();
		cout << get(rowIndex, 0);
		for(int i = 1; i < m; ++i){
			cout << " " << get(rowIndex, i);
		}
		cout << endl;
	}

	void printCol(int colIndex) const { //assert->ASSERT
		const int m = colCount();
		//ASSERT(colIndex >= 0 && colIndex < m, "colIndex out of bound!");
		assert(colIndex >= 0 && colIndex < m);
		const int n = rowCount();
		cout << get(0, colIndex);
		for(int i = 1; i < n; ++i){
			cout << " " << get(i, colIndex);
		}
		cout << endl;
	}	

	//write matrix into a file to check against magma results
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
	vector<BitVector<COLS>> _rows;
	//static const _rowCount = _rows.size(); (check if it is possible to do so)
	//static const _colCount = COLS << 6; (check if it is possible to do so)
	bool getRightBottomCorner(){
		return get(rowCount()-1, colCount()-1);
	}

	//adapted from: https://www.cs.umd.edu/~gasarch/TOPICS/factoring/fastgauss.pdf
	//assume square matrix for now, generalize later
	BitMatrix<COLS> rrefFastGauss() const{ //assert->ASSERT
		BitMatrix<COLS> A = *this;
		size_t n = rowCount();
		//ASSERT(colCount() == n, "Matrix dimension mismatch!");
		assert(colCount() == n);
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
