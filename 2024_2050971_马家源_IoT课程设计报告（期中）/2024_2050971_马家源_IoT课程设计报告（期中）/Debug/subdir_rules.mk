################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Font_lib.obj: ../Font_lib.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me --include_path="D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="D:/software/WebOfThings/workspace/LAB_BLE2050971" -g --define=cc3200 --define=ccs --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="Font_lib.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

LCD_TFT_ILI9341.obj: ../LCD_TFT_ILI9341.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me --include_path="D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="D:/software/WebOfThings/workspace/LAB_BLE2050971" -g --define=cc3200 --define=ccs --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="LCD_TFT_ILI9341.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

LCD_display.obj: ../LCD_display.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me --include_path="D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="D:/software/WebOfThings/workspace/LAB_BLE2050971" -g --define=cc3200 --define=ccs --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="LCD_display.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SHT2X.obj: ../SHT2X.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me --include_path="D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="D:/software/WebOfThings/workspace/LAB_BLE2050971" -g --define=cc3200 --define=ccs --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="SHT2X.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gpio_if.obj: ../gpio_if.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me --include_path="D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="D:/software/WebOfThings/workspace/LAB_BLE2050971" -g --define=cc3200 --define=ccs --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="gpio_if.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

i2c_if.obj: ../i2c_if.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me --include_path="D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="D:/software/WebOfThings/workspace/LAB_BLE2050971" -g --define=cc3200 --define=ccs --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="i2c_if.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me --include_path="D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="D:/software/WebOfThings/workspace/LAB_BLE2050971" -g --define=cc3200 --define=ccs --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="main.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

pin_mux_config.obj: ../pin_mux_config.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me --include_path="D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="D:/software/WebOfThings/workspace/LAB_BLE2050971" -g --define=cc3200 --define=ccs --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="pin_mux_config.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

pinmux.obj: ../pinmux.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me --include_path="D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="D:/software/WebOfThings/workspace/LAB_BLE2050971" -g --define=cc3200 --define=ccs --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="pinmux.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup_ccs.obj: ../startup_ccs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me --include_path="D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="D:/software/WebOfThings/workspace/LAB_BLE2050971" -g --define=cc3200 --define=ccs --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="startup_ccs.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

timer_if.obj: ../timer_if.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me --include_path="D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="D:/software/WebOfThings/workspace/LAB_BLE2050971" -g --define=cc3200 --define=ccs --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="timer_if.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

uart_if.obj: ../uart_if.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me --include_path="D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="D:/software/WebOfThings/workspace/LAB_BLE2050971" -g --define=cc3200 --define=ccs --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="uart_if.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils_if.obj: ../utils_if.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me --include_path="D:/software/WebOfThings/CCS6.2.0/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="D:/software/WebOfThings/workspace/LAB_BLE2050971" -g --define=cc3200 --define=ccs --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="utils_if.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


