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
#include <cstdio>
#include <string>
#include <assert.h>
#define _KBV_N_ (NUM_BITS>>6)

using namespace std;

//TODO: Wrap this in a class that can release the file handle and automatically select a good source of randomness on Windows.
//file pointer urandom must be closed by any class importing BitVector
static FILE * urandom = std::fopen("/dev/urandom", "rb" );

/*
 * Template for BitVector
 * Bit values are stored in an array of N many 64-bit longs
 * in the array used to store the bits
 */
template<unsigned int NUM_BITS>
class BitVector {
public:

/* Generation */

    /*
     * Function: zeroVector()
     * Returns a zero-initialized BitVector
     */
    static const BitVector<NUM_BITS> & zeroVector() {
        static BitVector<NUM_BITS> v;
        v.zero();
        return v;
    }

    /*
     * Function: randomVector()
     * Returns a nonzero BitVector with random values
     */
    static const BitVector<NUM_BITS> randomVector() {
        BitVector<NUM_BITS> result = BitVector<NUM_BITS>::zeroVector();
        while( result.isZero() ) {
            std::fread(&result._bits, sizeof( unsigned long long ),_KBV_N_, urandom );
        }
        return result;
    }

    /*
     * Function: randomVector()
     * Returns a BitVector with leading term zeroed
     */
    static const BitVector<NUM_BITS> randomSmallVector(){
        BitVector<NUM_BITS> result = BitVector<NUM_BITS>::randomVector();
        result.clear(0);
        return result;
    }

    /*
     * Function: randomVectorLeadingZeroes(n)
     * Returns a BitVector with n leading zeroes followed by random values
     * Assumes n < numBits
     */
    static const BitVector<NUM_BITS> randomVectorLeadingZeroes(unsigned int n) {
        BitVector<NUM_BITS> result = BitVector<NUM_BITS>::randomVector();

        for (unsigned int i = 0; i < n; ++i) {
            result.clear(i);
        }
        return result;
    }

/* Operators */

    /*
     * Operator: []
     * Returns the value of the bit at a given index
     */
    const bool operator[](unsigned int n) const{
        return get(n);
    }

    /*
     * Operator: &
     * Returns a BitVector with values resulting from bitwise AND
     */
    BitVector<NUM_BITS> operator&(const BitVector<NUM_BITS> & rhs) const {
        BitVector<NUM_BITS> result;
        for (unsigned int i = 0; i < _KBV_N_; ++i) {
            result._bits[i] = _bits[i] & rhs._bits[i];
        }
        return result;
    }

    /*
     * Operator: |
     * Returns a BitVector with values resulting from bitwise OR
     */
    BitVector<NUM_BITS> operator|(const BitVector<NUM_BITS> & rhs) {
        BitVector<NUM_BITS> result;
        for (unsigned int i = 0; i < _KBV_N_; ++i) {
            result._bits[i] = _bits[i] | rhs._bits[i];
        }
        return result;
    }

    /*
     * Operator: ^
     * Returns a BitVector with values resulting from bitwise XOR
     */
    BitVector<NUM_BITS> operator^(const BitVector<NUM_BITS> & rhs) const {
        BitVector<NUM_BITS> result;
        for (unsigned int i = 0; i < _KBV_N_; ++i) {
            result._bits[i] = _bits[i] ^ rhs._bits[i];
        }
        return result;
    }

    /*
     * Operator: =
     * Sets the current BitVector to have the same value as the rhs
     * Returns the current BitVector
     */
    const BitVector<NUM_BITS> & operator=(const BitVector<NUM_BITS> & rhs) {
        for (unsigned int i = 0; i <_KBV_N_; ++i) {
            _bits[i] = rhs._bits[i];
        }
        return *this;
    }

    /*
     * Operator: &=
     * Sets the current BitVector to have values resulting from bitwise AND
     * Returns the current BitVector
     */
    BitVector<NUM_BITS> & operator&=(const BitVector<NUM_BITS> & rhs) {
        for (unsigned int i = 0; i < _KBV_N_; ++i) {
            _bits[i] &= rhs._bits[i];
        }
        return *this;
    }

