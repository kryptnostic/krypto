//
//  BitVector.h
//  krypto
//
//  Created by Matthew Tamayo on 12/11/14.
//  Copyright (c) 2014 Kryptnostic. All rights reserved.
//
//  Implementation of binary vector using an array of longs
//

#ifndef krypto_BitVector_h
#define krypto_BitVector_h
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <assert.h>

using namespace std;

//TODO: Wrap this in a class that can release the file handle and automatically select a good source of randomness on Windows.
//file pointer urandom must be closed by any class importing BitVector
static FILE * urandom = std::fopen("/dev/urandom", "rb" );

/*
 * Template for BitVector
 * Bit values are stored in an array of N many 64-bit longs
 * in the array used to store the bits
 */
template<unsigned int N>
class BitVector {
public:

/* Constructors */

    /*
     * Constructor
     * Constructs a zero-initialized BitVector
     */
    BitVector() {
        zero();
    }
    
    /*
     * Copy Constructor
     * Constructs a copy of an input BitVector
     */
    BitVector( const BitVector<N> & v ) {
        for (unsigned int i = 0; i < N; ++i) {
            _bits[i] = v._bits[i];
        }
    }
    
    /*
     * Move Constructor
     * Constructs a copy of an input BitVector
     */
    BitVector( BitVector<N> && v ) {
        for (unsigned int i = 0; i < N; ++i) {
            _bits[i] = v._bits[i];
        }
    }

/* Generation */

    /*
     * Function: zeroVector()
     * Returns a zero-initialized BitVector
     */
    static const BitVector<N> & zeroVector() {
        static BitVector<N> v;
        return v;
    }

    /*
     * Function: randomVector()
     * Returns a BitVector with random values
     */
    static const BitVector<N> randomVector() {
        BitVector<N> result;
        
        while( result.isZero() ) {
            std::fread(&result._bits, sizeof( unsigned long long ), N, urandom );
        }
        return result;
    }

    /*
     * Function: randomVectorLeadingZeroes(n)
     * Returns a BitVector with n leading zeroes followed by random values
     * Assumes n < numBits
     */
    static const BitVector<N> randomVectorLeadingZeroes(unsigned int n) {
        BitVector<N> result;
        
        while( result.isZero() ) {
            std::fread(&result._bits, sizeof( unsigned long long ), N, urandom );
        }

        for (int i = 0; i < n; ++i) {
            result.clear(i);
        }
        return result;
    }

/* Operators */

    /*
     * Operator: []
     * Returns the value of the bit at a given index
     */
    bool operator[](unsigned int n) const{
        return get(n);
    }

    /*
     * Operator: &
     * Returns a BitVector with values resulting from bitwise AND
     */
    BitVector<N> operator&(const BitVector<N> & rhs) const {
        BitVector<N> result;
        for (unsigned int i = 0; i < N; ++i) {
            result._bits[i] = _bits[i] & rhs._bits[i];
        }
        return result;
    }

    /*
     * Operator: |
     * Returns a BitVector with values resulting from bitwise OR
     */
    BitVector<N> operator|(const BitVector<N> & rhs) {
        BitVector<N> result;
        for (unsigned int i = 0; i < N; ++i) {
            result._bits[i] = _bits[i] | rhs._bits[i];
        }
        return result;
    }

    /*
     * Operator: ^
     * Returns a BitVector with values resulting from bitwise XOR
     */
    BitVector<N> operator^(const BitVector<N> & rhs) const {
        BitVector<N> result;
        for (unsigned int i = 0; i < N; ++i) {
            result._bits[i] = _bits[i] ^ rhs._bits[i];
        }
        return result;
    }

    /*
     * Operator: =
     * Sets the current BitVector to have the same value as the rhs
     * Returns the current BitVector
     */
    const BitVector<N> & operator=(const BitVector<N> & rhs) {
        for (unsigned int i = 0; i < N; ++i) {
            _bits[i] = rhs._bits[i];
        }
        return *this;
    }

    /*
     * Operator: &=
     * Sets the current BitVector to have values resulting from bitwise AND
     * Returns the current BitVector
     */
    BitVector<N> & operator&=(const BitVector<N> & rhs) {
        for (unsigned int i = 0; i < N; ++i) {
            _bits[i] &= rhs._bits[i];
        }
        return *this;
    }

