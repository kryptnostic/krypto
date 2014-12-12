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

template<unsigned int N>
class BitVector {
public:
    BitVector(){}
    
    //Generates a random vector, shoudl be improved to not just multiply two random longs together.
    static BitVector<N> randomVector() {
        srandom(time(NULL));
        BitVector<N> result;
        for( unsigned int i = 0 ; i < N ; ++i) {
            result.bits[i] = random()*random();
        }
        return result;
    }
    
    long * elements(){ return bits; }
    int length() { return N<<6; }
    
    bool get( unsigned int n ) {
        return (bits[ n >> 6 ] & (1ul<<(n&63ul)))!=0;
    }
    
    BitVector & set( unsigned int n ) {
        bits[ n >> 6 ] |= (1ul<<(n&63ul));
        return *this;
    }
    
    void clear( unsigned int n ) {
         bits[ n >> 6ul ] &= ~(1ul<<(n&63ul));
    }
    
    BitVector<N> operator&( const BitVector<N> & rhs ) {
        BitVector<N> result;
        for( unsigned int i = 0 ; i < N ; ++i) {
            result.bits[i] = bits[i]&rhs.bits[i];
        }
        return *this;
    }
    
    BitVector<N> operator|( const BitVector<N> & rhs ) {
        BitVector<N> result;
        for( unsigned int i = 0 ; i < N ; ++i) {
            result.bits[i] = bits[i]|rhs.bits[i];
        }
        return *this;
    }
    
    BitVector<N> operator^( const BitVector<N> & rhs ) {
        BitVector<N> result;
        for( unsigned int i = 0 ; i < N ; ++i) {
            result.bits[i] = bits[i]^rhs.bits[i];
        }
        return *this;
    }
    
    BitVector<N> & operator^=( const BitVector<N> & rhs ) {
        for( unsigned int i = 0 ; i < N ; ++i) {
            bits[i]^=rhs.bits[i];
        }
        return *this;
    }
    
    template<unsigned int M>
    const bool operator==( const BitVector<M> & rhs ) {
        if( N != M ) {
            return false;
        }
        for( unsigned int i = 0 ; i < N ; ++i) {
            if(bits[i] != rhs.bits[i]) {
                return false;
            }
        }
        return true;
    }
    
    void print() {
        const unsigned int numBits = (N << 6) - 1;
        std::cout<<"[ ";
        for( int i = 0; i < numBits ; ++i ) {
            std::cout<< get( i )<<", ";
        }
        std::cout<<get( numBits )<<"]"<<std::endl;
    }
private:
    unsigned long long bits[N];
 
};


#endif
