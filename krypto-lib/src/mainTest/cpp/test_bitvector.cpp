#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/BitVector.h"

using namespace testing;

TEST(BitVectorTests, test_smallrandom){
    BitVector<64> bv = BitVector<64>::randomSmallVector();
    ASSERT_TRUE(!bv.get(0));
}

TEST(BitVectorTests, test_clear) {
    BitVector<64> bv = BitVector<64>::randomVector();

    while( bv.isZero() ) {
        bv = BitVector<64>::randomVector();
    }

    ASSERT_FALSE( bv.isZero() );

    bv.zero();

    ASSERT_TRUE( bv.isZero() );
}

TEST(BitVectorTests, test_equals) {
    BitVector<64> bv = BitVector<64>::randomVector();
    BitVector<64> bv2 = BitVector<64>::randomVector();
    BitVector<64> bv1 = bv;

    //Make sure that equals isn't totally busted
    ASSERT_TRUE(bv == bv);

    while( bv2 == bv ) {
        bv2 = BitVector<64>::randomVector();
    }

    //Make sure the copy assignment makes a deep copy.
    ASSERT_TRUE( bv.elements() != bv1.elements() );
    ASSERT_TRUE(bv != bv2 );
    ASSERT_TRUE(bv1 != bv2 );
    ASSERT_TRUE(bv == bv1 );
}

TEST(BitVectorTests, test_zero) {
    BitVector<64> v = BitVector<64>::zeroVector();
    ASSERT_TRUE(v.isZero());
}

TEST(BitVectorTests, test_and) {
    BitVector<64> v1 = BitVector<64>::zeroVector();
    BitVector<64> v2 = BitVector<64>::randomVector();
    BitVector<64> v3 = BitVector<64>::randomVector();
    ASSERT_TRUE( (v1&v2).isZero() );

    ASSERT_EQ( (v2&v3).elements()[0] , ((v2.elements())[0]&(v3.elements()[0])) );
}

TEST(BitVectorTests, test_xor) {
    BitVector<64> v1 = BitVector<64>::zeroVector();
    BitVector<64> v2 = BitVector<64>::randomVector();
    BitVector<64> v3 = BitVector<64>::randomVector();
    ASSERT_EQ(v2, v1^v2);

    ASSERT_TRUE( (v2^v3).elements()[0] == (v2.elements()[0]^v3.elements()[0]) );
}

TEST(BitVectorTests, test_or) {
    BitVector<64> v1 = BitVector<64>::zeroVector();
    BitVector<64> v2 = BitVector<64>::randomVector();
    BitVector<64> v3 = BitVector<64>::randomVector();
    ASSERT_EQ(v2, v1|v2);

    ASSERT_TRUE( (v2|v3).elements()[0] == (v2.elements()[0]|v3.elements()[0]) );
}

TEST(BitVectorTests, test_parity) {
    BitVector<64> v = BitVector<64>::zeroVector();
    ASSERT_FALSE(v.parity());

    v.elements()[ 0 ] = 0x1;
    ASSERT_TRUE( v.parity() );

    v.elements()[ 0 ] = 0xFF;
    ASSERT_FALSE( v.parity() );
}

TEST(BitVectorTests, test_shift) {
    BitVector<64> v = BitVector<64>::randomSmallVector();
    BitVector<64> w = v.leftShift(1);
    ASSERT_FALSE(w.get(63));
    bool shift = true;
    for(int i = 0; i < 63; ++i){
        if(v.get(i+1) != w.get(i)){
            shift = false;
            break;
        }
    }
    ASSERT_TRUE(shift);
}

TEST(BitVectorTests, test_add) {
    BitVector<64> x = BitVector<64>::randomSmallVector();
    BitVector<64> y = BitVector<64>::randomSmallVector();
    std::cout << "x: " << endl;
    x.print();
    std::cout << "y: " << endl;
    y.print();
    std::cout << "x + y: " << endl;
    BitVector<64> sum = x + y;
    sum.print(); 
    ASSERT_TRUE(true);
}

TEST(BitVectorTests, test_mult) {
    BitVector<64> x = BitVector<64>::zeroVector();
    BitVector<64> y = BitVector<64>::zeroVector();
    x.set(63); x.set(60);
    y.set(62); y.set(61);
    std::cout << "x: " << endl;
    x.print();
    std::cout << "y: " << endl;
    y.print();
    std::cout << "x * y: " << endl;
    BitVector<64> sum = x * y;
    sum.print(); 
    ASSERT_TRUE(true);
}