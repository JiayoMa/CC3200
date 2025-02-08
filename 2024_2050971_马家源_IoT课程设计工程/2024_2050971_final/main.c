// Standard includes
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// Simplelink includes
#include "simplelink.h"
#include "netcfg.h"

// Driverlib includes
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "utils.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"
#include "pin.h"
#include "timer.h"
#include "adc.h"
#include "hw_apps_rcm.h"
#include "hw_adc.h"

// Common interface includes
#include "gpio_if.h"
#include "uart_if.h"
#include "i2c_if.h"
#include "common.h"
#include "timer_if.h"

// App Includes
#include "device_status.h"
#include "smartconfig.h"
#include "SHT2x.h"
#include "bma222drv.h"
#include "pinmux.h"
#include "LCD_TFT_ILI9341.h"
#include "Font_lib.h"
#include "LCD_Display.h"


//Free_rtos/ti-rtos includes
#include "osi.h"

#define SPAWN_TASK_PRIORITY              9
#define SLEEP_TIME              8000000
#define OSI_STACK_SIZE                   2048
#define AP_SSID_LEN_MAX                 32
#define SH_GPIO_3                       3       /* P58 - Device Mode */
#define AUTO_CONNECTION_TIMEOUT_COUNT   50      /* 5 Sec */
#define SL_STOP_TIMEOUT                 200

#define APP_NAME "马家源-22050971-智能物联网创新实践期末课程设计"
unsigned int g_uiDelay_sht20;//sht20温湿度传感器延迟
float T;//温度
float H;//湿度
static unsigned char g_ucDryerRunning = 0;//运动状态
float voltage;
int flag=1;//按键标记

#define NO_OF_SAMPLES 		128
unsigned long pulAdcSamples[4096];
unsigned int  uiIndex=0;

static volatile unsigned long g_ulSysTickValue;
static volatile unsigned long g_ulBase;
static volatile unsigned long g_ulIntClearVector;
unsigned long g_ulTimerInts;

unsigned char key3flag,key4flag;//按键3

#define TIMER_INTERVAL_RELOAD   40035 /* =(255*157) */
#define DUTYCYCLE_GRANULARITY   157


typedef enum
{
  LED_OFF = 0,
  LED_ON,
  LED_BLINK
}eLEDStatus;

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************


#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif


static const char pcDigits[] = "0123456789";

//http请求
static unsigned char POST_token[] = "__SL_P_ULD";
//static unsigned char GET_token_UIC[]  = "__SL_G_UIC";
static unsigned char GET_token_TEMP[]  = "__SL_G_UTP";
static unsigned char GET_token_HUMI[]  = "__SL_G_UHI";
static unsigned char GET_token_VOICE[]  = "__SL_G_UVE";
static unsigned char GET_token_SERVE[]  = "__SL_G_USE";


static int g_iInternetAccess = -1;
static unsigned int g_uiDeviceModeConfig = ROLE_STA; //default is STA mode
static unsigned char g_ucLEDStatus = LED_OFF;
static unsigned long  g_ulStatus = 0;//SimpleLink Status
static unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID
static unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID

void wait_ms(long i)
{
 while(i)
	  {__delay_cycles(20000);;i--;}

}


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

#ifdef USE_FREERTOS
//*****************************************************************************
//
//! Application defined hook (or callback) function - the tick hook.
//! The tick interrupt can optionally call this
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void
vApplicationTickHook( void )
{
}

//*****************************************************************************
//
//! Application defined hook (or callback) function - assert
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void
vAssertCalled( const char *pcFile, unsigned long ulLine )
{
    while(1)
    {

    }
}

//*****************************************************************************
//
//! Application defined idle task hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void
vApplicationIdleHook( void )
{

}

//*****************************************************************************
//
//! Application provided stack overflow hook function.
//!
//! \param  handle of the offending task
//! \param  name  of the offending task
//!
//! \return none
//!
//*****************************************************************************
void
vApplicationStackOverflowHook( OsiTaskHandle *pxTask, signed char *pcTaskName)
{
    ( void ) pxTask;
    ( void ) pcTaskName;

    for( ;; );
}

