//
//  BitVector.h
//  krypto
//
//  Created by Matthew Tamayo on 12/11/14.
//  Copyright (c) 2014 Kryptnostic. All rights reserved.
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
//extern FILE * urandom = std::fopen("/dev/urandom", "rb" );
static FILE * urandom = std::fopen("/dev/urandom", "rb" );

//N is number of 64 bit longs in the bitvector. N<<6 is the total number of bits in a bitvector
template<unsigned int N>
class BitVector {
public:
    BitVector() {
        zero();
    }
    
    BitVector( const BitVector<N> & v ) {
        for (unsigned int i = 0; i < N; ++i) {
            _bits[i] = v._bits[i];
        }
    }
    
    BitVector( BitVector<N> && v ) {
        for (unsigned int i = 0; i < N; ++i) {
            _bits[i] = v._bits[i];
        }
    }
    
    unsigned long long * elements() /*const*/ {
        return _bits;
    }
    
    int length() const {
        return N << 6;
    }

    bool get(unsigned int n) const {
        return (_bits[n >> 6] & (1ul << (n & 63ul))) != 0;
    }

    bool operator[](unsigned int n) const{
        return get(n);
    }

    BitVector & set(unsigned int n) {
        _bits[n >> 6] |= (1ul << (n & 63ul));
        return *this;
    }

    BitVector & clear(unsigned int n) {
        _bits[n >> 6ul] &= ~(1ul << (n & 63ul));
        return *this; //added
    }

    BitVector & set(unsigned int n, bool val){
        if(val) return set(n);
        return clear(n);
    }
    
    BitVector<N> operator&(const BitVector<N> & rhs) const {
        BitVector<N> result;
        for (unsigned int i = 0; i < N; ++i) {
            result._bits[i] = _bits[i] & rhs._bits[i];
        }
        return result;
    }

    BitVector<N> operator|(const BitVector<N> & rhs) {
        BitVector<N> result;
        for (unsigned int i = 0; i < N; ++i) {
            result._bits[i] = _bits[i] | rhs._bits[i];
        }
        return result;
    }

    BitVector<N> operator^(const BitVector<N> & rhs) const {
        BitVector<N> result;
        for (unsigned int i = 0; i < N; ++i) {
            result._bits[i] = _bits[i] ^ rhs._bits[i];
        }
        return result;
    }

    const BitVector<N> & operator=(const BitVector<N> & rhs) {
        for (unsigned int i = 0; i < N; ++i) {
            _bits[i] = rhs._bits[i];
        }
        return *this;
    }
    
    BitVector<N> & operator^=(const BitVector<N> & rhs) {
        for (unsigned int i = 0; i < N; ++i) {
            _bits[i] ^= rhs._bits[i];
        }
        return *this;
    }

    BitVector<N> & operator&=(const BitVector<N> & rhs) {
        for (unsigned int i = 0; i < N; ++i) {
            _bits[i] &= rhs._bits[i];
        }
        return *this;
    }

    bool dot(const BitVector<N> & rhs) const {
        int n = length();
        assert(n == rhs.length());
        bool result = 0;
        for(int i = 0; i < n; ++i){
            result ^= (get(i) & rhs.get(i));
        }
        return result;
    }

    bool equals(const BitVector<N> & rhs) const {
        int n = length();
        assert(n == rhs.length());
        for(int i = 0; i < n; ++i){
            if(get(i) ^ rhs.get(i)) return false;
        }
        return true;
    }

    void swap(int firstIndex, int secondIndex){
        bool firstOld = get(firstIndex);
        bool secondOld = get(secondIndex);
        set(secondIndex, firstOld);
        set(firstIndex, secondOld);
    }

    template <unsigned int N1, unsigned int N2>
    //static const BitVector<N1 + N2> vcat2(const BitVector<N1> & v1, const BitVector<N2> & v2){
    static const BitVector<N1 + N2> vcat2(BitVector<N1> & v1, BitVector<N2> & v2){
        const int N_SUM = N1 + N2;
        BitVector<N_SUM> result;
        unsigned long long *b1 = v1.elements();
        unsigned long long *b2 = v2.elements();
        memcpy(result.elements(), b1, N1*sizeof(unsigned long long));
        memcpy(result.elements() + N1, b2, N2*sizeof(unsigned long long));
        return result;
    }

    template <unsigned int N1, unsigned int N2, unsigned int N3>
    static const BitVector<N1 + N2 + N3> vcat3(BitVector<N1> & v1, BitVector<N2> & v2, BitVector<N3> & v3){
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

    void proj2(BitVector<(N>>1)> & v1, BitVector<(N>>1)> & v2) const{
        unsigned int M = (N >> 1);
        memcpy(v1.elements(), _bits, M*sizeof(unsigned long long));
        memcpy(v2.elements(), _bits+M, M*sizeof(unsigned long long));
    }

    void proj3(BitVector<(N/3)> & v1, BitVector<(N/3)> & v2, BitVector<(N/3)> & v3) const{
        unsigned int M = N/3;
        memcpy(v1.elements(), _bits, M*sizeof(unsigned long long));
        memcpy(v2.elements(), _bits+M, M*sizeof(unsigned long long));
        memcpy(v3.elements(), _bits+2*M, M*sizeof(unsigned long long));
    }    

    BitVector<(N >> 1)> proj2(int part) const{//part = 0 or 1
        BitVector<(N>>1)> r;
        unsigned int M = (N >> 1);
        memcpy(r.elements(), _bits+(part*M), M*sizeof(unsigned long long));
        return r;
    }

    BitVector<(N/3)> proj3(int part) const{//part = 0, 1, or 2
        BitVector<(N/3)> r;
        unsigned int M = (N/3);
        memcpy(r.elements(), _bits+(part*M), M*sizeof(unsigned long long));
        return r;
    }

    const bool isZero() const {
        for (unsigned int i = 0; i < N; ++i) {
            if (_bits[i] != 0) {
                return false;
            }
        }
        return true;
    }
    
    /*
     * Returns 1 if number of bits set in bitvector is odd
     * Returns 0 otherwise.
     */
    const bool parity() const {
        unsigned long long accumulator = 0;
        for (unsigned int i = 0; i < N; ++i) {
            accumulator ^= _bits[i];
        }
        return __builtin_parityll(accumulator)==1;
    }

    void zero(){
        for (unsigned int i = 0; i < N; ++i) {
            _bits[i] &= 0;
        }
    }

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

    void print() const {
        std::cout << "[";
        for (int i = 0; i < numBits - 1; ++i) {
            std::cout << get(i) << ", ";
        }
        std::cout << get(numBits - 1) << "]" << std::endl;
    }

    static const BitVector<N> & zeroVector() {
        static BitVector<N> v;
        return v;
    }
    
    //Generates a random vector, shoudl be improved to not just multiply two random longs together.
    static const BitVector<N> randomVector() {
        BitVector<N> result;
        
        while( result.isZero() ) {
            std::fread(&result._bits, sizeof( unsigned long long ), N, urandom );
        }
        return result;
    }

private:
    unsigned long long _bits[N];
    static const unsigned int numBits = N << 6;
};

template<>
class BitVector<0> {
private: 
    unsigned char x;
};

#endif
