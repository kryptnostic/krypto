  # the compiler: gcc for C program, define as g++ for C++
  CC = g++

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
  CFLAGS  = -g -Wall --std=c++14 -O3

  DEPS = ../main/cpp/BitMatrix.h ../main/cpp/BitVector.h ../main/cpp/PrivateKey.h ../main/cpp/BridgeKey.h ../main/cpp/PublicKey.h ../main/cpp/MultiQuadTuple.h ../main/cpp/MultiQuadTupleChain.h

  # the build target executable:
  TARGET1 = test_speed
  TARGET2 = test_local

  all: $(TARGET1) $(TARGET2)

  $(TARGET1): $(TARGET1).cpp $(DEPS)
	$(CC) $(CFLAGS) -o $(TARGET1) $(TARGET1).cpp

  $(TARGET2): $(TARGET2).cpp $(DEPS)
	$(CC) $(CFLAGS) -o $(TARGET2) $(TARGET2).cpp

  clean:
	$(RM) $(TARGET1) $(TARGET2)