void vApplicationMallocFailedHook()
{
    while(1)
  {
    // Infinite loop;
  }
}
#endif

//*****************************************************************************
//
//! itoa
//!
//!    @brief  Convert integer to ASCII in decimal base
//!
//!     @param  cNum is input integer number to convert
//!     @param  cString is output string
//!
//!     @return number of ASCII parameters
//!
//!
//
//*****************************************************************************
static unsigned short itoa(char cNum, char *cString)
{
    char* ptr;
    char uTemp = cNum;
    unsigned short length;

    // value 0 is a special case
    if (cNum == 0)
    {
        length = 1;
        *cString = '0';

        return length;
    }

    // Find out the length of the number, in decimal base
    length = 0;
    while (uTemp > 0)
    {
        uTemp /= 10;
        length++;
    }

    // Do the actual formatting, right to left
    uTemp = cNum;
    ptr = cString + length;
    while (uTemp > 0)
    {
        --ptr;
        *ptr = pcDigits[uTemp % 10];
        uTemp /= 10;
    }

    return length;
}

char * ftoa(float dValue, char * chBuffer , int size)
{
    char * pch = chBuffer;
    int temp;
    int i;
    if(!pch)
      return 0;

    if(!(dValue <= 1E-307 && dValue >= -1E-307)){

        if(dValue < 0){
            *pch++ = '-';
            dValue = -dValue;
        }

        temp = (int)dValue;
        itoa(temp , pch);
        unsigned char ucLen = strlen(pch);
        pch += ucLen;
        *pch++ = '.';
        dValue -= (int)dValue;
        ucLen = size - ucLen - 1;

        for(i = 0; i < ucLen; i++){
            dValue = dValue  * 10;
            temp = (int)dValue;
            itoa(temp, pch);
            pch += strlen(pch);
            dValue -= (int)dValue;
        }
    }else
        *pch++ = '0';

    pch--;
    return chBuffer;
}


//****************************************************************************
//
//! Update the dutycycle of the PWM timer
//!
//! \param ulBase is the base address of the timer to be configured
//! \param ulTimer is the timer to be setup (TIMER_A or  TIMER_B)
//! \param ucLevel translates to duty cycle settings (0:255)
//!
//! This function
//!    1. The specified timer is setup to operate as PWM
//!
//! \return None.
//
//****************************************************************************
void UpdateDutyCycle(unsigned long ulBase, unsigned long ulTimer,
                     unsigned char ucLevel)
{
    //
    // Match value is updated to reflect the new dutycycle settings
    //
    MAP_TimerMatchSet(ulBase,ulTimer,(ucLevel*DUTYCYCLE_GRANULARITY));
}

//****************************************************************************
//
//! Setup the timer in PWM mode
//!
//! \param ulBase is the base address of the timer to be configured
//! \param ulTimer is the timer to be setup (TIMER_A or  TIMER_B)
//! \param ulConfig is the timer configuration setting
//! \param ucInvert is to select the inversion of the output
//!
//! This function
//!    1. The specified timer is setup to operate as PWM
//!
//! \return None.
//
//****************************************************************************
void SetupTimerPWMMode(unsigned long ulBase, unsigned long ulTimer,
                       unsigned long ulConfig, unsigned char ucInvert)
{
    //
    // Set GPT - Configured Timer in PWM mode.
    //
    MAP_TimerConfigure(ulBase,ulConfig);
    MAP_TimerPrescaleSet(ulBase,ulTimer,0);

    //
    // Inverting the timer output if required
    //
    MAP_TimerControlLevel(ulBase,ulTimer,ucInvert);

    //
    // Load value set to ~0.5 ms time period
    //
    MAP_TimerLoadSet(ulBase,ulTimer,TIMER_INTERVAL_RELOAD);

    //
    // Match value set so as to output level 0
    //
    MAP_TimerMatchSet(ulBase,ulTimer,TIMER_INTERVAL_RELOAD);

}

