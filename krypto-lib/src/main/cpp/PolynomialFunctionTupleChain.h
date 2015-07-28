#ifndef krypto_PolynomialFunctionTupleChain_h
#define krypto_PolynomialFunctionTupleChain_h

#include <iostream>
#include <vector>
#include "BitVector.h"
#include "MultivariateQuadraticFunctiontuple.h"

//for now, assuming that the input and output all live in F_2^{N*2^6}
//ChainPolynomialFunctionTuple f = {f_1,...,f_L}, f_i: F_2^N -> F_2^N
//TODO: implement for different dimensions for all f_i
//TODO: come up with a better name for this class!
template<unsigned int N, unsigned int L>
class PolynomialFunctionTupleChain {
public:
	PolynomialFunctionTupleChain(){
	}

	int length() const{
		return L;
	}

    static const PolynomialFunctionTupleChain<N, L> randomPolynomialFunctionTupleChain() {
        PolynomialFunctionTupleChain<N, L> result;
        for(int i = 0; i < L; ++i){
        	result.addFunctionTuple(MultivariateQuadraticFunctionTuple<N,N>::randomMultivariateQuadraticFunctionTuple());
        }
        return result;
    }

	const BitVector<N> operator()(const BitVector<N> m) const{
		BitVector<N> result = m;
		for(int i = 0; i < L; ++i){
			result = (_tuple[i])(result);
		}
		return result;
	} 

	void addFunctionTuple(const MultivariateQuadraticFunctionTuple<N,N> & f){
		_tuple.push_back(f);
	}

private:
	vector<MultivariateQuadraticFunctionTuple<N,N> > _tuple;
};

#endif