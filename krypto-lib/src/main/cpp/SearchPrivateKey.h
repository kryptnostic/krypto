//
//  SearchPrivateKey.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 8/13/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  C++ implementation of the SearchPrivateKey
//  which generates all of the necessary client-side parts
//  for indexing and searching objects
//
//  Notations: key_{user} and key_{doc} denote a key
//  specific to a user and a document respectively.
//  They generally match with the notations in the search spec dodumentation:
//  http://github.com/kryptnostic/publications/search_spec/search_spec.tex
//

#ifndef krypto_SearchPrivateKey_h
#define krypto_SearchPrivateKey_h

#include <utility>
#include "PrivateKey.h"
#include "ClientHashFunction.h"

//N should be a multiple of 64, otherwise BitVector would create
//the incorrect number of unsigned long longs
template<unsigned int N = 128>
class SearchPrivateKey{
public:
  SearchPrivateKey() :
  _K(BitMatrix<N>::randomInvertibleMatrix()),
  _R(BitMatrix<N>::randomInvertibleMatrix())
  {}

  /* Getters */

  /*
   * Function: getObjectSearchKey()
   * Returns a random object search key to be serialized
   * Equation: d_{doc}
   */
  const BitVector<N> getObjectSearchKey() const {
    return BitVector<N>::randomVector();
  }

  /*
   * Function: getObjectAddressMatrix()
   * Returns a random object address matrix L_i
   * Equation: L_{doc}
   */
  const BitMatrix<N> getObjectAddressMatrix() const {
    return BitMatrix<N>::randomInvertibleMatrix();
  }

  /*
   * Function: getObjectIndexPair
   * Returns a {objectSearchKey, objectAddressMatrix} pair
   */
  const std::pair<BitVector<N>, BitMatrix<N>> getObjectIndexPair() const {
    return std::make_pair(getObjectSearchKey(), getObjectAddressMatrix());
  }

  /*
   * Function: getObjectConversionMatrix(objectAddressMatrix)
   * Returns object(document) conversion matrix given object address function
   * Equation: L_{doc} * K_{user}^{-1}
   */
  const BitMatrix<N> getObjectConversionMatrix(const BitMatrix<N> & objectAddressMatrix) const {
    return objectAddressMatrix * _K.inv();
  }

  /*
   * TODO: rename to getMetadataAddress
   * Function: getMetadataAddress(objectIndexPair, token)
   * Returns the address for metadatum given raw unencrypted data
   * Equation: L_{doc} * [I | R_{user}] * (t || d_{doc})
   */
  const BitVector<N> getMetadataAddress(const std::pair<BitVector<N>, BitMatrix<N>> & objectIndexPair, const BitVector<N> &token) const {
    return objectIndexPair.second * (token ^ (_R * objectIndexPair.first));
  }

  /*
   * Function: getMetadataAddressFromPair(token, objectSearchPair, privateKey)
   * Test function for computing metdataum address given server-side objects
   */
  const BitVector<N> getMetadataAddressFromPair(const BitVector<N> &token, const std::pair<BitVector<2*N>, BitMatrix<N>> & objectSearchPair, const PrivateKey<N> & pk) const {
    return objectSearchPair.second * _K * (token ^ (_R * pk.decrypt(objectSearchPair.first)));
  }

  /*
   * Function: getClientHashFunction(pk)
   * Given a private key, generates a client hash function with a random matrix C
   */
  const ClientHashFunction<N> getClientHashFunction(const PrivateKey<N> & pk) const {
    ClientHashFunction<N> h;
    h.initialize(BitMatrix<N>::randomInvertibleMatrix(), _K * BitMatrix<N, 2*N>::augH(BitMatrix<N>::identityMatrix(), _R), pk);
    return h;
  }

  /*
   * Function: getObjectSearchPair(objectIndexPair, pk)
   * Generates the (encrypted object search key, object conversion matrix) pair to be stored on server during indexing
   * Equation: {E_{user}(d_{doc}), L_{doc} * K_{user}^{-1}}
   */
  const std::pair<BitVector<2*N>, BitMatrix<N>> getObjectSearchPairFromObjectIndexPair(const std::pair<BitVector<N>, BitMatrix<N>> & objectIndexPair, const PrivateKey<N> & pk) const {
    const BitVector<2*N> encryptedObjectSearchKey = pk.encrypt(objectIndexPair.first); //FHE-encrypted
    const BitMatrix<N> objectConversionMatrix = getObjectConversionMatrix(objectIndexPair.second); //L_{object}K_{user}^{-1}
    return std::make_pair(encryptedObjectSearchKey, objectConversionMatrix);
  }

  /*
   * Function: getObjectIndexPairFromObjectSearchPair(objectSearchPair, pk)
   * Equation: {d_{doc}, L_{doc}} = {D_{user}(objectSearchPair_1), objectSearchPair_2 * K_{user}}
   */
  const std::pair<BitVector<N>, BitMatrix<N>> getObjectIndexPairFromObjectSearchPair(const std::pair<BitVector<2*N>, BitMatrix<N>> & objectSearchPair, const PrivateKey<N> & pk) const {
    return std::make_pair(pk.decrypt(objectSearchPair.first), objectSearchPair.second * _K);
  }

  /*
   * Function: getObjectSharePairFromObjectSearchPair(objectSearchPair, pk)
   * Generates the pair to be shared with another client
   * Note: Server encrypts this with sharing destination's RSA public key
   * Equation: {R_{user} * d_{doc}, L_{doc}}
   */
  const std::pair<BitVector<N>, BitMatrix<N>> getObjectSharePairFromObjectSearchPair(const std::pair<BitVector<2*N>, BitMatrix<N>> & objectSearchPair, const PrivateKey<N> & pk) const {
    return std::make_pair(_R * pk.decrypt(objectSearchPair.first), objectSearchPair.second * _K);
  }

  /*
   * Function: getObjectSearchPairFromObjectSharePair(objectSharePair, pk)
   * Generates the (encrypted object search key, object conversion matrix) pair to be stored on server during indexing from sharing
   * Equation: {R_{recipient}^{-1} * R_{sender} * d_{doc}, L_{doc} * K_{recipient}^{-1}}
   */
  const std::pair<BitVector<2*N>, BitMatrix<N>> getObjectSearchPairFromObjectSharePair(const std::pair<BitVector<N>, BitMatrix<N>> & objectSharePair, const PrivateKey<N> & pk) const {
    return std::make_pair(pk.encrypt(_R.solve(objectSharePair.first)), getObjectConversionMatrix(objectSharePair.second));
  }

private:
  BitMatrix<N> _K; // user-specific front protection
  BitMatrix<N> _R; // user-specific hash auxiliary matrix
};

#endif/* defined(__krypto__SearchPrivateKey__) */