//****************************************************************************
//
//! Sets up the identified timers as PWM to drive the peripherals
//!
//! \param none
//!
//! This function sets up the folowing
//!    1. TIMERA2 (TIMER B) as RED of RGB light
//!    2. TIMERA3 (TIMER B) as YELLOW of RGB light
//!    3. TIMERA3 (TIMER A) as GREEN of RGB light
//!
//! \return None.
//
//****************************************************************************
void InitPWMModules()
{
    //
    // Initialization of timers to generate PWM output
    //
    MAP_PRCMPeripheralClkEnable(PRCM_TIMERA1, PRCM_RUN_MODE_CLK);

    //
    // TIMERA1 (TIMER A) as BEEP. GPIO 25 --> PWM_2
    //
    SetupTimerPWMMode(TIMERA1_BASE, TIMER_A,
    		(TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM), 1);


    MAP_TimerEnable(TIMERA1_BASE,TIMER_A);

}

//****************************************************************************
//
//! Disables the timer PWMs
//!
//! \param none
//!
//! This function disables the timers used
//!
//! \return None.
//
//****************************************************************************
void DeInitPWMModules()
{
    //
    // Disable the peripherals
    //
    MAP_TimerDisable(TIMERA1_BASE, TIMER_A);

    MAP_PRCMPeripheralClkDisable(PRCM_TIMERA1, PRCM_RUN_MODE_CLK);

}

