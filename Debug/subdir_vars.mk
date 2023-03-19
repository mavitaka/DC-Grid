################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CFG_SRCS += \
../app.cfg 

CMD_SRCS += \
../TM4C123GH6PM.cmd 

C_SRCS += \
../HardwareSetup.c \
../Task_RTOS.c \
../main.c 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_MISC_DIRS += \
./configPkg/ 

C_DEPS += \
./HardwareSetup.d \
./Task_RTOS.d \
./main.d 

GEN_OPTS += \
./configPkg/compiler.opt 

OBJS += \
./HardwareSetup.obj \
./Task_RTOS.obj \
./main.obj 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

OBJS__QUOTED += \
"HardwareSetup.obj" \
"Task_RTOS.obj" \
"main.obj" 

C_DEPS__QUOTED += \
"HardwareSetup.d" \
"Task_RTOS.d" \
"main.d" 

GEN_FILES__QUOTED += \
"configPkg\linker.cmd" \
"configPkg\compiler.opt" 

C_SRCS__QUOTED += \
"../HardwareSetup.c" \
"../Task_RTOS.c" \
"../main.c" 