    /*
     * Operator: ^=
     * Sets the current BitVector to have values resulting from bitwise XOR
     * Returns the current BitVector
     */
    BitVector<N> & operator^=(const BitVector<N> & rhs) {
        for (unsigned int i = 0; i < N; ++i) {
            _bits[i] ^= rhs._bits[i];
        }
        return *this;
    }

    /*
     * Operator: ==
     * Returns whether the current BitVector has values
     * equal to the the values of the rhs
     */
    template<unsigned int M>
    const bool operator==(const BitVector<M> & rhs) const {
        if (N != M) {
            return false;
        }
        for (unsigned int i = 0; i < N; ++i) {
            if (_bits[i] != rhs._bits[i]) {
                return false;
            }
        }
        return true;
    }

    /*
     * Operator: !=
     * Returns whether the current BitVector has any values
     * not equal to the values of the rhs
     */
    template<unsigned int M>
    const bool operator!=(const BitVector<M> & rhs) {
        if (N != M) return true;
        for (unsigned int i = 0; i < N; ++i) {
            if (_bits[i] != rhs._bits[i]) {
                return true;
            }
        }
        return false;
    }

    /*
     * Function: equals(rhs)
     * Returns whether the current BitVector has values
     * equal to the values of an input BitVector
     */
    bool equals(const BitVector<N> & rhs) const {
        for(unsigned int i = 0; i < N; ++i){
            if(_bits[i] != rhs._bits[i]) return false;
        }
        return true;
    }

/* Access and Modification */

    /*
     * Function: elements()
     * Returns the underlying array of bits
     */
    unsigned long long * elements() /*const*/ {
        return _bits;
    }
    
    /*
     * Function: length()
     * Returns the number of bits
     */
    int length() const {
        return N << 6;
    }

    /*
     * Function: parity()
     * Returns 1 if number of bits set in BitVector is odd
     * Returns 0 otherwise.
     */
    const bool parity() const {
        unsigned long long accumulator = 0;
        for (unsigned int i = 0; i < N; ++i) {
            accumulator ^= _bits[i];
        }
        return __builtin_parityll(accumulator)==1;
    }

    /*
     * Function: isZero()
     * Returns whether the current BitVector is a zero vector
     */
    const bool isZero() const {
        for (unsigned int i = 0; i < N; ++i) {
            if (_bits[i] != 0) {
                return false;
            }
        }
        return true;
    }

    /*
     * Function: zero()
     * Sets the current BitVector to be a zero vector
     */
    void zero(){
        for (unsigned int i = 0; i < N; ++i) {
            _bits[i] &= 0;
        }
    }

    /*
     * Function: get(n)
     * Returns the value of the bit at a given index
     */
    bool get(unsigned int n) const {
        return (_bits[n >> 6] & (1ull << (n & 63ull))) != 0;
    }

    /*
     * Function: getFirstOne()
     * Returns the index of the first bit that is 1
     * Returns -1 if all bits are 0
     */
    int getFirstOne() const {
        int r = 0;
        for (int i = 0; i < numBits; ++i) {
            if (get(i)) return i;
        }
        return -1;
    }

    /*
     * Function: set(n)
     * Sets the bit at a given index to 1
     */
    inline void set(unsigned int n) {
        _bits[n >> 6] |= (1ull << (n & 63ull));
    }

    /*
     * Function: clear(n)
     * Sets the bit at a given index to 0
     */
    inline void clear(unsigned int n) {
        _bits[n >> 6] &= ~(1ull << (n & 63ull));
    }

    /*
     * Function: set(n, val)
     * Sets the bit at a given index to a given value
     */
    inline void set(unsigned int n, bool val){
        if(val) set(n);
        else clear(n);
    }

/* Multi-Vector Functions */

    /*
     * Function: dot(rhs)
     * Returns the dot product of the current BitVector with an
     * input BitVector
     */
    bool dot(const BitVector<N> & rhs) const {
        int n = length();
        bool result = 0;
        for(int i = 0; i < n; ++i){
            result ^= (get(i) & rhs.get(i));
        }
        return result;
    }

    /*
     * Function: swap(firstIndex, secondIndex)
     * Swaps the values at two given indices
     */
    void swap(int firstIndex, int secondIndex){
        bool firstOld = get(firstIndex);
        bool secondOld = get(secondIndex);
        set(secondIndex, firstOld);
        set(firstIndex, secondOld);
    }