void Voiceinit(void)
{

    //
    // Pinmux for the selected ADC input pin
    //
    MAP_PinTypeADC(PIN_60,PIN_MODE_255);

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

float Voicegetvalue(void)
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

    MAP_ADCChannelEnable(ADC_BASE, ADC_CH_3);

    while(uiIndex < NO_OF_SAMPLES + 4)
    {
        if(MAP_ADCFIFOLvlGet(ADC_BASE, ADC_CH_3))
        {
            ulSample = MAP_ADCFIFORead(ADC_BASE, ADC_CH_3);
            pulAdcSamples[uiIndex++] = ulSample;
        }
    }

    MAP_ADCChannelDisable(ADC_BASE, ADC_CH_3);

    uiIndex = 0;

    //
    // Convert ADC samples
    //
    while(uiIndex < NO_OF_SAMPLES)
    {
     Vol_value=(((float)((pulAdcSamples[4+uiIndex] >> 2 ) & 0x0FFF))*1.4)/4096;
     uiIndex++;
    }
    return Vol_value;
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
//按键初始化
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



//UART初始输出
static void
DisplayBanner(char * AppName)
{
    UART_PRINT("\n\n\n\r");
    UART_PRINT("\t\t *************************************************\n\r");
    UART_PRINT("\t\t     %s        \n\r", AppName);
    UART_PRINT("\t\t *************************************************\n\r");
    UART_PRINT("\n\n\n\r");
}

//SHT20温湿度部分
void TimerBaseIntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    Timer_IF_InterruptClear(g_ulBase);

    g_uiDelay_sht20 ++;
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
        	voltage = Voicegetvalue();
        	if(key3flag==1)
                	{
           		    voltage = Voicegetvalue();
                    //显示温度大小
                    sprintf(string1, "T=%.2f", temperatureC);
                    LCD_StringDisplay(120,173,string1);
                    LCD_StringDisplay(200,173,"℃");
                    //显示电压 大小
                    Voiceinit();
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
                	 voltage=Voicegetvalue();
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
//*****************************************************************************
// write data--byte
//*****************************************************************************
void I2C_writedata(unsigned char command,unsigned char data)
{
  unsigned char TxData[2]={0,0};
  TxData[0]=command;
  TxData[1]=data;
  I2C_IF_Write(APDS_Addr,TxData,2,1);
}

//*****************************************************************************
// read data--byte
//*****************************************************************************
unsigned char I2C_readdata(unsigned char command)
{
  unsigned char *p1;
  p1=&command;
  I2C_IF_Write(APDS_Addr,p1,1,0);
  unsigned char data;
  I2C_IF_Read(APDS_Addr,&data,1);
  return data;
}




//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- Start
//*****************************************************************************


//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    if(pWlanEvent == NULL)
    {
        UART_PRINT("Null pointer\n\r");
        LOOP_FOREVER();
    }
    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);

            //
            // Information about the connected AP (like name, MAC etc) will be
            // available in 'slWlanConnectAsyncResponse_t'
            // Applications can use it if required
            //
            //  slWlanConnectAsyncResponse_t *pEventData = NULL;
            // pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            //

            // Copy new connection SSID and BSSID to global parameters
            memcpy(g_ucConnectionSSID,pWlanEvent->EventData.
                   STAandP2PModeWlanConnected.ssid_name,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
            memcpy(g_ucConnectionBSSID,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
                   SL_BSSID_LENGTH);

            UART_PRINT("[WLAN EVENT] Device Connected to the AP: %s , "
                       "BSSID: %x:%x:%x:%x:%x:%x\n\r",
                      g_ucConnectionSSID,g_ucConnectionBSSID[0],
                      g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                      g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                      g_ucConnectionBSSID[5]);
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            // If the user has initiated 'Disconnect' request,
            //'reason_code' is SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION
            if(SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                UART_PRINT("[WLAN EVENT] Device disconnected from the AP: %s, "
                           "BSSID: %x:%x:%x:%x:%x:%x on application's "
                           "request \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            else
            {
                UART_PRINT("[WLAN ERROR] Device disconnected from the AP AP: %s, "
                           "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
            memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
        }
        break;

        case SL_WLAN_STA_CONNECTED_EVENT:
        {
            // when device is in AP mode and any client connects to device cc3xxx
            //SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            //CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION_FAILED);

            //
            // Information about the connected client (like SSID, MAC etc) will
            // be available in 'slPeerInfoAsyncResponse_t' - Applications
            // can use it if required
            //
            // slPeerInfoAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.APModeStaConnected;
            //

            UART_PRINT("[WLAN EVENT] Station connected to device\n\r");
        }
        break;

        case SL_WLAN_STA_DISCONNECTED_EVENT:
        {
            // when client disconnects from device (AP)
            //CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            //CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);

            //
            // Information about the connected client (like SSID, MAC etc) will
            // be available in 'slPeerInfoAsyncResponse_t' - Applications
            // can use it if required
            //
            // slPeerInfoAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.APModestaDisconnected;
            //
            UART_PRINT("[WLAN EVENT] Station disconnected from device\n\r");
        }
        break;

        case SL_WLAN_SMART_CONFIG_COMPLETE_EVENT:
        {
            //SET_STATUS_BIT(g_ulStatus, STATUS_BIT_SMARTCONFIG_START);

            //
            // Information about the SmartConfig details (like Status, SSID,
            // Token etc) will be available in 'slSmartConfigStartAsyncResponse_t'
            // - Applications can use it if required
            //
            //  slSmartConfigStartAsyncResponse_t *pEventData = NULL;
            //  pEventData = &pSlWlanEvent->EventData.smartConfigStartResponse;
            //

        }
        break;

        case SL_WLAN_SMART_CONFIG_STOP_EVENT:
        {
            // SmartConfig operation finished
            //CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_SMARTCONFIG_START);

            //
            // Information about the SmartConfig details (like Status, padding
            // etc) will be available in 'slSmartConfigStopAsyncResponse_t' -
            // Applications can use it if required
            //
            // slSmartConfigStopAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.smartConfigStopResponse;
            //
        }
        break;

        default:
        {
            UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                       pWlanEvent->Event);
        }
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    if(pNetAppEvent == NULL)
    {
        UART_PRINT("Null pointer\n\r");
        LOOP_FOREVER();
    }

    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        {
            SlIpV4AcquiredAsync_t *pEventData = NULL;

            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            //Ip Acquired Event Data
            pEventData = &pNetAppEvent->EventData.ipAcquiredV4;

            UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
                       "Gateway=%d.%d.%d.%d\n\r",
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,0),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,0));

            UNUSED(pEventData);
        }
        break;

        case SL_NETAPP_IP_LEASED_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);

            //
            // Information about the IP-Leased details(like IP-Leased,lease-time,
            // mac etc) will be available in 'SlIpLeasedAsync_t' - Applications
            // can use it if required
            //
            // SlIpLeasedAsync_t *pEventData = NULL;
            // pEventData = &pNetAppEvent->EventData.ipLeased;
            //

        }
        break;

        case SL_NETAPP_IP_RELEASED_EVENT:
        {
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);

            //
            // Information about the IP-Released details (like IP-address, mac
            // etc) will be available in 'SlIpReleasedAsync_t' - Applications
            // can use it if required
            //
            // SlIpReleasedAsync_t *pEventData = NULL;
            // pEventData = &pNetAppEvent->EventData.ipReleased;
            //
        }
		break;

        default:
        {
            UART_PRINT("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
                       pNetAppEvent->Event);
        }
        break;
    }
}


