//build instruction:
//g++ -std=c++11 -ftemplate-depth=1000 sizeof.cpp -o sizeof

#include <iostream>
#include <utility>
#include "../cpp/PrivateKey.h"
#include "../cpp/BridgeKey.h"
#include "../cpp/PublicKey.h"
#include "../cpp/SearchPrivateKey.h"
#include "../cpp/MultiQuadTuple.h"

using namespace std;

#define N 128

int main(){
	cout << "N: " << N << endl;

	cout << endl;

	cout << "PrivateKey<N>: " << sizeof(PrivateKey<N>) << endl;
	cout << "BridgeKey<N>: " << sizeof(BridgeKey<N>) << endl;
	cout << "PublicKey<N>: " << sizeof(PublicKey<N>) << endl;
	cout << "SearchPrivateKey<N>: " << sizeof(SearchPrivateKey<N>) << endl;
	cout << "MultiQuadTuple<N,N>: " << sizeof(MultiQuadTuple<N, N>) << endl;
	cout << "MultiQuadTuple<2*N,N>: " << sizeof(MultiQuadTuple<2*N, N>) << endl;
	cout << "MultiQuadTuple<N,2*N>: " << sizeof(MultiQuadTuple<N, 2*N>) << endl;

	cout << endl;
	cout << "Registration: " << endl;

	cout << "getPrivateKey: " << sizeof(PrivateKey<N>) << endl;
	cout << "getSearchPrivateKey: " << sizeof(SearchPrivateKey<N>) << endl;
	cout << "getClientHashFunction: " << sizeof(ClientHashFunction<N>) << endl;

	cout << endl;
	cout << "Indexing: " << endl;

	cout << "getMetadatumAddress: " << sizeof(BitVector<N>) << endl;
	cout << "getObjectAddressFunction: " << sizeof(BitMatrix<N, 2*N>) << endl;
	cout << "getObjectIndexPair: " << sizeof(std::pair <BitVector<2*N>,BitMatrix<N> >) << endl;

	cout << endl;
	cout << "Search: " << endl;

	cout << "getEncryptedSearchToken: " << sizeof(BitVector<2*N>) << endl;

	cout << endl;
	cout << "Sharing: " << endl;

	cout << "getObjectSharingPair: " << sizeof(std::pair <BitVector<2*N>,BitMatrix<N> >) << endl;

	return 0;
}
