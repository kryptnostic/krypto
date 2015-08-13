//
//  BridgeKey.h
//  krypto
//
//  Created by Peng Hui How and Robin Zhang on 7/22/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//
//  Implementation of bridge key generation
//  Accessed by clients, used to generate components of the public keys (Refer to implementation.pdf for the notations)
//

#ifndef krypto_BridgeKey_h
#define krypto_BridgeKey_h

#include "test_MultiQuadTuple.h"

/*
 * Template for BridgeKey
 * Plaintext Length: 64N, Length of the obfuscation chain: L
 * L === 2 for now
 */
template<unsigned int N, unsigned int L>
class BridgeKey{
public:

    /*
     * Constructor
     * Constructs a BridgeKey with a given PrivateKey
     * and BitMatrix K (for left-matrix multiplcation)
     */
    BridgeKey() : 
    _M(BitMatrix<2*N>::squareIdentityMatrix()),
    _Ai(BitMatrix<N>::squareIdentityMatrix().inv()),
    _Bi(BitMatrix<N>::squareIdentityMatrix().inv()),
    _Cb2(BitMatrix<3*N>::squareIdentityMatrix())
    {}

/* AND */

    /*
     * Function: getANDz
     * Returns function tuple z used for homomorphic AND
     */
    const MultiQuadTuple<7*N, 2*N> getANDz() const{
        BitMatrix<2*N> X = getANDX();
        BitMatrix<3*N> Y1 = _Bi.pMult(_Cb2, 0, NN-1);
        BitMatrix<3*N> Y2 = _Bi.pMult(_Cb2, NN, 2*NN-1);
        BitMatrix<3*N> Y3 = _Cb2.inv().splitV3(2);
        BitMatrix<7*N> Y3t = BitMatrix<7*N>::augH(BitMatrix<4*N>::zeroMatrix(N << 6), Y3);

        BitMatrix<N> contrib = BitMatrix<N>::augV(getANDP(X, Y2), getANDQ(X, Y1), getANDS(Y1, Y2));
        MultiQuadTuple<7*N, N> zTop(contrib, BitVector<N>::zeroVector());
        MultiQuadTuple<7*N, N> zeroMQT = MultiQuadTuple<7*N, N>::zeroMultiQuadTuple();

        MultiQuadTuple<7*N, 2*N> z = MultiQuadTuple<7*N, 2*N>::augV(zTop, zeroMQT);
        return z.template rMult<2*N>(_M);
    }


private:
    BitMatrix<2*N> _M;
    BitMatrix<3*N> _Cb2;
        
    BitMatrix<N> _Ai;
    BitMatrix<N> _Bi;
    static const unsigned int twoN = N << 1;
    static const unsigned int threeN = 3 * N;
    static const unsigned int NN = N << 6;
    static const unsigned int twoNN = NN << 1;
    static const unsigned int threeNN = 3 * NN;

/*Helper functions for getANDz*/

    /*
     * Function: getANDX
     * Returns matrix X used to compute z for homomorphic AND
     * Dimension of X: (N * 2^6) by 2*(N * 2^6)
     */
    const BitMatrix<2*N> getANDX() const{
        BitMatrix<2*N> inner = BitMatrix<2*N>::augH(BitMatrix<N>::squareIdentityMatrix(), _Ai);
        return _Bi * inner * _M.inv();
    }

    /* 
     * Function: getANDPk
     * top chunk of contrib matrix for z for homomorphic AND
     * level ranges from 0 to 64 * 2N - 1
     * Dimension: 7 * 64N - level
     */
    const BitMatrix<N> getANDPk(const int level, const BitMatrix<2*N> &X, const BitMatrix<3*N> &Y2) const{
        BitMatrix<N> top = BitMatrix<N>::zeroMatrix(twoNN - level); //not sure if there's obob here

        BitMatrix<N> mid = BitMatrix<N>::zeroMatrix(twoNN);
        for (size_t j = 0; j < NN; ++j) { //col within middle block
            if(X.get(j, level)){ 
                for (size_t i = 0; i < twoNN; ++i) { //row within middle block
                    mid.set(i, j, X.get(j, i));
                }               
            }
        }

        BitMatrix<N> bot = BitMatrix<N>::zeroMatrix(threeNN);
        for (int j = 0; j < NN; ++j) { //col within bottom block
            if(X.get(j, level)){
                for (size_t i = 0; i < threeNN; ++i) { //row within bottom block        
                    bot.set(i, j, Y2.get(j, i));
                }               
            }
        }
        return BitMatrix<N>::augV(top, mid, bot);
    }

