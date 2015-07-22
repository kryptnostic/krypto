#ifndef krypto_PrivateKey_h
#define krypto_PrivateKey_h

#include <iostream>
#include <vector>
#include "BitMatrix.h"
#include "MultivariatePolynomialFunction.h"

using namespace std;

/*length of plaintext = 64N; length of obfuscation chain = L*/
template<unsigned int N, unsigned int L>
class PrivateKey {
public:
	PrivateKey():
		_A(BitMatrix<N>::randomInvertibleMatrix(N<<6)),
		_B(BitMatrix<N>::randomInvertibleMatrix(N<<6)),
		_M(BitMatrix<2*N>::randomInvertibleMatrix((2*N)<<6)){
		generateObfuscationFunctionChain();
		generateObfuscationMatrixChains();
	}
	BitMatrix<N> getA(){
		return _A;
	}
private:
	BitMatrix<N> _A, _B; //SL_n(F_2)
	BitMatrix<2*N> _M; //SL_{2n}(F_2)
	vector<MultivariatePolynomialFunction<N,N> > _f; //f_1,...,f_L random multivariate quadratic function tuples
	vector<BitMatrix<2*N> > _C_u; //chain of obfuscation matrix for unary operations
	vector<BitMatrix<3*N> > _C_b; //chain of obfuscation matrix for binary operations
	void generateObfuscationFunctionChain(){//generates f_1,...,f_L
		for(int i = 0; i < L; ++i)
			_f.push_back(MultivariatePolynomialFunction<N,N>::denseRandomMultivariateFunctions());
	}
	void generateObfuscationMatrixChains(){ //generates C_{u1},...,C_{uL} and C_{b1},...,C_{bL}
		for(int i = 0; i < L; ++i){
			_C_u.push_back(BitMatrix<(2*N)>::randomInvertibleMatrix(N<<7));
			_C_b.push_back(BitMatrix<(3*N)>::randomInvertibleMatrix((3*N)<<6));
		}
	}
};

#endif /* defined(__krypto__PrivateKey__) */