//*****************************************************************************
//
//! \brief This function handles HTTP server events
//!
//! \param[in]  pServerEvent - Contains the relevant event information
//! \param[in]    pServerResponse - Should be filled by the user with the
//!                                      relevant response information
//!
//! \return None
//!
//****************************************************************************

void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pSlHttpServerEvent,
								SlHttpServerResponse_t *pSlHttpServerResponse)
{
	switch (pSlHttpServerEvent->Event)
	{
		case SL_NETAPP_HTTPGETTOKENVALUE_EVENT:
		{
			unsigned char *ptr;

			ptr = pSlHttpServerResponse->ResponseData.token_value.data;
			pSlHttpServerResponse->ResponseData.token_value.len = 0;

			//温度检测
			if(memcmp(pSlHttpServerEvent->EventData.httpTokenName.data,
					GET_token_TEMP, strlen((const char *)GET_token_TEMP)) == 0)
			{
				char GET_TEMP[10];
				snprintf(GET_TEMP, sizeof(GET_TEMP), "%.2f", T);
				strncpy((char*)ptr, GET_TEMP, strlen(GET_TEMP));
				pSlHttpServerResponse->ResponseData.token_value.len += strlen(GET_TEMP);
			}

			//湿度检测
			if(memcmp(pSlHttpServerEvent->EventData.httpTokenName.data,
					GET_token_HUMI, strlen((const char *)GET_token_HUMI)) == 0)
			{
				char GET_HUMI[10];
				snprintf(GET_HUMI, sizeof(GET_HUMI), "%.2f", H);
				strncpy((char*)ptr, GET_HUMI, strlen(GET_HUMI));
				pSlHttpServerResponse->ResponseData.token_value.len += strlen(GET_HUMI);
			}
			
			//发送服务请求
			if(memcmp(pSlHttpServerEvent->EventData.httpTokenName.data,
					GET_token_SERVE, strlen((const char *)GET_token_SERVE)) == 0)
			{
				//按键检测
				if(key3flag==1)
				{
					flag=!flag;
					key3flag=0;
				}
				if(flag==1)
				{
					ReadAccSensor();
					if(g_ucDryerRunning)
					{
						strcpy((char*)pSlHttpServerResponse->ResponseData.token_value.data,"Service required");
						pSlHttpServerResponse->ResponseData.token_value.len += strlen("Service required");
					}
					else
					{
						strcpy((char*)pSlHttpServerResponse->ResponseData.token_value.data,"Nothing");
						pSlHttpServerResponse->ResponseData.token_value.len += strlen("Nothing");
					}
				}
				else
				{
					unsigned int t1,t2,t3;
					t1=I2C_readdata(0x80|0x18);t2=I2C_readdata(0x80|0x19);
					t3=t2<<8 | t1;
					if(t3>150)
					{
						strcpy((char*)pSlHttpServerResponse->ResponseData.token_value.data,"Service required");
						pSlHttpServerResponse->ResponseData.token_value.len += strlen("Service required");
					}
					else
					{
						strcpy((char*)pSlHttpServerResponse->ResponseData.token_value.data,"Nothing");
						pSlHttpServerResponse->ResponseData.token_value.len += strlen("Nothing");
					}
				}
			}



		}
			break;

		case SL_NETAPP_HTTPPOSTTOKENVALUE_EVENT:
		{
			unsigned char led;
			unsigned char *ptr = pSlHttpServerEvent->EventData.httpPostData.token_name.data;

			//g_ucLEDStatus = 0;
			if(memcmp(ptr, POST_token, strlen((const char *)POST_token)) == 0)
			{
				ptr = pSlHttpServerEvent->EventData.httpPostData.token_value.data;
				if(memcmp(ptr, "LED", 3) != 0)
					break;
				ptr += 3;
				led = *ptr;
				ptr += 2;
				if(led == '1')
				{
					if(memcmp(ptr, "ON", 2) == 0)
					{
						GPIO_IF_LedOn(MCU_RED_LED_GPIO);
						g_ucLEDStatus = LED_ON;

					}
					else if(memcmp(ptr, "Blink", 5) == 0)
					{
						GPIO_IF_LedOff(MCU_RED_LED_GPIO);
						g_ucLEDStatus = LED_BLINK;
					}
					else
					{
						GPIO_IF_LedOff(MCU_RED_LED_GPIO);
						g_ucLEDStatus = LED_OFF;
					}
				}
				else if(led == '2')
				{
					if(memcmp(ptr, "ON", 2) == 0)
					{
						GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
					}
					else if(memcmp(ptr, "Blink", 5) == 0)
					{
						GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
						g_ucLEDStatus = 1;
					}
					else
					{
						GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
					}
				}

			}
		}
			break;
		default:
			break;
	}
}

