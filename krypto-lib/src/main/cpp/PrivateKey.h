#ifndef krypto_PrivateKey_h
#define krypto_PrivateKey_h

#include "BitMatrix.h"
#include "MultivariatePolynomialFunction.h"
#include "PolynomialFunctionTupleChain.h"

using namespace std;

/*length of plaintext = 64N; length of obfuscation chain = L*/
template<unsigned int N, unsigned int L>
class PrivateKey {
public:
	PrivateKey():
		_A(BitMatrix<N>::randomInvertibleMatrix(N<<6)),
		_B(BitMatrix<N>::randomInvertibleMatrix(N<<6)),
		_M(BitMatrix<2*N>::randomInvertibleMatrix((2*N)<<6)),
		_f(PolynomialFunctionTupleChain<N,L>::randomPolynomialFunctionTupleChain()){
		generateObfuscationMatrixChains();
	}
	const BitVector<2*N> encrypt(const BitVector<N> m) const{//returns x = E(m, r) given a plaintext m 
		//For now, we assume that m is padded and hashed. These operations will be included later.
		BitVector<N> r = BitVector<N>::randomVector();
		BitVector<N> fr = _f(m);
		BitVector<N> Bm = _B*m;
		BitVector<N> top = Bm ^ (r ^ fr);
		BitVector<N> bottom = _A*r;
		BitVector<2*N> result = BitVector<N>::vcat2(top, bottom);
		return BitVector<N>::vcat2(top, bottom);
	}
private:
	BitMatrix<N> _A, _B; //SL_n(F_2)
	BitMatrix<2*N> _M; //SL_{2n}(F_2)
	PolynomialFunctionTupleChain<N,L> _f; //f_1,...,f_L random quadratic function tuples
	vector<BitMatrix<2*N> > _C_u; //chain of obfuscation matrix for unary operations
	vector<BitMatrix<3*N> > _C_b; //chain of obfuscation matrix for binary operations
	void generateObfuscationMatrixChains(){ //generates C_{u1},...,C_{uL} and C_{b1},...,C_{bL}
		for(int i = 0; i < L; ++i){
			_C_u.push_back(BitMatrix<(2*N)>::randomInvertibleMatrix(N<<7));
			_C_b.push_back(BitMatrix<(3*N)>::randomInvertibleMatrix((3*N)<<6));
		}
	}
};

#endif /* defined(__krypto__PrivateKey__) */