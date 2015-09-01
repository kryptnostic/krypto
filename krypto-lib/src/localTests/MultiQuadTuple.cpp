//
//  MultiQuadTuple.cpp
//  krypto
//
//  Created by Matthew Tamayo-Rios on 8/14/15.
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//


#include "../main/cpp/MultiQuadTuple.h"
#include <chrono>

#define hug 448
#define big 256
#define mid 128
#define sma 64
#define mon ((sma*(sma + 1)) >> 1)

void testLeftCompose(){
    BitVector<sma> v = BitVector<sma>::randomVector();
    MultiQuadTuple<sma,sma> cs;
    cs.randomize();

    BitMatrix<sma> m = BitMatrix<sma>::randomMatrix();

    std::cout<<"Starting left compose"<<endl;
    auto start = std::chrono::high_resolution_clock::now();

    MultiQuadTuple<sma, sma> csm = cs * m;

    auto stop = std::chrono::high_resolution_clock::now();
    auto total =std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();
    std::cout<<"Total time: "<< total <<" nanos"<<std::endl;
    std::cout<<"Mean time: "<< total/double(sma) <<" nanos"<<std::endl;

   // std::cout<<"size of CS: "<< sizeof( MultiQuadTuple<mid,mid> ) <<endl;

    BitVector<sma> mv = m * v;//m.template operator*<sma>(v);
    csm(v).print();
    cs(mv).print();
}

void testLeftComposeDiffDim(){
    BitMatrix<sma,mid> m = BitMatrix<sma, mid>::randomMatrix();
    MultiQuadTuple<sma,sma> cs;
    cs.randomize();
    MultiQuadTuple<mid,sma> csm = cs * m;
    BitVector<mid> v = BitVector<mid>::randomVector();
    BitVector<sma> mv = m * v;//m.template operator*<sma>(v);
    cout << "Left Compose with Different Dimensions" << endl;
    csm(v).print();
    cs(mv).print();
}

void testRightCompose(){
    BitVector<sma> v = BitVector<sma>::randomVector();
    MultiQuadTuple<sma,sma> cs;
    cs.randomize();
    BitMatrix<sma> m = BitMatrix<sma>::randomMatrix();
    std::cout<<"Starting right compose"<<endl;
    auto start = std::chrono::high_resolution_clock::now();

    MultiQuadTuple<sma, sma> mcs = cs.template rMult<sma>(m);//cs * m;

    auto stop = std::chrono::high_resolution_clock::now();
    auto total =std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();
    std::cout<<"Total time: "<< total <<" nanos"<<std::endl;
    std::cout<<"Mean time: "<< total/double(sma) <<" nanos"<<std::endl;

    BitVector<sma> m_csv = m * cs(v);//m.template operator*<sma>(cs(v));
    BitVector<sma> mcs_v = mcs(v);
    m_csv.print();
    mcs_v.print();
}

void testAugV(){
    BitVector<sma> v = BitVector<sma>::randomVector();
    MultiQuadTuple<sma,sma> cs1;
    cs1.randomize();
    MultiQuadTuple<sma,sma> cs2;
    cs2.randomize();
    MultiQuadTuple<sma,sma> cs3;
    cs3.randomize();
    MultiQuadTuple<sma,3*sma> cs;
    cs.augV(cs1, cs2, cs3);
    cout << "AugV:" << endl;
    cs1(v).print();
    cs2(v).print();
    cs3(v).print();
    cs(v).print();
}

void testSetAsMatrix(){
    MultiQuadTuple<sma,sma> cs;
    BitMatrix<sma,sma> m = BitMatrix<sma,sma>::randomMatrix();
    BitVector<sma> v = BitVector<sma>::randomVector();
    BitVector<sma> mv = m * v;//m.template operator*<sma>(v);
    cs.setAsMatrix(m);
    BitVector<sma> csv = cs(v);
    cout << "Matrix => MQT" << endl;
    mv.print();
    csv.print();
}

void testSetAsMatrixHuge(){
    MultiQuadTuple<hug,sma> cs;
    BitMatrix<sma,hug> m = BitMatrix<sma,hug>::randomMatrix();
    BitVector<hug> v = BitVector<hug>::randomVector();
    BitVector<sma> mv = m * v;//m.template operator*<sma>(v);
    cs.setAsMatrix(m);
    BitVector<sma> csv = cs(v);
    cout << "Huge Matrix => MQT" << endl;
    mv.print();
    csv.print();
}

void testSetMQT(){
    MultiQuadTuple<sma,sma> cs;
    BitMatrix<mon,sma> m = BitMatrix<mon,sma>::randomMatrix();
    BitVector<sma> v = BitVector<sma>::randomVector();
    cs.setContributions(m, v);
    BitVector<sma> x = BitVector<sma>::randomVector();
    cout << "Emulating constructor" << endl;
    cs(x).print();
}

int main() {
    testLeftCompose();
    testLeftComposeDiffDim();
    testRightCompose();
    testAugV();
    testSetAsMatrix();
    testSetAsMatrixHuge();
    testSetMQT();
    return 0;
}