    /*
     * Function: getANDQk
     * middle chunk of contrib matrix for z
     * level ranges from 0 to 64 * 2N - 1
     * Dimension: 5 * 64N - level
     */
    const BitMatrix<N> getANDQk(const int level, const BitMatrix<2*N>&X, const BitMatrix<3*N> &Y1) const{
        BitMatrix<N> top = BitMatrix<N>::zeroMatrix(twoNN - level);
        BitMatrix<N> bot = BitMatrix<N>::zeroMatrix(threeNN);
        for (size_t j = 0; j < NN; ++j) { //col within bottom block
            if(X.get(j, level)){
                for (size_t i = 0; i < threeNN; ++i) { //row within bottom block
                    bot.set(i, j, Y1.get(j, i));
                }
            }
        }
        return BitMatrix<N>::augV(top, bot);
    }

    /*
     * Function: getANDSk
     * level ranges from 0 to 64 * 3N - 1
     * Dimension: 3 * 64N - level
     */
    const BitMatrix<N> getANDSk(const int level, const BitMatrix<3*N> &Y1, const BitMatrix<3*N> &Y2) const{
        BitMatrix<N> contrib = BitMatrix<N>::zeroMatrix(threeNN - level);
        for (size_t j = 0; j < NN; ++j) { //cols
            bool prod = Y1.get(j, level) && Y2.get(j, level); //first row
            contrib.set(0, j, prod);
            if(Y1.get(j, level)){
                for (size_t i = 1; i < threeNN - level; ++i) { //rows
                    bool prod1 = Y2.get(j, level + i);
                    contrib.set(i, j, Y2.get(j, level + i)); 
                }               
            }
            if(Y2.get(j, level)){
                for(size_t i = 1; i < threeNN - level; ++i){
                    contrib.set(i, j, contrib.get(i, j) ^ Y1.get(j, level + i)); 
                } 
            }           
        }
        return contrib;
    }

    /*
     * Function: getANDP (entire chunk)
     * top chunk of contrib matrix for z for homomorphic AND
     */
    const BitMatrix<N> getANDP(const BitMatrix<2*N> &X, const BitMatrix<3*N> &Y2) const{
        BitMatrix<N> contrib = BitMatrix<N>::zeroMatrix(NN*(12*NN + 1));
        unsigned int count = 0;
        unsigned int toAdd = 7 * NN;
        for(size_t level = 0; level < twoNN; ++level){
            contrib.setSubMatrix(count, getANDPk(level, X, Y2));
            count += toAdd;
            --toAdd;
        }
        return contrib;
    }

    /*
     * Function: getANDQ (entire chunk)
     * middle chunk of contrib matrix for z for homomorphic AND
     */
    const BitMatrix<N> getANDQ(const BitMatrix<2*N> &X, const BitMatrix<3*N> &Y1) const{
        BitMatrix<N> contrib = BitMatrix<N>::zeroMatrix(NN*(8*NN + 1));
        unsigned int count = 0;
        unsigned int toAdd = 5 * NN;
        for(size_t level = 0; level < twoNN; ++level){
            contrib.setSubMatrix(count, getANDQk(level, X, Y1));
            count += toAdd;
            --toAdd;
        }
        return contrib;     
    }

    /*
     * Function: getANDS (entire chunk)
     * bottom chunk of contrib matrix for z
     */ 
    const BitMatrix<N> getANDS(const BitMatrix<3*N> &Y1, const BitMatrix<3*N> &Y2) const{
        BitMatrix<N> contrib = BitMatrix<N>::zeroMatrix((3*NN*(3*NN + 1)) >> 1);
        unsigned int count = 0;
        unsigned int toAdd = 3 * NN;
        for(size_t level = 0; level < threeNN; ++level){
            contrib.setSubMatrix(count, getANDSk(level, Y1, Y2));
            count += toAdd;
            --toAdd;
        }       
        return contrib;
    }   
};

#endif