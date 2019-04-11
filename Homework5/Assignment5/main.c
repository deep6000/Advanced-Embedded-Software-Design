

/* Temperature Logger`
 *
 * main.c
 *
 * @author Deepesh Sonigra
 *
 *
 *
 * Reference:
 *
 * [1]  Project was build on this demo project
 *      https://github.com/akobyl/TM4C129_FreeRTOS_Demo
 *      Andy Kobyljanec
 *      This is a simple demonstration project of FreeRTOS 8.2 on the Tiva Launchpad
 *      EK-TM4C1294XL.  TivaWare driverlib sourcecode is included.
 *
 * [2]  RTC register initialization was referred
 *      https://e2e.ti.com/support/microcontrollers/other/f/908/t/368236
 *      This code has resolution of 1 sec
 *
 * [3]  I2C Getbyte
 *      https://github.com/rheidebr/ECEN5013-LUX_Temp_demo/blob/master/ECEN_LUX_Temp.zip
 *      i2c get byte code was referred from Prof Rick Heidebrecht
 */


#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "drivers/pinout.h"
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/hibernate.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/i2c.h"
#include "driverlib/inc/hw_i2c.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "main.h"
#include"timers.h"


/* Priorities at which the tasks are created. */
#define mainQUEUE_LOGGER_TASK_PRIORITY              ( tskIDLE_PRIORITY + 3 )
#define mainQUEUE_TEMPERATURE_TASK_PRIORITY         ( tskIDLE_PRIORITY + 1 )
#define mainQUEUE_LED_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_ALERT_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 1 )
#define mainQUEUE_LENGTH                            ( 2 )   /* The number of items the queue can hold*/


static QueueHandle_t xQueue = NULL;
TaskHandle_t Alert_task;



// Main function
int main(void)
{
    // Initialize system clock to 120 MHz

    uint32_t output_clock_rate_hz;
    output_clock_rate_hz = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);
    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);
    RTC_init(output_clock_rate_hz);
    I2C_init(output_clock_rate_hz);

    xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof(Packet) );
    if( xQueue != NULL )
    {

    // Create tasks
          xTaskCreate(LoggerTask, (const portCHAR *)"Logger",
                    configMINIMAL_STACK_SIZE, NULL,  mainQUEUE_LOGGER_TASK_PRIORITY , NULL);
          xTaskCreate(TempTask, (const portCHAR *)"LEDs",
                configMINIMAL_STACK_SIZE, NULL, mainQUEUE_TEMPERATURE_TASK_PRIORITY, NULL);

          xTaskCreate(LEDTask, (const portCHAR *)"LEDs",
                configMINIMAL_STACK_SIZE, NULL,mainQUEUE_LED_TASK_PRIORITY , NULL);

          xTaskCreate(AlertTask, (const portCHAR *)"ALERT",
                      configMINIMAL_STACK_SIZE, NULL,mainQUEUE_ALERT_TASK_PRIORITY ,&Alert_task);


          vTaskStartScheduler();
    }
    return 0;
}

void LoggerTask(void *pvParameters)
{
    char *level_str[4] = {"ERROR", "INFO","STATUS", "ALERT"};
    uint32_t temp_num;
    Packet Received;
    // Set up the UART which is connected to the virtual COM port
    UARTStdioConfig(0, 115200, SYSTEM_CLOCK);
    UARTprintf("Time\t TaskName\tLogLevel\tMessage\n");
    while(1)
    {
        xQueueReceive( xQueue, &Received, portMAX_DELAY );


        if(Received.ID == LED_TASK)
        {
            UARTprintf("[%dms]     [LED]    \t[%s]\t Name: %s\t Count: %d\n", Received.timestamp,level_str[Received.level] ,Received.message,Received.pkt_led.count);
        }
        if(Received.ID == TEMPERATURE_TASK)
        {
           temp_num = (Received.pkt_temp.temperature_val * 10000);
            UARTprintf("[%dms] [TEMPERATURE]\t[%s]\t %s %d.%d DegC\n", Received.timestamp,level_str[Received.level],Received.message,temp_num/10000,temp_num % 10000);
        }
        if(Received.ID == ALERT_TASK)
        {
            UARTprintf("[%dms]    [ALERT] \t\t[%s]\t %s\t \n", Received.timestamp,level_str[Received.level] ,Received.message);
        }
    }
}

void LEDTask(void *pvParameters)
{
    TimerHandle_t led_timer;
    led_timer = xTimerCreate("LEDTimer10Hz", pdMS_TO_TICKS(100) ,
                                            pdTRUE,  (void*)0, led_timer_handler);

    if((xTimerStart(led_timer, 0)) != pdTRUE)
    {
        while(1);
    }

    /* Suspend Task */
    vTaskSuspend(NULL);

}


void TempTask(void *pvParameters)
{

       TimerHandle_t temp_timer;

       temp_timer= xTimerCreate("TempTimer10Hz", pdMS_TO_TICKS(1000) ,
                                               pdTRUE,  (void*)0, temp_timer_handler);

       if((xTimerStart(temp_timer, 0)) != pdTRUE)
       {
           while(1);


       }
       /* Suspend Task */
       vTaskSuspend(NULL);

}

