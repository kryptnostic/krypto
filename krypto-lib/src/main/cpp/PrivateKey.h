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
	const BitVector<2*N> encrypt(BitVector<N> m) const{//returns x = E(m, r) given a plaintext m 
		//For now, we assume that m is padded and hashed. These operations will be included later.
		BitVector<N> r = BitVector<N>::randomVector();
		BitVector<N> fr = eval(_f, r); //TODO: shift this to another class
		BitVector<N> Bm = _B*m;
		BitVector<N> top = Bm ^ (r ^ fr);
		BitVector<N> bottom = _A*r;
		BitVector<2*N> result = BitVector<N>::vcat2(top, bottom);
		return BitVector<N>::vcat2(top, bottom);
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
	const BitVector<N> eval(vector<MultivariatePolynomialFunction<N,N> > f, BitVector<N> m) const{
		BitVector<N> result = m;
		int l = _f.size();
		for(int i = 0; i < l; ++i){
			MultivariatePolynomialFunction<N,N> ff = f[i];
			result = ff(result);
		}
		return result;
	}
};

#endif /* defined(__krypto__PrivateKey__) */