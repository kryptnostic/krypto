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
	cout << "Search Token: " << N << endl;

	cout << endl;
	cout << "Registration: " << endl;

	cout << "PrivateKey: " << sizeof(PrivateKey<N>) << endl;
	cout << "SearchPrivateKey: " << sizeof(SearchPrivateKey<N>) << endl;
	cout << "getClientHashFunction: " << sizeof(ClientHashFunction<N>) << endl;

	cout << endl;
	cout << "Indexing: " << endl;

	cout << "MetadataAddress: " << sizeof(BitVector<N>) << endl;
	cout << "ObjectAddressMatrix: " << sizeof(BitMatrix<N, 2*N>) << endl;
	cout << "ObjectIndexPair: " << sizeof(std::pair <BitVector<N>,BitMatrix<N> >) << endl;

	cout << endl;
	cout << "Search: " << endl;

	cout << "ObjectSearchPair: " << sizeof(std::pair <BitVector<2*N>,BitMatrix<N> >) << endl;
	cout << "EncryptedSearchToken: " << sizeof(BitVector<2*N>) << endl;

	cout << endl;
	cout << "Sharing: " << endl;

	cout << "ObjectSharePair: " << sizeof(std::pair <BitVector<N>,BitMatrix<N> >) << endl;

	return 0;
}