void AlertTask(void *pvParameters)
{
    uint32_t timestamp;
    Packet Send;
    while(1)
    {
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY);

        timestamp = GetTimeMs();
        Send.ID =ALERT_TASK;
        Send.level = ALERT ;
        Send.message = "TEMPERATURE THRESHOLD CROSSED";
        Send.timestamp = timestamp;

        xQueueSend( xQueue, &Send, 0U );
    }
}
void led_timer_handler()
{
      static int count =1;
      uint32_t timestamp;
      static uint32_t status_led1 = 0;
      static uint32_t status_led2 = GPIO_PIN_0 ;

      Packet Send;
      memset(&Send,0,sizeof(Send));



      status_led1 = status_led1 ^ GPIO_PIN_1;
      status_led2 = status_led2 ^ GPIO_PIN_0;

      timestamp = GetTimeMs();

//     / secs = GetTimeSecs();
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, status_led1);
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, status_led2);

      Send.ID =LED_TASK;
      Send.level = INFO;
      Send.timestamp = timestamp;
      Send.pkt_led.count = count++;
      Send.message = "Deepesh";

      if(xQueueSend( xQueue, &Send, 0U ) != pdTRUE)
      {
      }

}

void temp_timer_handler()
{
       uint32_t timestamp;
       uint16_t regval;
       float temperature;
       Packet Send;
       memset(&Send,0,sizeof(Send));
       timestamp = GetTimeMs();

       regval = I2CGet2Bytes(TMP_SLAVE_ADDRESS, TMP_REG_TEMPERATURE);
       temperature = regval_to_Temperature(regval);

       if(temperature > TEMPERATURE_THRESHOLD)
       {
           xTaskNotifyGive(Alert_task);
       }

       Send.ID =TEMPERATURE_TASK;
       Send.level = INFO;
       Send.message = "Temperature :";
       Send.timestamp = timestamp;
       Send.pkt_temp.temperature_val = temperature;

       xQueueSend( xQueue, &Send, 0U );

}

void RTC_init(uint32_t clock)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);
    HibernateEnableExpClk(clock);
    HibernateClockConfig(HIBERNATE_OSC_LOWDRIVE);
    HibernateRTCEnable();
}

uint32_t GetTimeMs()
{
    float time1, time2;
    uint32_t secs, subsecs,time;
    secs = HibernateRTCGet();
    subsecs =  HibernateRTCSSGet();
    time1 = (float)((secs*1000));
    time2 = (float)((subsecs*1000)/32768);
    time =(uint32_t) (time1+ time2);
    return time;
}

void I2C_init(uint32_t clock)
{

    // Enable I2C2 Peripheral
        SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

        // Enable GPION and configure Pin4 and Pin5
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

        GPIOPinConfigure(GPIO_PN4_I2C2SDA);
        GPIOPinConfigure(GPIO_PN5_I2C2SCL);

        GPIOPinTypeI2CSCL(GPIO_PORTN_BASE, GPIO_PIN_5);
        GPIOPinTypeI2C(GPIO_PORTN_BASE, GPIO_PIN_4);
        I2CMasterInitExpClk(I2C2_BASE, clock, false);
}

/**
 *
 */
uint16_t I2CGet2Bytes(uint8_t target_address, uint8_t register_address)
{
   //
   // Tell the master module what address it will place on the bus when
   // communicating with the slave.  Set the address to LUX_SENSOR
   // (as set in the slave module).  The receive parameter is set to false
   // which indicates the I2C Master is initiating a writes to the slave.  If
   // true, that would indicate that the I2C Master is initiating reads from
   // the slave.
   //
   I2CMasterSlaveAddrSet(I2C2_BASE, target_address, false);

   //
   // Place the data to be sent in the data register
   //
   I2CMasterDataPut(I2C2_BASE, register_address);

   //
   // Initiate send of data from the master.  Since the loopback
   // mode is enabled, the master and slave units are connected
   // allowing us to receive the same data that we sent out.
   //
   I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);

#ifdef BUSY_WAIT
   //
   // Wait until master module is says it's busy. Errata I2C#08
   //
   while(!I2CMasterBusy(I2C2_BASE));
#endif

   //
   // Wait until master module is done
   //
   while(I2CMasterBusy(I2C2_BASE));

   /*
   ** now switch to read mode
   */
   I2CMasterSlaveAddrSet(I2C2_BASE, target_address, true);

   /*
   ** read one byte
   */
   I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

#ifdef BUSY_WAIT
   //
   // Wait until master module is says it's busy. Errata I2C#08
   //
   while(!I2CMasterBusy(I2C2_BASE));
#endif

   //
   // Wait until master module is done
   //
   while(I2CMasterBusy(I2C2_BASE));

   uint32_t data_one = I2CMasterDataGet(I2C2_BASE);

   /*
   ** read one byte
   */
   I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

#ifdef BUSY_WAIT
   //
   // Wait until master module is says it's busy. Errata I2C#08
   //
   while(!I2CMasterBusy(I2C2_BASE));
#endif

   //
   // Wait until master module is done
   //
   while(I2CMasterBusy(I2C2_BASE));

   uint32_t data_two = I2CMasterDataGet(I2C2_BASE);

   return (data_one << 8) | data_two;
}

float regval_to_Temperature(uint16_t regval)
{
    float temperature;
    regval = regval >> 4;
    if(regval > 0x7FF)
    {
        temperature = -1 * (0.0625 * regval);
    }
    else
        temperature = 0.0625 * regval;

    return temperature;
}

/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}
