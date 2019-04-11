/*
 * main.h
 *
 *  Created on: Mar 28, 2015
 *      Author: akobyljanec
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// System clock rate, 120 MHz
#define SYSTEM_CLOCK    120000000U

#define TMP_SLAVE_ADDRESS           (0X48)
/**
 * TMP102 Reg address
 */
#define TMP_REG_TEMPERATURE         (0x00)
#define TMP_REG_CONFIGURATION       (0x01)
#define TMP_REG_TLOW                (0x02)
#define TMP_REG_THIGH               (0x03)
#define TMP_REG_TEMPERATURE         (0x00)
#define TMP_REG_CONFIGURATION       (0x01)
#define TMP_REG_TLOW                (0x02)
#define TMP_REG_THIGH               (0x03)

#define TEMPERATURE_THRESHOLD         (22)


typedef enum
{
    ERROR = 0,
    INFO = 1,
    STATUS = 2,
    ALERT = 3,
}LogLevel_t;

typedef enum TID_t
{
    LED_TASK = 0,
    TEMPERATURE_TASK = 1,
    ALERT_TASK = 2,
}TID_t;

typedef struct Temperature_Packet
{
    float temperature_val;
}Temperature_Packet;

typedef struct Led_Packet
{
    uint32_t count;
}Led_Packet;

typedef struct Packet
{
    TID_t ID;
    LogLevel_t level;
    uint32_t timestamp;
    char* message;
    union
    {
      Temperature_Packet pkt_temp;
      Led_Packet pkt_led;
    };
}Packet;




/**
 * Function Declarations
 */
void LEDTask(void *pvParameters);
void LoggerTask(void *pvParameters);
void TempTask(void *pvParamerters);
void AlertTask(void *pvParameters);
void led_timer_handler();
void temp_timer_handler();
void RTC_init(uint32_t clock);
uint32_t GetTimeMs();
void I2C_init(uint32_t clock);
uint16_t I2CGet2Bytes(uint8_t target_address, uint8_t register_address);
float regval_to_Temperature(uint16_t regval);

#endif /* MAIN_H_ */
