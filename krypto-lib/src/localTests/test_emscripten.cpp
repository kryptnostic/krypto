#include <iostream>

using namespace std;


#define DEBUG false
#define TESTRUNS 1

#define numBits 64


/*
 * Function: get(n)
 * Returns the value of the bit at a given index
 */
bool get(unsigned long long x, unsigned int n) {
    return (x & (1ul << (n & 63ul))) != 0;
}

/*
 * Function: set(n)
 * Sets the bit at a given index to 1
 */
void set(unsigned long long &x, unsigned int n) {
    x |= (1ul << (n & 63ul));
}

/*
 * Function: print()
 * Prints the values of the current BitVector
 * Ex. [1, 0, 0, 1, 0, 1, 1, 0]
 */
void print(unsigned long long x) {
    std::cout << "[";
    for (int i = 0; i < numBits - 1; ++i) {
        std::cout << get(x, i) << ", ";
    }
    std::cout << get(x, numBits - 1) << "]" << std::endl;
}

int main(int argc, char **argv) {

    unsigned long long x = 0ull;
    set(x, 5);
    print(x);

    return 0;
}