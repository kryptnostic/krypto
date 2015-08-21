//
//  ClientHashFunction.h
//  krypto
//
//  Created by Robin Zhang on 8/21/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ struct concatenating the three components of the ClientHashFunction h_\Omega
//

#include "MultiQuadTuple.h"

using namespace std;

struct ClientHashFunction
{
	/* Data */
	BitMatrix<N, 4*N> hashMatrix;
	MultiQuadTuple<N, 2*N> augmentedF2;
	MultiQuadTuple<2*N, N> concealedF1;

};
