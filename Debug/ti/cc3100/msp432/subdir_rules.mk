################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
ti/cc3100/msp432/board.obj: ../ti/cc3100/msp432/board.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/opt/ti/ccsv7/ccs_base/arm/include" --include_path="/home/igor/workspace_v7/PAC1/ti/msp432" --include_path="/opt/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/home/igor/workspace_v7/PAC1" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="ti/cc3100/msp432/board.d_raw" --obj_directory="ti/cc3100/msp432" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ti/cc3100/msp432/cli_uart.obj: ../ti/cc3100/msp432/cli_uart.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/opt/ti/ccsv7/ccs_base/arm/include" --include_path="/home/igor/workspace_v7/PAC1/ti/msp432" --include_path="/opt/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/home/igor/workspace_v7/PAC1" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="ti/cc3100/msp432/cli_uart.d_raw" --obj_directory="ti/cc3100/msp432" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ti/cc3100/msp432/spi_cc3100.obj: ../ti/cc3100/msp432/spi_cc3100.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/opt/ti/ccsv7/ccs_base/arm/include" --include_path="/home/igor/workspace_v7/PAC1/ti/msp432" --include_path="/opt/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/home/igor/workspace_v7/PAC1" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="ti/cc3100/msp432/spi_cc3100.d_raw" --obj_directory="ti/cc3100/msp432" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ti/cc3100/msp432/uart_cc3100.obj: ../ti/cc3100/msp432/uart_cc3100.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/opt/ti/ccsv7/ccs_base/arm/include" --include_path="/home/igor/workspace_v7/PAC1/ti/msp432" --include_path="/opt/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/home/igor/workspace_v7/PAC1" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="ti/cc3100/msp432/uart_cc3100.d_raw" --obj_directory="ti/cc3100/msp432" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


