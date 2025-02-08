// Standard includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "hw_common_reg.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "gpio.h"
#include "pin.h"
#include "prcm.h"
#include "utils.h"
#include "uart.h"
#include "timer.h"

// Common interface includes
#include "uart_if.h"
#include "i2c_if.h"
#include "gpio_if.h"

#include "pinmux.h"
#include "SHT2x.h"
#include "timer_if.h"
#include "adc.h"

#include "LCD_TFT_ILI9341.h"
#include "Font_lib.h"
#include "LCD_Display.h"
#include "pin_mux_config.h""

#define FONT1608   (2)
//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

#define UART_PRINT              Report

void wait_ms(long i)
{
 while(i)
	  {__delay_cycles(20000);;i--;}

}


#define APPLICATION_VERSION     "1.1.1"

#define TIMER_10MS_DELAY        80000000/100    /*10ms/loop for delay*/
unsigned int g_uiDelay_sht20;        //for common

//*****************************************************************************
//
// Globals used by the timer interrupt handler.
//
//*****************************************************************************
static volatile unsigned long g_ulSysTickValue;
static volatile unsigned long g_ulBase;
static volatile unsigned long g_ulIntClearVector;
unsigned long g_ulTimerInts;

unsigned char key3flag,key4flag;

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
    //
    // Set vector table base
    //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif

    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}
#define ADC_CH_1   0x00000008
#define NO_OF_SAMPLES 		128
unsigned long pulAdcSamples[4096];
unsigned int  uiIndex=0;

float Wheelgetvalue(void)
{
    unsigned long ulSample;
    float Vol_value;

    //
    // Initialize Array index for multiple execution
    //
    uiIndex=0;

    //
    // Enable ADC channel
    //

    MAP_ADCChannelEnable(ADC_BASE, ADC_CH_1);

    while(uiIndex < NO_OF_SAMPLES + 4)
    {
        if(MAP_ADCFIFOLvlGet(ADC_BASE, ADC_CH_1))
        {
            ulSample = MAP_ADCFIFORead(ADC_BASE, ADC_CH_1);
            pulAdcSamples[uiIndex++] = ulSample;
        }
    }

    MAP_ADCChannelDisable(ADC_BASE, ADC_CH_1);

    uiIndex = 0;

    //
    // Convert ADC samples
    //
    while(uiIndex < NO_OF_SAMPLES)
    {
     Vol_value=(((float)((pulAdcSamples[4+uiIndex] >> 2 ) & 0x0FFF))*1.75)/4096;
     uiIndex++;
    }
    return Vol_value;
}





//*****************************************************************************
//
//! The interrupt handler for the first timer interrupt.
//!
//! \param  None
//!
//! \return none
//
//*****************************************************************************
void
TimerBaseIntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    Timer_IF_InterruptClear(g_ulBase);

    g_uiDelay_sht20 ++;
}

void Key3Handler(void)
{
    unsigned long ulPinState =  GPIOIntStatus(GPIOA2_BASE,1);
    if(ulPinState & GPIO_INT_PIN_7)
    {
    	GPIOIntClear(GPIOA2_BASE,GPIO_INT_PIN_7);
        key3flag=1;
        key4flag=0;

    }

}

void Key4Handler(void)
{
    unsigned long ulPinState =  GPIOIntStatus(GPIOA3_BASE,1);
    if(ulPinState & GPIO_INT_PIN_0)
    {
    	GPIOIntClear(GPIOA3_BASE,GPIO_INT_PIN_0);
        key4flag=1;
        key3flag=0;
    }

}
void Keyinit(void)
{
	GPIOIntTypeSet(GPIOA2_BASE,GPIO_PIN_7,GPIO_RISING_EDGE);
	IntPrioritySet(INT_GPIOA2, INT_PRIORITY_LVL_1);
	GPIOIntRegister(GPIOA2_BASE,Key3Handler);

	GPIOIntClear(GPIOA2_BASE,GPIO_INT_PIN_7);
    IntPendClear(INT_GPIOA2);
    IntEnable(INT_GPIOA2);
	GPIOIntEnable(GPIOA2_BASE,GPIO_INT_PIN_7);

	GPIOIntTypeSet(GPIOA3_BASE,GPIO_PIN_0,GPIO_RISING_EDGE);
	IntPrioritySet(INT_GPIOA3, INT_PRIORITY_LVL_1);
	GPIOIntRegister(GPIOA3_BASE,Key4Handler);

	GPIOIntClear(GPIOA3_BASE,GPIO_INT_PIN_0);
    IntPendClear(INT_GPIOA3);
    IntEnable(INT_GPIOA3);
	GPIOIntEnable(GPIOA3_BASE,GPIO_INT_PIN_0);

}




