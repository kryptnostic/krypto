#include "../main/cpp/KryptnosticJNIMapper.h"
#include <iostream>

using namespace std;

#define N 128

void testJNI() {
	JNIEnv env;
	BitVector<N> v;
	jbyteArray jVector = convertBitVectorToJByteArray(&env, v);
}


int main(int argc, char **argv) {
	cout << "JNI tests" << endl;
	testJNI();
	return 0;
}