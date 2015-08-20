#ifndef krypto_SearchPrivateKey_h
#define krypto_SearchPrivateKey_h

#include "PrivateKey.h"
#include "UUID.h"
#include <unordered_map>
#include <unordered_set>

template<unsigned int N, unsigned int L>
class SearchPrivateKey{
public:
	SearchPrivateKey(const PrivateKey<N, L> & pk) :
	_pk(pk),
	_K(generateK()),
	_C(BitMatrix<N>::randomInvertibleMatrix())
	{ }

/* Getters */

	/*
     * Function: generateK()
     * Returns the random client-specific n x 2n matrix K_\Omega
     */
	const BitMatrix<2*N> getK() const{
		return _K;
	}

	/*
	 * Function: getDocKey
	 * Returns a serialized random unused document key
	 * and inserts the document key into a stored hash set
	 * Returns existing key if object has an existing key
	 */
	const BitVector<N> getDocKey( const UUID & objectId) {
		BitVector<N> docKey = generateDocKey(objectId);
		if (!docKey.isZero()) { //objectId already used
			while (docKeySet.count(docKey) == 1) docKey = generateDocKey(objectId); //generated new key
			docToKeyMap[objectId] = docKey;
			docKeySet.insert(docKey);
		} else docKey = docToKeyMap[objectId];
		return docKey;
	}

	/*
	 * Function: setDocKey
	 * Sets the document key of a given object to a given document key
	 * Returns whether the operation was valid and successful
	 */
	const bool setDocKey(const UUID & objectId, const BitVector<N> & docKey) {
		if (docToKeyMap.count(objectId) != 0) {
			docToKeyMap[objectId] = docKey;
			return true;
		}
		return false;
	}

	/*
	 * Function: getDocAddressFunction
	 * Returns a serialized random unused document address function L
	 * and inserts the document address function into a stored hash set
	 * Returns existing address function if object has an existing key
	 */
	const BitMatrix<2*N> getDocAddressFunction(const UUID & objectId) {
		BitMatrix<2*N> addressMatrix;
		if (docKeySet.count(objectId) == 0) { //objectId already used
			addressMatrix = generateK();
			while (addressMatrix.docAddressFunctionSet.count(addressMatrix) == 1) addressMatrix = generateK(); //generated new matrix
			docToAddressFunctionMap[objectId] = addressMatrix;
			docAddressFunctionSet.insert(addressMatrix);
		} else addressMatrix = docToAddressFunctionMap[objectId];
		return addressMatrix;
	}

	/*
	 * Function: getAddress
	 * Given a token and a document key, returns the address for the
	 * associated metadatum
	 */
	const BitVector<N> getAddress(const BitVector<N> & token, const UUID & objectId) {
		BitVector<N> docKey = docToKeyMap[objectId];
		BitMatrix<2*N> addressMatrix = docToAddressFunctionMap[objectId];

		return addressMatrix * (BitVector<2*N>::vCat(token, objectId));
	}

private:
	PrivateKey<N, L> _pk;
	BitMatrix<2*N> _K;
	BitMatrix<N> _C;

	unordered_set<BitVector<N> > docKeySet;
	unordered_set<BitMatrix<2*N> > docAddressFunctionSet;
	unordered_map<UUID, BitVector<N> > docToKeyMap;
	unordered_map<UUID, BitMatrix<2*N> > docToAddressFunctionMap;


	/*
     * Function: generateK()
     * Returns a random client-specific n x 2n matrix K_\Omega
     * with 0 bottom left and top right blocks
     * Assumes N is even
     */
	const BitMatrix<2*N> generateK() const{
		const unsigned int half = N >> 1;
		BitMatrix<half> K11 = BitMatrix<half>::randomInvertibleMatrix();
		BitMatrix<half> K12 = BitMatrix<half>::randomInvertibleMatrix();
		BitMatrix<half> K23 = BitMatrix<half>::randomInvertibleMatrix();
		BitMatrix<half> K24 = BitMatrix<half>::randomInvertibleMatrix();
		BitMatrix<N> zero = BitMatrix<N>::zeroMatrix(N << 5);

		BitMatrix<2*N> top = BitMatrix<2*N>::augH(BitMatrix<N>::augH(K11, K12), zero);
		BitMatrix<2*N> bot = BitMatrix<2*N>::augH(zero, BitMatrix<N>::augH(K23, K24));

		return BitMatrix<2*N>::augV(top, bot);
	}

	/*
     * Function: generateHashMatrix()
     * Returns the matrix portion of the hash function
     * Applied to x concatenated with y
     */
	const BitMatrix<4*N> generateHashMatrix() const{
		BitMatrix<2*N> Mi = _pk.getM().inv();
		BitMatrix<2*N> Mi1 = Mi.splitV(0);
		BitMatrix<2*N> Mi2 = Mi.splitV(1);

		BitMatrix<2*N> left = _pk.getB().inv() * Mi1;
		BitMatrix<2*N> right = _pk.getA().inv() * Mi2;
		BitMatrix<2*N> decryptMatrix = left ^ right; //n x 2n

		BitMatrix<2*N> zero = BitMatrix<2*N>::zeroMatrix(N << 6);
		BitMatrix<4*N> top = BitMatrix<2*N>::augH(decryptMatrix, zero);
		BitMatrix<4*N> bot = BitMatrix<2*N>::augH(zero, decryptMatrix);
		return _K * (BitMatrix<2*N>::augV(top, bot));
	}

	/*
     * Function: generateAugmentedF2()
     * Returns the K (f2 C || f2 C) portion of the hash function
     * Applied to concealedF1(x) concatenated with concealedF1(y)
     */
	const MultiQuadTuple<2*N, N> generateAugmentedF2() const{
		MultiQuadTuple<N, N> f2 = _pk.getf().get(1);
		MultiQuadTuple<N, N> topBot = (f2 * _C);
		MultiQuadTuple<2*N, 2*N> augmentedDecrypt = MultiQuadTuple<2*N, 2*N>::augV(topBot, topBot);
		return augmentedDecrypt.rMult(_K);
	}

	/*
     * Function: generateConcealedF1()
     * Returns the f1 C portion of the hash function
     * Applied to x and y separately
     */
	const MultiQuadTuple<2*N, N> generateConcealedF1() const{
		MultiQuadTuple<N, N> f1 = _pk.getf().get(0);

		BitMatrix<2*N> Mi2 = _pk.getM().inv().splitV(1);
		BitMatrix<2*N> inner = _pk.getA().inv() * Mi2;
		return (f1 * inner).rMult(_C.inv());
	}


	/*
	 * Function: generateDocKey
	 * Returns a serialized random unused document key
	 * Returns 0 if object has an existing key
	 */
	const BitVector<N> generateDocKey(const UUID & objectId) const{
		BitVector<N> docKey = BitVector<N>::randomVector();
		if (docToKeyMap.count(objectId) == 0) {
	        docKey = BitVector<N>::randomVector();
		}
		return docKey;
	}
};

#endif/* defined(__krypto__SearchPrivateKey__) */