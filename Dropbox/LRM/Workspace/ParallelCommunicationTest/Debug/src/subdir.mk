################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ParallelCommunicationTest.cpp \
../src/SRV.cpp \
../src/TCPSocket.cpp 

OBJS += \
./src/ParallelCommunicationTest.o \
./src/SRV.o \
./src/TCPSocket.o 

CPP_DEPS += \
./src/ParallelCommunicationTest.d \
./src/SRV.d \
./src/TCPSocket.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


