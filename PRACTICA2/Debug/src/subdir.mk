################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ejemploClienteChat.c \
../src/ejemploServidorChat.c 

CPP_SRCS += \
../src/Ruleta.cpp \
../src/User.cpp \
../src/clienteRuleta.cpp \
../src/resuelvefrase.cpp \
../src/servidorRuleta.cpp 

OBJS += \
./src/Ruleta.o \
./src/User.o \
./src/clienteRuleta.o \
./src/ejemploClienteChat.o \
./src/ejemploServidorChat.o \
./src/resuelvefrase.o \
./src/servidorRuleta.o 

C_DEPS += \
./src/ejemploClienteChat.d \
./src/ejemploServidorChat.d 

CPP_DEPS += \
./src/Ruleta.d \
./src/User.d \
./src/clienteRuleta.d \
./src/resuelvefrase.d \
./src/servidorRuleta.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


