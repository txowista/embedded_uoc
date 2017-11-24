################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
freertos/cortex-m4/port.obj: /home/igor/workspace_v7/PAC2/freertos/cortex-m4/port.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/igor/workspace_v7/PAC2/freertos/inc" --include_path="/home/igor/workspace_v7/PAC2/sensors" --include_path="/home/igor/workspace_v7/PAC2/freertos/cortex-m4" --include_path="/home/igor/workspace_v7/PAC2/msp432" --include_path="/home/igor/workspace_v7/PAC2" --include_path="/opt/ti/ccsv7/ccs_base/arm/include" --include_path="/opt/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="freertos/cortex-m4/port.d_raw" --obj_directory="freertos/cortex-m4" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

freertos/cortex-m4/portasm.obj: /home/igor/workspace_v7/PAC2/freertos/cortex-m4/portasm.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/igor/workspace_v7/PAC2/freertos/inc" --include_path="/home/igor/workspace_v7/PAC2/sensors" --include_path="/home/igor/workspace_v7/PAC2/freertos/cortex-m4" --include_path="/home/igor/workspace_v7/PAC2/msp432" --include_path="/home/igor/workspace_v7/PAC2" --include_path="/opt/ti/ccsv7/ccs_base/arm/include" --include_path="/opt/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="freertos/cortex-m4/portasm.d_raw" --obj_directory="freertos/cortex-m4" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


