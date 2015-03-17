################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/mainTest/cpp/test_bitmatrix.cpp \
../src/mainTest/cpp/test_bitvector.cpp \
../src/mainTest/cpp/test_main.cpp \
../src/mainTest/cpp/test_trivial.cpp 

OBJS += \
./src/mainTest/cpp/test_bitmatrix.o \
./src/mainTest/cpp/test_bitvector.o \
./src/mainTest/cpp/test_main.o \
./src/mainTest/cpp/test_trivial.o 

CPP_DEPS += \
./src/mainTest/cpp/test_bitmatrix.d \
./src/mainTest/cpp/test_bitvector.d \
./src/mainTest/cpp/test_main.d \
./src/mainTest/cpp/test_trivial.d 


# Each subdirectory must supply rules for building sources it contributes
src/mainTest/cpp/%.o: ../src/mainTest/cpp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


