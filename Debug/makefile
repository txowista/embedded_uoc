################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./i2c_driver.obj" \
"./main.obj" \
"./msp432_startup_ccs.obj" \
"./system_msp432p401r.obj" \
"./freertos/cortex-m4/port.obj" \
"./freertos/cortex-m4/portasm.obj" \
"./freertos/src/croutine.obj" \
"./freertos/src/event_groups.obj" \
"./freertos/src/heap_2.obj" \
"./freertos/src/list.obj" \
"./freertos/src/queue.obj" \
"./freertos/src/tasks.obj" \
"./freertos/src/timers.obj" \
"./sensors/adc14_multiple_channel_no_repeat.obj" \
"./sensors/opt3001.obj" \
"./sensors/temp_sensor.obj" \
"./sensors/tmp006.obj" \
"../msp432p401r.cmd" \
"../lib/msp432p4xx_driverlib.lib" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include freertos/cortex-m4/subdir_vars.mk
-include freertos/src/subdir_vars.mk
-include lib/subdir_vars.mk
-include sensors/subdir_vars.mk
-include subdir_rules.mk
-include freertos/cortex-m4/subdir_rules.mk
-include freertos/src/subdir_rules.mk
-include lib/subdir_rules.mk
-include sensors/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
PAC2.out \

EXE_OUTPUTS__QUOTED += \
"PAC2.out" \

BIN_OUTPUTS += \
PAC2.hex \

BIN_OUTPUTS__QUOTED += \
"PAC2.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "PAC2.out"

# Tool invocations
PAC2.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: MSP432 Linker'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi -z -m"PAC2.map" --heap_size=1024 --stack_size=512 -i"/opt/ti/ccsv7/ccs_base/arm/include" -i"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/lib" -i"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="PAC2_linkInfo.xml" --rom_model -o "PAC2.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

PAC2.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: MSP432 Hex Utility'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armhex" --memwidth=8 --romwidth=8 -o "PAC2.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "i2c_driver.obj" "main.obj" "msp432_startup_ccs.obj" "system_msp432p401r.obj" "freertos/cortex-m4/port.obj" "freertos/cortex-m4/portasm.obj" "freertos/src/croutine.obj" "freertos/src/event_groups.obj" "freertos/src/heap_2.obj" "freertos/src/list.obj" "freertos/src/queue.obj" "freertos/src/tasks.obj" "freertos/src/timers.obj" "sensors/adc14_multiple_channel_no_repeat.obj" "sensors/opt3001.obj" "sensors/temp_sensor.obj" "sensors/tmp006.obj" 
	-$(RM) "i2c_driver.d" "main.d" "msp432_startup_ccs.d" "system_msp432p401r.d" "freertos/cortex-m4/port.d" "freertos/src/croutine.d" "freertos/src/event_groups.d" "freertos/src/heap_2.d" "freertos/src/list.d" "freertos/src/queue.d" "freertos/src/tasks.d" "freertos/src/timers.d" "sensors/adc14_multiple_channel_no_repeat.d" "sensors/opt3001.d" "sensors/temp_sensor.d" "sensors/tmp006.d" 
	-$(RM) "freertos/cortex-m4/portasm.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

