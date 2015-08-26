  # the compiler: gcc for C program, define as g++ for C++
  CC = g++

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
  CFLAGS  = -g -Wall --std=c++14 -O3 -fslp-vectorize-aggressive -march=native -ftemplate-depth=8192 -I$(JAVA_HOME)/include/ -I$(JAVA_HOME)/include/darwin/ -I$(JAVA_HOME)/include/linux/

  DEPS1 = ../main/cpp/BitMatrix.h ../main/cpp/BitVector.h ../main/cpp/MultiQuadTuple.h ../main/cpp/MultiQuadTupleChain.h ../main/cpp/PrivateKey.h ../main/cpp/BridgeKey.h ../main/cpp/PublicKey.h
  DEPS2 = ../main/cpp/SearchPrivateKey.h ../main/cpp/KryptnosticClient.h

  # the build target executable:
  TARGET1 = test_speed
  TARGET2 = test_local
  TARGET3 = test_KryptnosticClient

  all: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)

  $(TARGET1): $(TARGET1).cpp $(DEPS1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(TARGET1).cpp

  $(TARGET2): $(TARGET2).cpp $(DEPS1)
	$(CC) $(CFLAGS) -o $(TARGET2) $(TARGET2).cpp

  $(TARGET3): $(TARGET3).cpp $(DEPS1) $(DEPS2)
	$(CC) $(CFLAGS) -o $(TARGET3) $(TARGET3).cpp

  clean:
	$(RM) $(TARGET1) $(TARGET2) $(TARGET3)
