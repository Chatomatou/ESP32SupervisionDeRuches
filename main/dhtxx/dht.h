/*
 * test.h
 *
 *  Created on: 8 févr. 2020
 *      Author: adolphefilsdedieu
 */

#ifndef DHT_H_
#define DHT_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp32/rom/ets_sys.h"

typedef enum DHT_STATUS {
    DHT_CRC_ERROR = -2,
    DHT_TIMEOUT_ERROR,
    DHT_OK
}DHT_STATUS;

typedef enum DHT_TYPE
{
	DHT_11,
	DHT_22
}DHT_TYPE;

typedef struct DHT
{
	DHT_TYPE type;
	DHT_STATUS status;
	gpio_num_t pin;
	int8_t humidity;
	int16_t temperature;
}DHT;



DHT DHT_InitializeSensor(const DHT_TYPE type, const gpio_num_t pin);
bool DHT_Read(DHT* dht);



#endif /* TEST_H_ */
