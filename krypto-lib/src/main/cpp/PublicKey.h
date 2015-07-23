#ifndef krypto_PublicKey_h
#define krypto_PublicKey_h

#include <assert.h>
#include "PrivateKey.h"
#include "BitMatrix.h"

using namespace std;

//L will be known by the server anyway (as we pass {f_1',...,f_L'} to it)
template<unsigned int N, unsigned int L>
class PublicKey{
public:
	PublicKey(PrivateKey<N, L> pk){
	}
private:
	BitMatrix<N> _BKBi; //to be renamed
};

#endif