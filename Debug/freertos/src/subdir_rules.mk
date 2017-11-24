################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
freertos/src/croutine.obj: /home/igor/workspace_v7/PAC2/freertos/src/croutine.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/igor/workspace_v7/PAC2/freertos/inc" --include_path="/home/igor/workspace_v7/PAC2/sensors" --include_path="/home/igor/workspace_v7/PAC2/freertos/cortex-m4" --include_path="/home/igor/workspace_v7/PAC2/msp432" --include_path="/home/igor/workspace_v7/PAC2" --include_path="/opt/ti/ccsv7/ccs_base/arm/include" --include_path="/opt/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="freertos/src/croutine.d_raw" --obj_directory="freertos/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

freertos/src/event_groups.obj: /home/igor/workspace_v7/PAC2/freertos/src/event_groups.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/igor/workspace_v7/PAC2/freertos/inc" --include_path="/home/igor/workspace_v7/PAC2/sensors" --include_path="/home/igor/workspace_v7/PAC2/freertos/cortex-m4" --include_path="/home/igor/workspace_v7/PAC2/msp432" --include_path="/home/igor/workspace_v7/PAC2" --include_path="/opt/ti/ccsv7/ccs_base/arm/include" --include_path="/opt/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="freertos/src/event_groups.d_raw" --obj_directory="freertos/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

freertos/src/heap_2.obj: /home/igor/workspace_v7/PAC2/freertos/src/heap_2.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/igor/workspace_v7/PAC2/freertos/inc" --include_path="/home/igor/workspace_v7/PAC2/sensors" --include_path="/home/igor/workspace_v7/PAC2/freertos/cortex-m4" --include_path="/home/igor/workspace_v7/PAC2/msp432" --include_path="/home/igor/workspace_v7/PAC2" --include_path="/opt/ti/ccsv7/ccs_base/arm/include" --include_path="/opt/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="freertos/src/heap_2.d_raw" --obj_directory="freertos/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

freertos/src/list.obj: /home/igor/workspace_v7/PAC2/freertos/src/list.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/igor/workspace_v7/PAC2/freertos/inc" --include_path="/home/igor/workspace_v7/PAC2/sensors" --include_path="/home/igor/workspace_v7/PAC2/freertos/cortex-m4" --include_path="/home/igor/workspace_v7/PAC2/msp432" --include_path="/home/igor/workspace_v7/PAC2" --include_path="/opt/ti/ccsv7/ccs_base/arm/include" --include_path="/opt/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="freertos/src/list.d_raw" --obj_directory="freertos/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

freertos/src/queue.obj: /home/igor/workspace_v7/PAC2/freertos/src/queue.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/igor/workspace_v7/PAC2/freertos/inc" --include_path="/home/igor/workspace_v7/PAC2/sensors" --include_path="/home/igor/workspace_v7/PAC2/freertos/cortex-m4" --include_path="/home/igor/workspace_v7/PAC2/msp432" --include_path="/home/igor/workspace_v7/PAC2" --include_path="/opt/ti/ccsv7/ccs_base/arm/include" --include_path="/opt/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="freertos/src/queue.d_raw" --obj_directory="freertos/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

freertos/src/tasks.obj: /home/igor/workspace_v7/PAC2/freertos/src/tasks.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/igor/workspace_v7/PAC2/freertos/inc" --include_path="/home/igor/workspace_v7/PAC2/sensors" --include_path="/home/igor/workspace_v7/PAC2/freertos/cortex-m4" --include_path="/home/igor/workspace_v7/PAC2/msp432" --include_path="/home/igor/workspace_v7/PAC2" --include_path="/opt/ti/ccsv7/ccs_base/arm/include" --include_path="/opt/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="freertos/src/tasks.d_raw" --obj_directory="freertos/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

freertos/src/timers.obj: /home/igor/workspace_v7/PAC2/freertos/src/timers.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/igor/workspace_v7/PAC2/freertos/inc" --include_path="/home/igor/workspace_v7/PAC2/sensors" --include_path="/home/igor/workspace_v7/PAC2/freertos/cortex-m4" --include_path="/home/igor/workspace_v7/PAC2/msp432" --include_path="/home/igor/workspace_v7/PAC2" --include_path="/opt/ti/ccsv7/ccs_base/arm/include" --include_path="/opt/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="freertos/src/timers.d_raw" --obj_directory="freertos/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


