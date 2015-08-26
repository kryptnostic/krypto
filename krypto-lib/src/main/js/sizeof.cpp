//build instruction:
//g++ -std=c++11 -ftemplate-depth=1000 sizeof.cpp -o sizeof

#include <iostream>
#include <utility>
#include "../main/cpp/PrivateKey.h"
#include "../main/cpp/SearchPrivateKey.h"
#include "../main/cpp/MultiQuadTuple.h"

using namespace std;

#define N 128

int main(){
	cout << "N: " << N << endl;

	cout << endl;

	cout << "PrivateKey<N>: " << sizeof(PrivateKey<N>) << endl; //12360
	cout << "SearchPrivateKey<N>: " << sizeof(SearchPrivateKey<N>) << endl; //6144
	cout << "MultiQuadTuple<N,N>: " << sizeof(MultiQuadTuple<N, N>) << endl; //132112
	cout << "MultiQuadTuple<2*N,N>: " << sizeof(MultiQuadTuple<2*N, N>) << endl; //526352
	cout << "MultiQuadTuple<N,2*N>: " << sizeof(MultiQuadTuple<N, 2*N>) << endl; //264224

	cout << endl;
	cout << "Registeration: " << endl;

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