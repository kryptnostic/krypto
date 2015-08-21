//
//  MultiQuadTupleChain.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 7/29/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  Implementation of tuples of multivariate quadratic polynomials over F2
//

#ifndef krypto_MultiQuadTupleChain_h
#define krypto_MultiQuadTupleChain_h

#include <vector>
#include "MultiQuadTuple.h"

//for now, assuming that the input and output all live in F_2^{N*2^6}
//ChainPolynomialFunctionTuple f = {f_1,...,f_L}, f_i: F_2^N -> F_2^N
//TODO: implement for different dimensions for all f_i
template<unsigned int N, unsigned int L>
class MultiQuadTupleChain {
public:
	MultiQuadTupleChain(){
	}

	unsigned int length() const{
		return L;
	}

    static const MultiQuadTupleChain<N, L> randomMultiQuadTupleChain() {
        MultiQuadTupleChain<N, L> result;
        for(unsigned int i = 0; i < L; ++i){
        	MultiQuadTuple<N,N> mqt;
        	mqt.randomize();
        	result.addFunctionTuple(mqt);
        }
        return result;
    }

	const BitVector<N> operator()(const BitVector<N> m) const{
		BitVector<N> result = m;
		for(unsigned int i = 0; i < L; ++i){
			result = (_tuple[i])(result);
		}
		return result;
	}

	void addFunctionTuple(const MultiQuadTuple<N,N> & f){
		_tuple.push_back(f);
	}

	const MultiQuadTuple<N,N> get(const unsigned int index) const{
		return _tuple[index];
	}

private:
	vector<MultiQuadTuple<N,N> > _tuple;
};

#endif