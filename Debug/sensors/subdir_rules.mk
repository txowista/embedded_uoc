################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
sensors/opt3001.obj: ../sensors/opt3001.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/toni/CodeComposerWorkspace/freertos/inc" --include_path="/Users/toni/CodeComposerWorkspace/PAC2/sensors" --include_path="/Users/toni/CodeComposerWorkspace/freertos/cortex-m4" --include_path="/Users/toni/CodeComposerWorkspace/ti/msp432" --include_path="/Users/toni/CodeComposerWorkspace/PAC2" --include_path="/Applications/ti/ccsv7/ccs_base/arm/include" --include_path="/Applications/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="sensors/opt3001.d" --obj_directory="sensors" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sensors/temp_sensor.obj: ../sensors/temp_sensor.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/toni/CodeComposerWorkspace/freertos/inc" --include_path="/Users/toni/CodeComposerWorkspace/PAC2/sensors" --include_path="/Users/toni/CodeComposerWorkspace/freertos/cortex-m4" --include_path="/Users/toni/CodeComposerWorkspace/ti/msp432" --include_path="/Users/toni/CodeComposerWorkspace/PAC2" --include_path="/Applications/ti/ccsv7/ccs_base/arm/include" --include_path="/Applications/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="sensors/temp_sensor.d" --obj_directory="sensors" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sensors/tmp006.obj: ../sensors/tmp006.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/toni/CodeComposerWorkspace/freertos/inc" --include_path="/Users/toni/CodeComposerWorkspace/PAC2/sensors" --include_path="/Users/toni/CodeComposerWorkspace/freertos/cortex-m4" --include_path="/Users/toni/CodeComposerWorkspace/ti/msp432" --include_path="/Users/toni/CodeComposerWorkspace/PAC2" --include_path="/Applications/ti/ccsv7/ccs_base/arm/include" --include_path="/Applications/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="sensors/tmp006.d" --obj_directory="sensors" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