void while_SHT20(){
    static nt16 sRH;
    static float   humidityRH;
    static nt16 sT;
    static float   temperatureC;
    unsigned char error = 0;
    static unsigned int uiTask=0;
    unsigned char  checksum;   //checksum
    unsigned char tmp = 0xF3,hum = 0xF5;
    float voltage,r;
    char string1[5];
    char string2[20];
    unsigned char RxData[3]={0,0,0};
    //LCD_StringDisplay(30,30,"111111111111111111111");
    //voltage=Wheelgetvalue();
    //LCD_StringDisplay(30,50,"2222222222222222222222");
	//Wheelinit();
	//LCD_StringDisplay(30,70,"333333333333333333333");
	//r=voltage*10000/1.75;
    switch(uiTask){
        case 0:
            I2C_IF_Write(0x40,&tmp,1,1);
            g_uiDelay_sht20 = 0;
            uiTask = 1;
            break;
        case 1:
            if(g_uiDelay_sht20 > 20){   //200ms的数据转化时间
                I2C_IF_Read(0x40,RxData,3);
                sT.s16.u8H = RxData[0];
                sT.s16.u8L = RxData[1];
                checksum=RxData[2];

                //-- verify checksum --
                error |= SHT2x_CheckCrc (RxData,2,checksum);

                temperatureC = SHT2x_CalcTemperatureC(sT.u16);
                uiTask = 2;
            }
            break;
        case 2:
            I2C_IF_Write(0x40,&hum,1,1);
            g_uiDelay_sht20 = 0;
            uiTask = 3;
            break;
        case 3:
                 if(g_uiDelay_sht20 > 20){
                     I2C_IF_Read(0x40,RxData,3);
                     sRH.s16.u8H = RxData[0];
                     sRH.s16.u8L = RxData[1];
                     checksum=RxData[2];

                     //-- verify checksum --
                     error |= SHT2x_CheckCrc (RxData,2,checksum);

                     humidityRH = SHT2x_CalcTemperatureC(sRH.u16);
                     uiTask = 4;
                 }
                 break;
        case 4:
        	voltage = Wheelgetvalue();
        	if(key3flag==1)
                	{
           		    voltage = Wheelgetvalue();
                    //显示温度大小
                    sprintf(string1, "T=%.2f", temperatureC);
                    LCD_StringDisplay(120,173,string1);
                    LCD_StringDisplay(200,173,"℃");
                    //显示电压 大小
                   Wheelinit();
        		   sprintf(string2, "U=%.2f   ", voltage);
        		   LCD_StringDisplay(120,213,string2);
        		   LCD_StringDisplay(200,213,"V");
                    	//显示串口输出
                    UART_PRINT("马家源（2050971）测量的温度为:%.2f℃      ",temperatureC);
                    UART_PRINT("测量的电压为:%.2fV      ",voltage);
                    //UART_PRINT("TC=%.2f C\t   ",temperatureC);//蓝牙用
                    //UART_PRINT("U=%.2f V\n",voltage);
                	 }
        ////////////////////////////////////////
                	//按键4
                   if(key4flag==1)
                  {
                	 temperatureC=temperatureC*9/5+32;
                	 voltage=Wheelgetvalue();
                	 r=voltage*10000/1.75;

                	 //显示电阻大小
                	 sprintf(string2, "R=%.2f", r);
                	 LCD_StringDisplay(120,213,string2);
                	 LCD_StringDisplay(200,213,"\t");
                 //显示温度大小
        	     sprintf(string1, "T=%.2f", temperatureC);
        		 LCD_StringDisplay(120,173,string1);
        		 LCD_StringDisplay(200,173,"F\t");
        		 //串口输出
                  UART_PRINT("马家源（2050971）测量的温度为:%.2f℉      ",temperatureC);
                  UART_PRINT("测量的电阻为:%.2fΩ      ",r);
                  //UART_PRINT("TF=%.2f F\t   ",temperatureC);
                  //UART_PRINT("R=%.2f \n",r);

                	 }
            uiTask = 0;
            break;
    };
}

void Wheelinit(void)
{
	//LCD_StringDisplay(30,120,"66666666666666666");
    //
    // Pinmux for the selected ADC input pin
    //
    MAP_PinTypeADC(PIN_58,PIN_MODE_255);

    //
    // Configure ADC timer which is used to timestamp the ADC data samples
    //
    MAP_ADCTimerConfig(ADC_BASE,2^17);

    //
    // Enable ADC timer which is used to timestamp the ADC data samples
    //
    MAP_ADCTimerEnable(ADC_BASE);

    //
    // Enable ADC module
    //
    MAP_ADCEnable(ADC_BASE);

}

/*
 * main.c
 */

void main(void) {
    //
    // Initialize Board configurations
    //
    BoardInit();

    //
    // Power on the corresponding GPIO port B for 9,10,11.
    // Set up the GPIO lines to mode 0 (GPIO)
    //
    PinMuxConfig();
        Pin_Mux_Config();
      //Pin_Mux_Config();
      //LCD
      lcd_init();

    LCD_ILI9341_TFT_background(White);
    LCD_ILI9341_TFT_foreground(Black);
    LCD_Show_StandbyPage();
    wait_ms(10000);
    lcd_clear();
    LCD_StringDisplay(30,10,"Tongji ");
    LCD_StringDisplay(30,30,"同济大学电子与信息工程学院");
    LCD_StringDisplay(30,50,"信息与通信工程系");
    LCD_StringDisplay(30,70,"2050971马家源");
    //LCD_StringDisplay(30,90,"2024年11月24日");
    UART_PRINT("*********************************************\n\n\n");
    UART_PRINT("          2022级通信工程  2050971  马家源       \n\n\n");
    UART_PRINT("*********************************************\n\n\n");
    //
    // I2C Init
    //
    I2C_IF_Open(I2C_MASTER_MODE_FST);

    //
    // Configuring UART
    //
    InitTerm();

    //
    // Base address for first timer
    //
    g_ulBase = TIMERA0_BASE;
    //
    // Configuring the timers
    //
    Timer_IF_Init(PRCM_TIMERA0, g_ulBase, TIMER_CFG_PERIODIC, TIMER_A, 0);

    //
    // Setup the interrupts for the timer timeouts.
    //
    Timer_IF_IntSetup(g_ulBase, TIMER_A, TimerBaseIntHandler);
    //UART_PRINT("tongji web\n");
    //UART_PRINT("2050971mjy  \n");//蓝牙用


    // Turn on the timers feeding values in mSec
    Timer_IF_Start(g_ulBase, TIMER_A, 10);
    Keyinit();
    Wheelinit();
	while(1)
	{

		while_SHT20();

	    MAP_UtilsDelay(8000000);
	}
}


