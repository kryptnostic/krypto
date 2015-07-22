/*Accessed by client*/
#ifndef krypto_PrivateKey_h
#define krypto_PrivateKey_h

#include <assert.h>
#include "BitMatrix.h"
#include "MultivariatePolynomialFunction.h"
#include "PolynomialFunctionTupleChain.h"

using namespace std;

/*length of plaintext = N*2^6; length of obfuscation chain = L*/
template<unsigned int N, unsigned int L>
class PrivateKey {
//friend class BridgeKey;
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
		BitVector<N> top = _B*m ^ (r ^ _f(r));
		BitVector<N> bottom = _A*r;
		BitVector<2*N> result = BitVector<N>::vcat2(top, bottom);
		return BitVector<N>::vcat2(top, bottom);
	}
	
	const BitVector<N> decrypt(const BitVector<2*N> x) const{//returns m = D(x) given a ciphertext x
		BitVector<N> x1, x2;
		x.proj2(x1, x2);
		BitVector<N> Aix2 = _A.solve(x2);
		BitVector<N> fAix2 = _f(Aix2);
		return _B.solve(x1 ^ (Aix2 ^ fAix2)); 
	}

/*protected:*/
	/****************TODO: make the following protected functions only accessible to BridgeKey*************/
	const BitMatrix<N> getA() const{
		return _A;
	}

	const BitMatrix<N> getB() const{ 
		return _B;
	}

	const BitMatrix<2*N> getM() const{
		return _M;
	}

	const PolynomialFunctionTupleChain<N,L> getf() const{
		return _f;
	}

	const vector<BitMatrix<2*N> > getUnaryObfChain() const{
		return _C_u;
	}
	
	const vector<BitMatrix<3*N> > getBinaryObfChain() const{
		return _C_b;
	}
	/****************** end of TODO ******************************/
private:
	BitMatrix<N> _A, _B; //SL_n(F_2)
	BitMatrix<2*N> _M; //SL_{2n}(F_2)
	PolynomialFunctionTupleChain<N,L> _f; //{f_1,...,f_L} random quadratic function tuples
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