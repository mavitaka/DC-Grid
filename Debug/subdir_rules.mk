################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
HardwareSetup.obj: ../HardwareSetup.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti_NEW/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/Yacine/workspace_v7/ProjectDCGrid" --include_path="C:/ti_NEW/TivaWare_C_Series-2.1.4.178" --include_path="C:/ti_NEW/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include" --define=DEBUG_SERIE --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="HardwareSetup.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Task_RTOS.obj: ../Task_RTOS.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti_NEW/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/Yacine/workspace_v7/ProjectDCGrid" --include_path="C:/ti_NEW/TivaWare_C_Series-2.1.4.178" --include_path="C:/ti_NEW/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include" --define=DEBUG_SERIE --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="Task_RTOS.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

build-167375519:
	@$(MAKE) -Onone -f subdir_rules.mk build-167375519-inproc

build-167375519-inproc: ../app.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"C:/ti_NEW/xdctools_3_50_01_12_core/xs" --xdcpath="C:/ti_NEW/bios_6_50_01_12/packages;C:/ti_NEW/uia_2_20_00_02/packages;C:/ti_NEW/ccsv7/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.tiva:TM4C123GH6PM -r release -c "C:/ti_NEW/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS" --compileOptions "-g --optimize_with_debug" "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: build-167375519 ../app.cfg
configPkg/compiler.opt: build-167375519
configPkg/: build-167375519

main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti_NEW/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/Yacine/workspace_v7/ProjectDCGrid" --include_path="C:/ti_NEW/TivaWare_C_Series-2.1.4.178" --include_path="C:/ti_NEW/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include" --define=DEBUG_SERIE --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="main.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


