################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
i2c_driver.obj: ../i2c_driver.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/toni/CodeComposerWorkspace/freertos/inc" --include_path="/Users/toni/CodeComposerWorkspace/PAC2/sensors" --include_path="/Users/toni/CodeComposerWorkspace/freertos/cortex-m4" --include_path="/Users/toni/CodeComposerWorkspace/ti/msp432" --include_path="/Users/toni/CodeComposerWorkspace/PAC2" --include_path="/Applications/ti/ccsv7/ccs_base/arm/include" --include_path="/Applications/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="i2c_driver.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/toni/CodeComposerWorkspace/freertos/inc" --include_path="/Users/toni/CodeComposerWorkspace/PAC2/sensors" --include_path="/Users/toni/CodeComposerWorkspace/freertos/cortex-m4" --include_path="/Users/toni/CodeComposerWorkspace/ti/msp432" --include_path="/Users/toni/CodeComposerWorkspace/PAC2" --include_path="/Applications/ti/ccsv7/ccs_base/arm/include" --include_path="/Applications/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="main.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

msp432_startup_ccs.obj: ../msp432_startup_ccs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/toni/CodeComposerWorkspace/freertos/inc" --include_path="/Users/toni/CodeComposerWorkspace/PAC2/sensors" --include_path="/Users/toni/CodeComposerWorkspace/freertos/cortex-m4" --include_path="/Users/toni/CodeComposerWorkspace/ti/msp432" --include_path="/Users/toni/CodeComposerWorkspace/PAC2" --include_path="/Applications/ti/ccsv7/ccs_base/arm/include" --include_path="/Applications/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="msp432_startup_ccs.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

system_msp432p401r.obj: ../system_msp432p401r.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/toni/CodeComposerWorkspace/freertos/inc" --include_path="/Users/toni/CodeComposerWorkspace/PAC2/sensors" --include_path="/Users/toni/CodeComposerWorkspace/freertos/cortex-m4" --include_path="/Users/toni/CodeComposerWorkspace/ti/msp432" --include_path="/Users/toni/CodeComposerWorkspace/PAC2" --include_path="/Applications/ti/ccsv7/ccs_base/arm/include" --include_path="/Applications/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="system_msp432p401r.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


