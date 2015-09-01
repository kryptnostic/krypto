################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/mainTest/cpp/subdir.mk
-include contrib/gtest/subdir.mk
-include build/objs/mainTestGoogleTestExe/release/mainTestCpp/ecg6g94siuajgz1c8eksmlrsi/subdir.mk
-include build/objs/mainTestGoogleTestExe/release/mainTestCpp/d8ll8xldwvr59aeshpagre89b/subdir.mk
-include build/objs/mainTestGoogleTestExe/release/mainTestCpp/akuyy4rnhlv4u0gzzxlw4wp24/subdir.mk
-include build/objs/mainTestGoogleTestExe/release/mainTestCpp/2u8w5kf0693qq9rj4p5ahr17g/subdir.mk
-include build/objs/mainTestGoogleTestExe/release/mainCpp/7skz2xkzjx8gqmzc7i6pq87en/subdir.mk
-include build/objs/mainTestGoogleTestExe/release/mainCpp/5oux44dogwvhyg2l9gzatzmh3/subdir.mk
-include build/objs/mainTestGoogleTestExe/release/mainCpp/1n4vdz0oa9ive5y5ifrt07s69/subdir.mk
-include build/objs/mainTestGoogleTestExe/failing/mainTestCpp/ecg6g94siuajgz1c8eksmlrsi/subdir.mk
-include build/objs/mainTestGoogleTestExe/failing/mainTestCpp/d8ll8xldwvr59aeshpagre89b/subdir.mk
-include build/objs/mainTestGoogleTestExe/failing/mainTestCpp/akuyy4rnhlv4u0gzzxlw4wp24/subdir.mk
-include build/objs/mainTestGoogleTestExe/failing/mainTestCpp/2u8w5kf0693qq9rj4p5ahr17g/subdir.mk
-include build/objs/mainTestGoogleTestExe/failing/mainCpp/7skz2xkzjx8gqmzc7i6pq87en/subdir.mk
-include build/objs/mainTestGoogleTestExe/failing/mainCpp/5oux44dogwvhyg2l9gzatzmh3/subdir.mk
-include build/objs/mainTestGoogleTestExe/failing/mainCpp/1n4vdz0oa9ive5y5ifrt07s69/subdir.mk
-include build/objs/mainTestGoogleTestExe/debug/mainTestCpp/ecg6g94siuajgz1c8eksmlrsi/subdir.mk
-include build/objs/mainTestGoogleTestExe/debug/mainTestCpp/d8ll8xldwvr59aeshpagre89b/subdir.mk
-include build/objs/mainTestGoogleTestExe/debug/mainTestCpp/akuyy4rnhlv4u0gzzxlw4wp24/subdir.mk
-include build/objs/mainTestGoogleTestExe/debug/mainTestCpp/2u8w5kf0693qq9rj4p5ahr17g/subdir.mk
-include build/objs/mainTestGoogleTestExe/debug/mainCpp/7skz2xkzjx8gqmzc7i6pq87en/subdir.mk
-include build/objs/mainTestGoogleTestExe/debug/mainCpp/5oux44dogwvhyg2l9gzatzmh3/subdir.mk
-include build/objs/mainTestGoogleTestExe/debug/mainCpp/1n4vdz0oa9ive5y5ifrt07s69/subdir.mk
-include build/objs/mainStaticLibrary/release/mainCpp/7skz2xkzjx8gqmzc7i6pq87en/subdir.mk
-include build/objs/mainStaticLibrary/release/mainCpp/5oux44dogwvhyg2l9gzatzmh3/subdir.mk
-include build/objs/mainStaticLibrary/release/mainCpp/1n4vdz0oa9ive5y5ifrt07s69/subdir.mk
-include build/objs/mainStaticLibrary/failing/mainCpp/7skz2xkzjx8gqmzc7i6pq87en/subdir.mk
-include build/objs/mainStaticLibrary/failing/mainCpp/5oux44dogwvhyg2l9gzatzmh3/subdir.mk
-include build/objs/mainStaticLibrary/failing/mainCpp/1n4vdz0oa9ive5y5ifrt07s69/subdir.mk
-include build/objs/mainStaticLibrary/debug/mainCpp/7skz2xkzjx8gqmzc7i6pq87en/subdir.mk
-include build/objs/mainStaticLibrary/debug/mainCpp/5oux44dogwvhyg2l9gzatzmh3/subdir.mk
-include build/objs/mainStaticLibrary/debug/mainCpp/1n4vdz0oa9ive5y5ifrt07s69/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: krypto

# Tool invocations
krypto: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o "krypto" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(C_UPPER_DEPS)$(CXX_DEPS)$(OBJS)$(CPP_DEPS)$(C_DEPS) krypto
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