//*****************************************************************************
//
//! \brief This function handles General Events
//!
//! \param[in]     pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
	if(pDevEvent == NULL)
	{
		UART_PRINT("Null pointer\n\r");
		LOOP_FOREVER();
	}

	//
	// Most of the general errors are not FATAL are are to be handled
	// appropriately by the application
	//
	UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
			   pDevEvent->EventData.deviceEvent.status,
			   pDevEvent->EventData.deviceEvent.sender);
}


//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]      pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if(pSock == NULL)
    {
        UART_PRINT("Null pointer\n\r");
        LOOP_FOREVER();
    }
    //
    // This application doesn't work w/ socket - Events are not expected
    //
    switch( pSock->Event )
    {
        case SL_SOCKET_TX_FAILED_EVENT:
            switch( pSock->socketAsyncEvent.SockTxFailData.status)
            {
                case SL_ECLOSE:
                    UART_PRINT("[SOCK ERROR] - close socket (%d) operation "
                                "failed to transmit all queued packets\n\n",
                                    pSock->socketAsyncEvent.SockTxFailData.sd);
                    break;
                default:
                    UART_PRINT("[SOCK ERROR] - TX FAILED  :  socket %d , reason "
                                "(%d) \n\n",
                                pSock->socketAsyncEvent.SockTxFailData.sd, pSock->socketAsyncEvent.SockTxFailData.status);
                  break;
            }
            break;

        default:
        	UART_PRINT("[SOCK EVENT] - Unexpected Event [%x0x]\n\n",pSock->Event);
          break;
    }
}


//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- End
//*****************************************************************************

//*****************************************************************************
//
//! \brief This function initializes the application variables
//!
//! \param    None
//!
//! \return None
//!
//*****************************************************************************
static void InitializeAppVariables()
{
    g_ulStatus = 0;
    memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
    memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
    g_iInternetAccess = -1;
    g_ucDryerRunning = 0;
    g_uiDeviceModeConfig = ROLE_STA; //default is STA mode
    g_ucLEDStatus = LED_OFF;
}

//****************************************************************************
//
//! Confgiures the mode in which the device will work
//!
//! \param iMode is the current mode of the device
//!
//!
//! \return   SlWlanMode_t
//!
//
//****************************************************************************
static int ConfigureMode(int iMode)
{
    long   lRetVal = -1;

    lRetVal = sl_WlanSetMode(iMode);
    ASSERT_ON_ERROR(lRetVal);

    /* Restart Network processor */
    lRetVal = sl_Stop(SL_STOP_TIMEOUT);

    // reset status bits
    CLR_STATUS_BIT_ALL(g_ulStatus);

    return sl_Start(NULL,NULL,NULL);
}

