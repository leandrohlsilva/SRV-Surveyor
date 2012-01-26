################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ConcurrentReadingInputs.cpp \
../src/Movement.cpp \
../src/MovementsController.cpp \
../src/MovementsReader.cpp \
../src/SRV.cpp 

OBJS += \
./src/ConcurrentReadingInputs.o \
./src/Movement.o \
./src/MovementsController.o \
./src/MovementsReader.o \
./src/SRV.o 

CPP_DEPS += \
./src/ConcurrentReadingInputs.d \
./src/Movement.d \
./src/MovementsController.d \
./src/MovementsReader.d \
./src/SRV.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


