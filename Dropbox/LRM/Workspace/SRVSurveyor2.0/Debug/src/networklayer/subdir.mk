################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/networklayer/Message.cpp \
../src/networklayer/Receiver.cpp \
../src/networklayer/Sender.cpp \
../src/networklayer/TCPSocket.cpp 

OBJS += \
./src/networklayer/Message.o \
./src/networklayer/Receiver.o \
./src/networklayer/Sender.o \
./src/networklayer/TCPSocket.o 

CPP_DEPS += \
./src/networklayer/Message.d \
./src/networklayer/Receiver.d \
./src/networklayer/Sender.d \
./src/networklayer/TCPSocket.d 


# Each subdirectory must supply rules for building sources it contributes
src/networklayer/%.o: ../src/networklayer/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