    /*
     * Operator: ^=
     * Sets the current BitVector to have values resulting from bitwise XOR
     * Returns the current BitVector
     */
    BitVector<NUM_BITS> & operator^=(const BitVector<NUM_BITS> & rhs) {
        for (unsigned int i = 0; i < _KBV_N_; ++i) {
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
        if (NUM_BITS != M) {
            return false;
        }
        for (unsigned int i = 0; i < _KBV_N_; ++i) {
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
    const bool operator!=(const BitVector<M> & rhs) const {
        if (NUM_BITS != M) return true;
        for (unsigned int i = 0; i < _KBV_N_; ++i) {
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
    bool equals(const BitVector<NUM_BITS> & rhs) const {
        for(unsigned int i = 0; i < _KBV_N_; ++i){
            if(_bits[i] != rhs._bits[i]) return false;
        }
        return true;
    }

/* Access and Modification */

    /*
     * Function: elements()
     * Returns the underlying array of bits
     */
    unsigned long long * elements() const {
        return const_cast<unsigned long long *>(_bits);
    }

    /*
     * Function: parity()
     * Returns 1 if number of bits set in BitVector is odd
     * Returns 0 otherwise.
     */
    const bool parity() const {
        unsigned long long accumulator = 0;
        for (unsigned int i = 0; i < _KBV_N_; ++i) {
            accumulator ^= _bits[i];
        }
        return __builtin_parityll(accumulator)==1;
    }

    /*
     * Function: isZero()
     * Returns whether the current BitVector is a zero vector
     */
    const bool isZero() const {
        for (unsigned int i = 0; i <_KBV_N_; ++i) {
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
        for (unsigned int i = 0; i <_KBV_N_; ++i) {
            _bits[i] &= 0;
        }
    }

    /*
     * Function: get(n)
     * Returns the value of the bit at a given index
     */
    bool get(unsigned int n) const {
       return (_bits[n >> 6] & (1ull << (n & 63u))) != 0;
    }

    /*
     * Function: getFirstOne()
     * Returns the index of the first bit that is 1
     * Returns -1 if all bits are 0
     */
    int getFirstOne() const {
        for (int i = 0; i < NUM_BITS; ++i) {
            if (get(i)) return i;
        }
        return -1;
    }

    /*
     * Function: set(n)
     * Sets the bit at a given index to 1
     */
    inline void set(unsigned int n) {
        _bits[n >> 6] |= (1ull << (n & 63u));
    }

    /*
     * Function: clear(n)
     * Sets the bit at a given index to 0
     */
    inline void clear(unsigned int n) {
        _bits[n >> 6] &= ~(1ull << (n & 63u));
    }

    /*
     * Function: set(n, val)
     * Sets the bit at a given index to a given value
     */
    inline void set(unsigned int n, bool val){
        if(val) set(n);
        else clear(n);
    }

/* Multi-Vector and Sub-Vector */

    /*
     * Function: dot(rhs)
     * Returns the dot product of the current BitVector with an
     * input BitVector
     */
    bool dot(const BitVector<NUM_BITS> & rhs) const {
        bool result = 0;
        for(int i = 0; i < NUM_BITS; ++i){
            result ^= (get(i) & rhs.get(i));
        }
        return result;
    }

    /*
     * Function: swap(firstIndex, secondIndex)
     * Swaps the values at two given indices
     */
    void swap(unsigned int firstIndex, unsigned int secondIndex){
        bool firstOld = get(firstIndex);
        bool secondOld = get(secondIndex);
        set(secondIndex, firstOld);
        set(firstIndex, secondOld);
    }

    /*
     * Function: vCat(v1, v2)
     * Returns a BitVector resulting from the concatenation of
     * two given BitVectors
     */
    template <unsigned int N1, unsigned int N2>
    static const BitVector<N1 + N2> vCat(const BitVector<N1> & v1, const BitVector<N2> & v2){
        const unsigned int N_SUM = N1 + N2;
        unsigned int K1 = N1 >> 6;
        unsigned int K2 = N2 >> 6;
        BitVector<N_SUM> result;
        unsigned long long *b1 = v1.elements();
        unsigned long long *b2 = v2.elements();
        memcpy(result.elements(), b1, K1*sizeof(unsigned long long));
        memcpy(result.elements() + K1, b2, K2*sizeof(unsigned long long));
        return result;
    }

    /*
     * Function: vCat(v1, v2, v3)
     * Returns a BitVector resulting from the concatenation of
     * three given BitVectors
     */
    template <unsigned int N1, unsigned int N2, unsigned int N3>
    static const BitVector<N1 + N2 + N3> vCat(const BitVector<N1> & v1, const BitVector<N2> & v2, const BitVector<N3> & v3){
        const unsigned int N_SUM = N1 + N2 + N3;
        unsigned int K1 = N1 >> 6;
        unsigned int K2 = N2 >> 6;
        unsigned int K3 = N3 >> 6;
        BitVector<N_SUM> result;
        unsigned long long *b1 = v1.elements();
        unsigned long long *b2 = v2.elements();
        unsigned long long *b3 = v3.elements();
        memcpy(result.elements(), b1, K1*sizeof(unsigned long long));
        memcpy(result.elements() + K1, b2, K2*sizeof(unsigned long long));
        memcpy(result.elements() + K1 + K2, b3, K3*sizeof(unsigned long long));
        return result;
    }

    /*
     * Function: proj(v1, v2)
     * Sets two input BitVectors of half the length of the current
     * BitVector to each contain half the values of the current BitVector
     * Assumes that the length of the current Bitvector is divisible by 2
     * and greater than 128
     */
    void proj(BitVector<(NUM_BITS>>1)> & v1, BitVector<(NUM_BITS>>1)> & v2) const{
        unsigned int M = (_KBV_N_ >> 1);
        memcpy(v1.elements(), _bits, M*sizeof(unsigned long long));
        memcpy(v2.elements(), _bits+M, M*sizeof(unsigned long long));
    }

    /*
     * Function: proj(v1, v2, v3)
     * Sets three input BitVectors of one-third the length of the current
     * BitVector to each contain one-third the values of the current BitVector
     * Assumes that the length of the current BitVector is divisible by 3
     */
    void proj(BitVector<(NUM_BITS/3)> & v1, BitVector<(NUM_BITS/3)> & v2, BitVector<(NUM_BITS/3)> & v3) const{
        unsigned int M = _KBV_N_/3;
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
    BitVector<(NUM_BITS >> 1)> proj2(unsigned int part) const{//part = 0 or 1
        BitVector<(NUM_BITS>>1)> result;
        unsigned int M = (_KBV_N_ >> 1);
        memcpy(result.elements(), _bits+(part*M), M*sizeof(unsigned long long));
        return result;
    }

    /*
     * Function: proj3(part)
     * Returns a BitVector of one-third the length of the current BitVector
     * containing a specified third of the values of the current BitVector
     * Assumes that the length of the current Bitvector is divisible by 2
     */
    BitVector<(NUM_BITS/3)> proj3(unsigned int part) const{//part = 0, 1, or 2
        BitVector<(NUM_BITS/3)> result;
        unsigned int M = (_KBV_N_/3);
        memcpy(result.elements(), _bits+(part*M), M*sizeof(unsigned long long));
        return result;
    }

    /*
     * Function: leftShift(n)
     * Returns a BitVector with the values of the current BitVector
     * shifted to the left by n and trailing zeroes
     */
    const BitVector<NUM_BITS> leftShift(unsigned int n){
        BitVector<NUM_BITS> result = BitVector<NUM_BITS>::zeroVector();
        for(unsigned int i = 0; i < NUM_BITS - n; ++i)
            result.set(i, get(i + n));
        return result;
    }

/* Print */

    /*
     * Function: print()
     * Prints the values of the current BitVector
     * Ex. [1, 0, 0, 1, 0, 1, 1, 0]
     */
    void print() const {
        std::cout << "[";
        for (int i = 0; i < NUM_BITS - 1; ++i) {
            std::cout << get(i) << ", ";
        }
        std::cout << get(NUM_BITS - 1) << "]" << std::endl;
    }

private:
    unsigned long long _bits[_KBV_N_]; //array of bit values
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

/*
 * Function: hash<BitVector<N> >()(vector)
 * Hashes a BitVector by taking the sum of the standard hashes of its components
 */

template<unsigned int N>
struct hash< BitVector<N> >
{
    size_t operator()(BitVector<N> const& vector) const
    {
        size_t sum = 0;
        for (int i = 0; i < N; ++i) {
            size_t const h ( hash<unsigned long long>()(vector.elements()[i]) );
            sum ^= h;
        }
        return sum;
    }
};

#endif