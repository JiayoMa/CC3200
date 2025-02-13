//*****************************************************************************
// pin_mux_config.c
//
// configure the device pins for different signals
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

// This file was automatically generated on 2016年8月6日 at 下午3:40:40
// by TI PinMux version 3.0.516 
//
//*****************************************************************************

#include "pin_mux_config.h" 
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "pin.h"
#include "gpio.h"
#include "prcm.h"

//*****************************************************************************
void Pin_Mux_Config(void)
{
    //
    // Enable Peripheral Clocks 
    //
    //PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
    PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);
    PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK);

    //
    // Configure PIN_55 for GPIO Input
    //
    //PinTypeGPIO(PIN_55, PIN_MODE_0, false);
   // GPIODirModeSet(GPIOA0_BASE, 0x2, GPIO_DIR_MODE_IN);

    //
    // Configure PIN_15 for GPIO Input
    //
    PinTypeGPIO(PIN_15, PIN_MODE_0, false);
    GPIODirModeSet(GPIOA2_BASE, 0x40, GPIO_DIR_MODE_IN);

    //
    // Configure PIN_16 for GPIO Input
    //
    PinTypeGPIO(PIN_16, PIN_MODE_0, false);
    GPIODirModeSet(GPIOA2_BASE, 0x80, GPIO_DIR_MODE_IN);

    //
    // Configure PIN_17 for GPIO Input
    //
    PinTypeGPIO(PIN_17, PIN_MODE_0, false);
    GPIODirModeSet(GPIOA3_BASE, 0x1, GPIO_DIR_MODE_IN);

    //
    // Configure PIN_18 for GPIO Output
    //
    PinTypeGPIO(PIN_18, PIN_MODE_0, false);
    GPIODirModeSet(GPIOA3_BASE, 0x10, GPIO_DIR_MODE_OUT);
//按键
    PinTypeGPIO(PIN_45, PIN_MODE_0, false);
    GPIODirModeSet(GPIOA3_BASE, 0x80, GPIO_DIR_MODE_OUT);
//    //
//    // Configure PIN_30 for GPIO Output
//    //
//    PinTypeGPIO(PIN_30, PIN_MODE_0, false);
//    GPIODirModeSet(GPIOA3_BASE, 0x08, GPIO_DIR_MODE_OUT);
//
//    //
//    // Configure PIN_29 for GPIO Output
//    //
//    PinTypeGPIO(PIN_29, PIN_MODE_0, false);
//    GPIODirModeSet(GPIOA3_BASE, 0x04, GPIO_DIR_MODE_OUT);

    //P29--GPIO26引脚的初始化要操作对应的寄存器GPIO_PAD_CONFIG_26(0x4402E108)
    GPIODirModeSet(GPIOA3_BASE,GPIO_PIN_2,GPIO_DIR_MODE_OUT);
    //PinModeSet
    HWREG(0x4402E108) = (((HWREG(0x4402E108) & ~0x0000000F) |
                                                    0x00000000) & ~(3<<10));
    //PinConfigSet
    HWREG(0x4402E108) = ((HWREG(0x4402E108) & ~0xC00) | 0x00000800);

    //P30--GPIO27引脚的初始化要操作对应的寄存器GPIO_PAD_CONFIG_27(0x4402E10C)
    GPIODirModeSet(GPIOA3_BASE,GPIO_PIN_3,GPIO_DIR_MODE_OUT);
    //PinModeSet
    HWREG(0x4402E10C) = (((HWREG(0x4402E10C) & ~0x0000000F) |
                                                    0x00000000) & ~(3<<10));
    //PinConfigSet
    HWREG(0x4402E10C) = ((HWREG(0x4402E10C) & ~0xC00) | 0x00000800);
}
