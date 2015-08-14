//
//  main.cpp
//  krypto-cli
//
//  Created by Matthew Tamayo on 12/11/14.
//  Copyright (c) 2014 Kryptnostic. All rights reserved.
//

#include <iostream>
#include <chrono>
#include "../../../contrib/gtest/gtest.h"

using namespace testing;

#define NUM_LONGS 2
int main(int argc, char * argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    // insert code here...
/*
    std::cout << "Hello, World!\n";
    BitVector<NUM_LONGS> x = BitVector<NUM_LONGS>::randomVector();
    BitVector<NUM_LONGS> zero;

    MultivariatePolynomialFunction<NUM_LONGS,NUM_LONGS> f = move( MultivariatePolynomialFunction<NUM_LONGS,NUM_LONGS>::denseRandomMultivariateFunctions() );

    BitVector<NUM_LONGS> y = f(x);
     
    x.print();
    if( !(zero ==  y) ) {
        y.print();
    }
    const int numInputs = 100000;
    BitVector<NUM_LONGS> inputs[ numInputs ];
    for(int i = 0 ; i < numInputs; ++i ) {
        inputs[ i ] = BitVector<NUM_LONGS>::randomVector();
    }
    
    std::cout<<"Starting benchmark!"<<std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0 ; i < numInputs; ++i ) {
        f( inputs[ i ] );
    }
    
    auto stop = std::chrono::high_resolution_clock::now();
    
    auto total =std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();

    std::cout<<"Total time: "<< total <<" nanos"<<std::endl;
    std::cout<<"Mean time: "<< total/double(numInputs) <<" nanos"<<std::endl;
    */
    return 0;
}
