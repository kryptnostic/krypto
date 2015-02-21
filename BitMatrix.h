//
//  BitMatrix.h
//  krypto
//
//  Created by Matthew Tamayo on 1/27/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//

#ifndef krypto_BitMatrix_h
#define krypto_BitMatrix_h

#include<vector>
#include "BitVector.h"

template<unsigned int COLS>
class BitMatrix {
public:
    BitMatrix(int numRows): _rows(numRows){}
    BitMatrix(const vector<BitVector<COLS>> & rows): _rows(rows){}
    
    const BitVector<COLS> operator*(const BitVector<COLS> & v) const {
        BitVector<COLS> result;
        size_t numRows = _rows.size();
        for(size_t i = 0; i < numRows; ++i) {
            BitVector<COLS> prod = _rows[i] & v;
            if( prod.parity() ) {
                result.set(i);
            }
        }
        
        return result;
    }
    
    const BitMatrix<COLS> operator*(const BitMatrix<COLS> & rhs) const
    {
        size_t  numRows = rowCount();
        const size_t numCols = colCount();
        
        ASSERT(numCols==rhs.rowCount(), "Matrix dimension mismatch!");
        
        BitMatrix<COLS> result(numRows);
        
        for(size_t row = 0; row<numRows; row++)
        {
            for(size_t col = 0; col < numCols; ++col) {
                if( _rows[row].getBit(col) ) {
                    result.rows[row] ^= rhs.rows[col];
                }
            }
        }
        
        return result;
    }
    
    bool isIdentity() const
    {
        size_t  numRows = rowCount();
        const size_t numCols = colCount();
        
        if(numRows!=numCols)
        {
            return false;
        }
        
        for(size_t i = 0; i < numRows; i++)
        {
            for(size_t j = 0; j < numCols; j++)
            {
                if( _rows[i].get(j) && i!=j )
                    return false;
                else if( i==j && !_rows[i].get(j) )
                    return false;
                
            }
        }
        return true;
    }
    
    void zero()
    {
        for(size_t i = 0; i < rowCount(); i++)
            _rows[i].zero();
    }
    
    const size_t rowCount() const {
        return _rows.size();
    }
    
    const size_t colCount() const {
        return COLS << 6;
    }
    
private:
    vector<BitVector<COLS>> _rows;
};

#endif
