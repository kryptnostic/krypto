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
	unsigned long long first;
	unsigned long long second;

	/* Boolean */
	const bool isZero() {
		return (first == 0) && (second == 0);
	}

	/* Generator */

	/*
	 * Function: randomUUID()
	 * Modifies current UUID to be a random nonzero UUID
	 */
	const void randomize() {
	    while( first == 0 && second == 0 ) {
	        std::fread(&first, sizeof( unsigned long long ), 2, urandom );
	    }
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

/*
 * Function: equal_to<UUID>()(id)
 * Compares two UUIDs by directly comparing their components
 */
template<>
struct equal_to<UUID>
{ 
	bool operator()(const UUID &lhs, const UUID &rhs) {
	    return (lhs.first == rhs.first) && (lhs.second && rhs.second);
	}
};