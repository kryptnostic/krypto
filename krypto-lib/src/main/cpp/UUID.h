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

#ifndef krypto_UUID_h
#define krypto_UUID_h

class UUID {

public:

/* Constructors */

	/*
     * Constructor
     * Constructs a zero UUID
     */
	UUID() :
	array()
	{
		array[0] = 0ull;
		array[1] = 0ull;
	}


/* Keys */

	/*
	 * Function: setDocKey
	 * Sets the document key of a given object to a given document key
	 * Returns whether the operation was valid and successful
	 */
	// const bool setDocKey(const unsigned int & objectId, const unsigned int & docKey) const{
	// 	return false;
	// }

private:
	unsigned long long array[2];

/* Generators */

	/*
	 * Function: randomUInt()
	 * Returns a random nonzero unsigned int
	 */
	// const unsigned int randomUInt() {
	// 	unsigned int docKey = 0;
 //        while( docKey == 0 ) {
 //            std::fread(&docKey, sizeof( unsigned int ), 1, urandom );
 //        }
	// 	return docKey;
	// }
};

#endif