//****************************************************************************
//
//!    \brief Connects to the Network in AP or STA Mode - If ForceAP Jumper is
//!                                             Placed, Force it to AP mode
//!
//! \return  0 - Success
//!            -1 - Failure
//
//****************************************************************************
long ConnectToNetwork()
{
    long lRetVal = -1;
    unsigned int uiConnectTimeoutCnt =0;

    // staring simplelink
    lRetVal =  sl_Start(NULL,NULL,NULL);
    ASSERT_ON_ERROR( lRetVal);

    // Device is in AP Mode and Force AP Jumper is not Connected
    if(ROLE_STA != lRetVal && g_uiDeviceModeConfig == ROLE_STA )
    {
        if (ROLE_AP == lRetVal)
        {
            // If the device is in AP mode, we need to wait for this event
            // before doing anything
            while(!IS_IP_ACQUIRED(g_ulStatus))
            {
            #ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask();
            #endif
            }
        }
        //Switch to STA Mode
        lRetVal = ConfigureMode(ROLE_STA);
        ASSERT_ON_ERROR( lRetVal);
    }

    //Device is in STA Mode and Force AP Jumper is Connected
    if(ROLE_AP != lRetVal && g_uiDeviceModeConfig == ROLE_AP )
    {
         //Switch to AP Mode
         lRetVal = ConfigureMode(ROLE_AP);
         ASSERT_ON_ERROR( lRetVal);

    }

    //No Mode Change Required
    if(lRetVal == ROLE_AP)
    {
        //waiting for the AP to acquire IP address from Internal DHCP Server
        // If the device is in AP mode, we need to wait for this event
        // before doing anything
        while(!IS_IP_ACQUIRED(g_ulStatus))
        {
        #ifndef SL_PLATFORM_MULTI_THREADED
            _SlNonOsMainLoopTask();
        #endif
        }
        //Stop Internal HTTP Server
        lRetVal = sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
        ASSERT_ON_ERROR( lRetVal);

        //Start Internal HTTP Server
        lRetVal = sl_NetAppStart(SL_NET_APP_HTTP_SERVER_ID);
        ASSERT_ON_ERROR( lRetVal);

       char cCount=0;

       //Blink LED 3 times to Indicate AP Mode
       for(cCount=0;cCount<3;cCount++)
       {
           //Turn RED LED On
           GPIO_IF_LedOn(MCU_RED_LED_GPIO);
           osi_Sleep(400);

           //Turn RED LED Off
           GPIO_IF_LedOff(MCU_RED_LED_GPIO);
           osi_Sleep(400);
       }

       char ssid[32];
	   unsigned short len = 32;
	   unsigned short config_opt = WLAN_AP_OPT_SSID;
	   sl_WlanGet(SL_WLAN_CFG_AP_ID, &config_opt , &len, (unsigned char* )ssid);
	   UART_PRINT("\n\r Connect to : \'%s\'\n\r\n\r",ssid);
    }
    else
    {
        //Stop Internal HTTP Server
        lRetVal = sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
        ASSERT_ON_ERROR( lRetVal);

        //Start Internal HTTP Server
        lRetVal = sl_NetAppStart(SL_NET_APP_HTTP_SERVER_ID);
        ASSERT_ON_ERROR( lRetVal);

    	//waiting for the device to Auto Connect
        while(uiConnectTimeoutCnt<AUTO_CONNECTION_TIMEOUT_COUNT &&
            ((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus))))
        {
            //Turn RED LED On
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);
            osi_Sleep(50);

            //Turn RED LED Off
            GPIO_IF_LedOff(MCU_RED_LED_GPIO);
            osi_Sleep(50);

            uiConnectTimeoutCnt++;
        }
        //Couldn't connect Using Auto Profile
        if(uiConnectTimeoutCnt == AUTO_CONNECTION_TIMEOUT_COUNT)
        {
            //Blink Red LED to Indicate Connection Error
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);

            CLR_STATUS_BIT_ALL(g_ulStatus);

            //Connect Using Smart Config
            lRetVal = SmartConfigConnect();
            ASSERT_ON_ERROR(lRetVal);

            //Waiting for the device to Auto Connect
            while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus)))
            {
                MAP_UtilsDelay(500);
            }
    }
    //Turn RED LED Off
    GPIO_IF_LedOff(MCU_RED_LED_GPIO);

    g_iInternetAccess = ConnectionTest();

    }
    return SUCCESS;
}

