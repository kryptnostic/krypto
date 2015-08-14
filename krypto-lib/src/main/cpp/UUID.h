//
//  KryptnosticEngine.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 8/13/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ implementation of the Kryptnostic Engine
//  Provides get functions for all of the cryptographic keys and functions
//  necessary for Kryptnostic search functionality
//

#include "BitVector.h"

using namespace std;

struct UUID
{
	unsigned long long first;
	unsigned long long second;
};

/* Generators */

/*
 * Function: randomUUID()
 * Returns a random nonzero UUID
 */
// const struct UUID randomUUID() {
// 	UUID result;
//     // while( result.first == 0 && result.second == 0 ) {
//     //     std::fread(&result.first, sizeof( unsigned long long ), 2, urandom );
//     // }
// 	return result;
// }

/*
 * Function: hashUUID()
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

template<>
struct equal_to<UUID>
{ 
	bool operator()(const UUID &lhs, const UUID &rhs) {
	    return (lhs.first == rhs.first) && (lhs.second && rhs.second);
	}
};


/* Boolean */

// const bool isZero(UUID id) {
// 	return (id.first == 0) && (id.first == 0);
// }