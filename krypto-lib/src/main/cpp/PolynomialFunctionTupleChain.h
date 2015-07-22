#ifndef krypto_PolynomialFunctionTupleChain_h
#define krypto_PolynomialFunctionTupleChain_h

#include <iostream>
#include <vector>

//for now, assuming that the input and output all live in F_2^{N*2^6}
//ChainPolynomialFunctionTuple f = {f_1,...,f_L}, f_i: F_2^N -> F_2^N
//TODO: implement for different dimensions for all f_i
//TODO: come up with a better name for this class!
template<unsigned int N, unsigned int L>
class PolynomialFunctionTupleChain {
public:
	PolynomialFunctionTupleChain(){
	}

	int length(){
		return L;
	}

    static const PolynomialFunctionTupleChain<N, L> randomPolynomialFunctionTupleChain() {
        PolynomialFunctionTupleChain<N, L> result;
        for(int i = 0; i < L; ++i){
        	result._tuple.push_back(MultivariatePolynomialFunction<N,N>::denseRandomMultivariateFunctions()); 
        }
        return result;
    }

	const BitVector<N> operator()(const BitVector<N> m) const{
		BitVector<N> result = m;
		for(int i = 0; i < L; ++i){
			MultivariatePolynomialFunction<N,N> ff = _tuple[i];
			result = ff(result);
		}
		return result;
	}    
private:
	vector<MultivariatePolynomialFunction<N,N> > _tuple;
};

#endif