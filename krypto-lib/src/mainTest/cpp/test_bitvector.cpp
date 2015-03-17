#include "../../../contrib/gtest/gtest.h"
#include "../../main/cpp/BitVector.h"

using namespace testing;

TEST(BitVectorTests, test_random) {
	BitVector<1> bv = BitVector<1>::randomVector();
	ASSERT_TRUE(bv.length() == 64);
}

TEST(BitVectorTests, test_clear) {
	BitVector<1> bv = BitVector<1>::randomVector();
    
    while( bv.isZero() ) {
        bv = BitVector<1>::randomVector();
    }
    
    ASSERT_FALSE( bv.isZero() );

    bv.zero();
    
    ASSERT_TRUE( bv.isZero() );
}

TEST(BitVectorTests, test_equals) {
	BitVector<1> bv = BitVector<1>::randomVector();
	BitVector<1> bv2 = BitVector<1>::randomVector();
    BitVector<1> bv1 = bv;

    //Make sure that equals isn't totally busted
    ASSERT_TRUE(bv == bv);

    while( bv2 == bv ) {
        bv2 = BitVector<1>::randomVector();
    }
    
    //Make sure the copy assignment makes a deep copy.
    ASSERT_TRUE( bv.elements() != bv1.elements() );
    

	bv.print();
    bv1.print();
	bv2.print();


    ASSERT_TRUE(bv != bv2 );
    ASSERT_TRUE(bv1 != bv2 );
    ASSERT_TRUE(bv == bv1 );
}

TEST(BitVectorTests, test_zero) {
    BitVector<1> v;
    ASSERT_TRUE(v.isZero());
}

TEST(BitVectorTests, test_and) {
    BitVector<1> v1;
    BitVector<1> v2 = BitVector<1>::randomVector();
    BitVector<1> v3 = BitVector<1>::randomVector();
    ASSERT_TRUE( (v1&v2).isZero() );

    ASSERT_EQ( (v2&v3).elements()[0] , ((v2.elements())[0]&(v3.elements()[0])) );
}

TEST(BitVectorTests, test_xor) {
    BitVector<1> v1;
    BitVector<1> v2 = BitVector<1>::randomVector();
    BitVector<1> v3 = BitVector<1>::randomVector();
    ASSERT_EQ(v2, v1^v2);
    
    ASSERT_TRUE( (v2^v3).elements()[0] == (v2.elements()[0]^v3.elements()[0]) );
}

TEST(BitVectorTests, test_or) {
    BitVector<1> v1;
    BitVector<1> v2 = BitVector<1>::randomVector();
    BitVector<1> v3 = BitVector<1>::randomVector();
    ASSERT_EQ(v2, v1|v2);
    
    ASSERT_TRUE( (v2|v3).elements()[0] == (v2.elements()[0]|v3.elements()[0]) );
}

TEST(BitVectorTests, test_parity) {
    BitVector<1> v;
    ASSERT_FALSE(v.parity());
    
    v.elements()[ 0 ] = 0x1;
    ASSERT_TRUE( v.parity() );
    
    v.elements()[ 0 ] = 0xFF;
    ASSERT_FALSE( v.parity() );
}
