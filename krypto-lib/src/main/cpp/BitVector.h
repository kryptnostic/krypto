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

//TODO: Wrap this in a class that can release the file handle and automatically select a good source of randomness on Windows.

static FILE * urandom = std::fopen("/dev/urandom", "rb" );

//N is number of 64 bit longs in the bitvector. N<<6 is the total number of bits in a bitvector
template<unsigned int N>
class BitVector {
public:
	BitVector() {
        zero();
	}
    
    BitVector( const BitVector & v ) {
        for (unsigned int i = 0; i < N; ++i) {
            _bits[i] = v._bits[i];
        }
    }
    
    BitVector( BitVector && v ) {
        for (unsigned int i = 0; i < N; ++i) {
            _bits[i] = v._bits[i];
        }
    }
    
	unsigned long long * elements() {
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

	BitVector & /*void*/ clear(unsigned int n) {
		_bits[n >> 6ul] &= ~(1ul << (n & 63ul));
		return *this; //added
	}
    
	BitVector<N> operator&(const BitVector<N> & rhs) {
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

	BitVector<N> operator^(const BitVector<N> & rhs) {
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
    	firstOld ? set(secondIndex) : clear(secondIndex);
    	secondOld ? set(firstIndex) : clear(firstIndex);
    }

    BitVector<2*N> vcat(BitVector<N> & rhs){
    	BitVector<2*N> result;
    	unsigned long long *b1 = _bits;
    	unsigned long long *b2 = rhs.elements();
    	memcpy(result.elements(), b1, N*sizeof(unsigned long long));
        memcpy(result.elements() + 1, b2, N*sizeof(unsigned long long));
    	return result;
    }

    void setBits(const unsigned long long *bits){
    	_bits = bits;
    }

    //TODO: to be merged with elements
    const unsigned long long *elements_C() const {
    	return _bits;
    }

    void proj2(BitVector<(N>>1)> & v1, BitVector<(N>>1)> & v2) const{
    	unsigned int M = (N >> 1);
    	memcpy(v1.elements(), _bits, M*sizeof(unsigned long long));
    	memcpy(v2.elements(), _bits+M, M*sizeof(unsigned long long));
    }

    BitVector<(N >> 1)> proj2(int part) const{//part = 0 or 1
    	BitVector<(N>>1)> r;
    	unsigned int M = (N >> 1);
    	memcpy(r.elements(), _bits+(part*M), M*sizeof(unsigned long long));
    	return r;
    }

    static const BitVector<N> proj(const BitVector<2*N> & v, int part){
    	BitVector<N> r;
    	unsigned long long *rbits = r.elements();
    	const unsigned long long *vbits = v.elements_C();
    	memcpy(rbits, vbits+(part*N), N*sizeof(unsigned long long));
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

	void zero() {
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
        if (N != M) {
            return true;
        }
        for (unsigned int i = 0; i < N; ++i) {
            if (_bits[i] != rhs._bits[i]) {
                return true;
            }
        }
        return false;
    }

	void print() const {
		const unsigned int numBits = (N << 6) - 1;
		std::cout << "[ ";
		for (int i = 0; i < numBits; ++i) {
			std::cout << get(i) << ", ";
		}
		std::cout << get(numBits) << "]" << std::endl;
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
};


#endif
