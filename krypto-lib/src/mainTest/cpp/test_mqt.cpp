#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/MultiQuadTuple.h"
#include <string>
#include <ctime>
#include <chrono>
using namespace testing;

#define big 256
#define mid 128
#define sma 64
#define mon ((sma*(sma + 1)) >> 1)

TEST(MQTTests, testLeftCompose){
    BitVector<sma> v = BitVector<sma>::randomVector();
    MultiQuadTuple<sma,sma> cs;
    cs.randomize();

    BitMatrix<sma> m = BitMatrix<sma>::randomMatrix();

    std::cout<<"Starting left compose"<<endl;
    auto start = std::chrono::high_resolution_clock::now();

    MultiQuadTuple<sma, sma> csm = cs * m;//cs(m);

    auto stop = std::chrono::high_resolution_clock::now();
    auto total =std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();
    std::cout<<"Total time: "<< total <<" nanos"<<std::endl;
    std::cout<<"Mean time: "<< total/double(sma) <<" nanos"<<std::endl;

   // std::cout<<"size of CS: "<< sizeof( MultiQuadTuple<mid,mid> ) <<endl;

    BitVector<sma> mv = m * v;

    BitVector<sma> csm_v = csm(v);
    BitVector<sma> cs_mv = cs(mv);
    ASSERT_TRUE(csm_v.equals(cs_mv));
}

TEST(MQTTests, testLeftComposeDiffDims){
    BitMatrix<sma,mid> m = BitMatrix<sma, mid>::randomMatrix();
    MultiQuadTuple<sma,sma> cs;
    cs.randomize();
    MultiQuadTuple<mid,sma> csm = cs * m;
    BitVector<mid> v = BitVector<mid>::randomVector();
    BitVector<sma> mv = m * v;
    cout << "Left Compose with Different Dimensions" << endl;

    BitVector<sma> csm_v = csm(v);
    BitVector<sma> cs_mv = cs(mv);
    ASSERT_TRUE(csm_v.equals(cs_mv));
}

TEST(MQTTests, testRightCompose){
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

    BitVector<sma> m_csv = m * cs(v);
    BitVector<sma> mcs_v = mcs(v);
    ASSERT_TRUE(m_csv.equals(mcs_v));
}

TEST(MQTTests, testAugV){
    BitVector<sma> v = BitVector<sma>::randomVector();
    MultiQuadTuple<sma,sma> cs1;
    cs1.randomize();
    MultiQuadTuple<sma,sma> cs2;
    cs2.randomize();
    MultiQuadTuple<sma,sma> cs3;
    cs3.randomize();
    MultiQuadTuple<sma,3*sma> cs;
    cs.augV(cs1, cs2, cs3);

    BitVector<3*sma> cs_v = cs(v);
    BitVector<3*sma> aug = BitVector<3*sma>::vCat(cs1(v), cs2(v), cs3(v));
    ASSERT_TRUE(cs_v.equals(aug));
}

TEST(MQTTests, testSetAsMatrix){
    MultiQuadTuple<sma,sma> cs;
    BitMatrix<sma,sma> m = BitMatrix<sma,sma>::randomMatrix();
    BitVector<sma> v = BitVector<sma>::randomVector();
    BitVector<sma> mv = m * v;
    cs.setAsMatrix(m);
    BitVector<sma> csv = cs(v);
    ASSERT_TRUE(mv.equals(csv));
}

TEST(MQTTests, testSetMQT){
    MultiQuadTuple<sma,sma> cs;
    BitMatrix<mon,sma> m = BitMatrix<mon,sma>::randomMatrix();
    BitVector<sma> v = BitVector<sma>::randomVector();
    cs.setContributions(m, v);
    BitVector<sma> x = BitVector<sma>::randomVector();
    cout << "Emulating constructor" << endl;
    cs(x).print();
}

TEST(MQTTests, testPartialEval){
    MultiQuadTuple<2*sma,sma> f;
    f.randomize();
    BitVector<sma> x = BitVector<sma>::randomVector();
    BitVector<sma> y = BitVector<sma>::randomVector();
    BitVector<2*sma> z = BitVector<2*sma>::vCat(x, y);

    MultiQuadTuple<sma, sma> g = f.partialEval<sma>(x);

    cout << "g(y): " << endl;
    g(y).print();

    cout << "f(x, y): " << endl;
    f(z).print();

    //ASSERT_TRUE(g(y).equals(f(z)));
}

/*
TODO: implement this with the recursive struct
TEST(MQTTests, testShifter){
	ASSERT_TRUE(34+55 == 89);
	MultiQuadTuple<N, M> f = MultiQuadTuple<N, M>::randomMultiQuadTuple();
	BitVector<N> v = BitVector<N>::randomVectorLeadingZeroes(2);
	BitVector<N> w = v.leftShift(1);
	BitVector<N> fv = f(v);
	MultiQuadTuple<N, M> g = f.leftShift();
	BitVector<N> gw = g(w);
	ASSERT_TRUE(fv.equals(gw));
	ASSERT_TRUE(55+89 == 144);
}
*/