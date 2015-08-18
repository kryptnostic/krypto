//
//  UUID.h
//  krypto
//
//  Created by Robin Zhang on 8/14/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ implementation of the UUID
//  Serves as an extension of unsigned long longs
//  with extended hash function and comparators
//

#include "BitVector.h"

using namespace std;

struct UUID
{
	/* Data */
	unsigned long long first = 0ull;
	unsigned long long second = 0ull;

	/* Boolean */

	/*
	 * Function: isZero()
	 * Returns whether current UUID is zero
	 */
	const bool isZero() const {
		return (first == 0) && (second == 0);
	}

	/*
	 * Operator: ==(rhs)
	 * Compares two UUIDs by directly comparing their components
	 */
	bool operator==(const UUID &rhs) const {
	    return (first == rhs.first) && (second == rhs.second);
	}

	/* Generator */
	/*
	 * Function: zero()
	 * Modifies current UUID to be a zero UUID
	 */
	const void zero() {
		first = 0ull;
		second = 0ull;
	}

	/*
	 * Function: randomize()
	 * Modifies current UUID to be a random nonzero UUID
	 */
	const void randomize() {
	    while( first == 0 && second == 0 ) {
	        std::fread(&first, sizeof( unsigned long long ), 2, urandom );
	    }
	}

	/*
     * Function: get(n)
     * Returns the value of the bit at a given index
     */
    bool get(unsigned int n) const {
    	if (n < 2 * sizeof(unsigned long long) * 8) return (first & (1ull << (n & 63ull))) != 0;
        return (second & (1ull << (n & 63ull))) != 0;
    }

	/*
     * Function: print()
     * Prints the values of the current BitVector
     * Ex. [1, 0, 0, 1, 0, 1, 1, 0]
     */
    void print() const {
        std::cout << "[";
        for (int i = 0; i < 2 * sizeof(unsigned long long) * 8; ++i) {
            std::cout << get(i) << ", ";
        }
        std::cout << get(2 * sizeof(unsigned long long) * 8) << "]" << std::endl;
    }
};

/*
 * Function: hash<UUID>()(id)
 * Hashes a UUID by taking the sum of the standard hashes of its components
 */
template<>
struct hash<UUID>
{ 
    size_t operator()(UUID const& id) const
    {
        size_t const h1 ( hash<unsigned long long>()(id.first) );
        size_t const h2 ( hash<unsigned long long>()(id.second) );
        return h1 + h2;
    }
};