    /*
     * Function: vcat(v1, v2)
     * Returns a BitVector resulting from the concatenation of
     * two given BitVectors
     */
    template <unsigned int N1, unsigned int N2>
    static const BitVector<N1 + N2> vcat(BitVector<N1> & v1, BitVector<N2> & v2){
        const int N_SUM = N1 + N2;
        BitVector<N_SUM> result;
        unsigned long long *b1 = v1.elements();
        unsigned long long *b2 = v2.elements();
        memcpy(result.elements(), b1, N1*sizeof(unsigned long long));
        memcpy(result.elements() + N1, b2, N2*sizeof(unsigned long long));
        return result;
    }

    /*
     * Function: vcat(v1, v2, v3)
     * Returns a BitVector resulting from the concatenation of
     * three given BitVectors
     */
    template <unsigned int N1, unsigned int N2, unsigned int N3>
    static const BitVector<N1 + N2 + N3> vcat(BitVector<N1> & v1, BitVector<N2> & v2, BitVector<N3> & v3){
        const int N_SUM = N1 + N2 + N3;
        BitVector<N_SUM> result;
        unsigned long long *b1 = v1.elements();
        unsigned long long *b2 = v2.elements();
        unsigned long long *b3 = v3.elements();
        memcpy(result.elements(), b1, N1*sizeof(unsigned long long));
        memcpy(result.elements() + N1, b2, N2*sizeof(unsigned long long));
        memcpy(result.elements() + N1 + N2, b3, N3*sizeof(unsigned long long));
        return result;
    }

    /*
     * Function: proj(v1, v2)
     * Sets two input BitVectors of half the length of the current
     * BitVector to each contain half the values of the current BitVector
     * Assumes that the length of the current Bitvector is divisible by 2
     */
    void proj(BitVector<(N>>1)> & v1, BitVector<(N>>1)> & v2) const{
        unsigned int M = (N >> 1);
        memcpy(v1.elements(), _bits, M*sizeof(unsigned long long));
        memcpy(v2.elements(), _bits+M, M*sizeof(unsigned long long));
    }

    /*
     * Function: proj(v1, v2, v3)
     * Sets three input BitVectors of one-third the length of the current
     * BitVector to each contain one-third the values of the current BitVector
     * Assumes that the length of the current BitVector is divisible by 3
     */
    void proj(BitVector<(N/3)> & v1, BitVector<(N/3)> & v2, BitVector<(N/3)> & v3) const{
        unsigned int M = N/3;
        memcpy(v1.elements(), _bits, M*sizeof(unsigned long long));
        memcpy(v2.elements(), _bits+M, M*sizeof(unsigned long long));
        memcpy(v3.elements(), _bits+2*M, M*sizeof(unsigned long long));
    }    

    /*
     * Function: proj2(part)
     * Returns a BitVector of half the length of the current BitVector
     * containing a specified half of the values of the current BitVector
     * Assumes that the length of the current Bitvector is divisible by 2
     */
    BitVector<(N >> 1)> proj2(int part) const{//part = 0 or 1
        BitVector<(N>>1)> r;
        unsigned int M = (N >> 1);
        memcpy(r.elements(), _bits+(part*M), M*sizeof(unsigned long long));
        return r;
    }

    /*
     * Function: proj3(part)
     * Returns a BitVector of one-third the length of the current BitVector
     * containing a specified third of the values of the current BitVector
     * Assumes that the length of the current Bitvector is divisible by 2
     */
    BitVector<(N/3)> proj3(int part) const{//part = 0, 1, or 2
        BitVector<(N/3)> r;
        unsigned int M = (N/3);
        memcpy(r.elements(), _bits+(part*M), M*sizeof(unsigned long long));
        return r;
    }

/* Print */

    /*
     * Function: print()
     * Prints the values of the current BitVector
     * Ex. [1, 0, 0, 1, 0, 1, 1, 0]
     */
    void print() const {
        std::cout << "[";
        for (int i = 0; i < numBits - 1; ++i) {
            std::cout << get(i) << ", ";
        }
        std::cout << get(numBits - 1) << "]" << std::endl;
    }

private:
    unsigned long long _bits[N]; //array of bit values
    static const unsigned int numBits = N << 6; //number of bits in the array
};

/*
 * Special template for 0-sized BitVector that ensures that the
 * size in memory is at least 1 byte
 */
template<>
class BitVector<0> {
private: 
    unsigned char x;
};

#endif
