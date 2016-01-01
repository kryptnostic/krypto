  # the compiler: gcc for C program, g++ for C++
  CC = g++

  # compiler flags:
  #  -g                           adds debugging information to the executable file
  #  -Wall                        enables most, but not all, compiler warnings
  #  --std=c++11                  enables C++11 functionality
  #  -fslp-vectorize-aggressive   enables SLP Vectorizer which merges multiple scalars into vectors
  #  -march=native                enables GCC optimization for local CPU instruction sets
  #  -ftemplate-depth=8192        increases the maximum instantiation depth for template classes from 900 to 8192
  #  -I$                          includes libraries
  CFLAGS  = -g -Wall --std=c++11 -O3 -fslp-vectorize-aggressive -march=native -ftemplate-depth=8192 -I$(JAVA_HOME)/include/ -I$(JAVA_HOME)/include/darwin/ -I$(JAVA_HOME)/include/linux/

  # dependencies
  DEPS1 = ../main/cpp/BitMatrix.h ../main/cpp/BitVector.h ../main/cpp/MultiQuadTuple.h ../main/cpp/MultiQuadTupleChain.h ../main/cpp/PrivateKey.h ../main/cpp/BridgeKey.h ../main/cpp/PublicKey.h
  DEPS2 = ../main/cpp/ClientHashFunction.h ../main/cpp/SearchPrivateKey.h ../main/cpp/KryptnosticClient.h ../main/cpp/KryptnosticServer.h

  # the build target executables:
  TARGET0 = test_speed_temp
  TARGET1 = test_speed
  TARGET2 = test_local
  #TARGET3 = test_KryptnosticClient
  TARGET4 = test_mqt
  #TARGET5 = test_server
  TARGET6 = test_search

  all: $(TARGET0)#$(TARGET1) $(TARGET2) $(TARGET4) $(TARGET6)

  $(TARGET0): $(TARGET1).cpp $(DEPS1)
	$(CC) $(CFLAGS) -o $(TARGET0) $(TARGET1).cpp

  $(TARGET2): $(TARGET2).cpp $(DEPS1)
	$(CC) $(CFLAGS) -o $(TARGET2) $(TARGET2).cpp

  #$(TARGET3): $(TARGET3).cpp $(DEPS1) $(DEPS2)
	#$(CC) $(CFLAGS) -o $(TARGET3) $(TARGET3).cpp

  $(TARGET4): $(TARGET4).cpp $(DEPS1) $(DEPS2)
	$(CC) $(CFLAGS) -o $(TARGET4) $(TARGET4).cpp

  #$(TARGET5): $(TARGET5).cpp $(DEPS1) $(DEPS2)
	#$(CC) $(CFLAGS) -o $(TARGET5) $(TARGET5).cpp

  $(TARGET6): $(TARGET6).cpp $(DEPS1) $(DEPS2)
	$(CC) $(CFLAGS) -o $(TARGET6) $(TARGET6).cpp

  clean:
	$(RM) $(TARGET1) $(TARGET2) $(TARGET4) $(TARGET6)