//****************************************************************************
//
//!    \brief Read Force AP GPIO and Configure Mode - 1(Access Point Mode)
//!                                                  - 0 (Station Mode)
//!
//! \return                        None
//
//****************************************************************************
static void ReadDeviceConfiguration()
{
    unsigned int uiGPIOPort;
    unsigned char pucGPIOPin;
    unsigned char ucPinValue;

    //Read GPIO
    GPIO_IF_GetPortNPin(SH_GPIO_3,&uiGPIOPort,&pucGPIOPin);
    ucPinValue = GPIO_IF_Get(SH_GPIO_3,uiGPIOPort,pucGPIOPin);

    //If Connected to VCC, Mode is AP
    if(ucPinValue == 1)
    {
        //AP Mode
        g_uiDeviceModeConfig = ROLE_AP;
    }
    else
    {
        //STA Mode
        g_uiDeviceModeConfig = ROLE_STA;
    }

}

void mjy_Iot_task(void *pvParameters)
{
	long   lRetVal = -1;
	//Read Device Mode Configuration
	ReadDeviceConfiguration();
	//Connect to Network
	lRetVal = ConnectToNetwork();
	if(lRetVal < 0)
	{
		ERR_PRINT(lRetVal);
		LOOP_FOREVER();
	}

	//固定显示
	LCD_StringDisplay(5,10,"项目名称：智能家居系统");
	LCD_StringDisplay(5,30,"同济大学电信学院");
	LCD_StringDisplay(5,50,"通信工程");
	LCD_StringDisplay(5,70,"2050971马家源");

	   while(1)
	   {
			while_SHT20();
	   MAP_UtilsDelay(80000);
	   }

}

//主函数
void main()
{
    long  lRetVal = -1;

    // Board Initilizationr
    BoardInit();
    
    // Configure the pinmux settings for the peripherals exercised
    PinMuxConfig();
    PinConfigSet(PIN_58,PIN_STRENGTH_2MA|PIN_STRENGTH_4MA,PIN_TYPE_STD_PD);

    //
	// LED Init
	//
	GPIO_IF_LedConfigure(LED1);

	//Turn Off the LEDs
	GPIO_IF_LedOff(MCU_RED_LED_GPIO);

    // Initialize Global Variables
	InitializeAppVariables();

    //LCD初始化
    lcd_init();
     LCD_ILI9341_TFT_background(White);
    LCD_ILI9341_TFT_foreground(Black);
    LCD_Show_StandbyPage();
    wait_ms(10000);
    lcd_clear();

    // UART Init
    InitTerm();
    
    DisplayBanner(APP_NAME);

    // I2C Init
    lRetVal = I2C_IF_Open(I2C_MASTER_MODE_FST);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    //温湿度传感器时钟
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

	//
	// Turn on the timers feeding values in mSec
	//
	Timer_IF_Start(g_ulBase, TIMER_A, 10);

    //按键初始化
    Keyinit();
    Voiceinit();
    apds_init();

	lRetVal = BMA222Open();
	if(lRetVal < 0)
	{
		ERR_PRINT(lRetVal);
		LOOP_FOREVER();
	}

    //
	// Simplelinkspawntask
	//
	lRetVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
	if(lRetVal < 0)
	{
		ERR_PRINT(lRetVal);
		LOOP_FOREVER();
	}


	lRetVal = osi_TaskCreate(mjy_Iot_task,
					(const signed char *)"My",
					OSI_STACK_SIZE,
					NULL,
					1,
					NULL );

	if(lRetVal < 0)
	{
		ERR_PRINT(lRetVal);
		LOOP_FOREVER();
	}

	//
	// Start the task scheduler
	//
	osi_start();

	while (1)
	{

	}
}
