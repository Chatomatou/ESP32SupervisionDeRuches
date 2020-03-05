#ifndef HIVE_H_
#define HIVE_H_

#include "driver/gpio.h"
#include "esp_task_wdt.h"

#include "freertos/task.h"
#include "freertos/semphr.h"

#include <dht.h>

// Standard lib
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

enum 
{
	DHT_11_PIN = GPIO_NUM_23,
	DHT_22_PIN = GPIO_NUM_22,
	
	WEIGHT_SENSOR_SCK_ONE_PIN = GPIO_NUM_32,
	WEIGHT_SENSOR_DATA_ONE_PIN = GPIO_NUM_33,
	WEIGHT_SENSOR_SCK_TWO_PIN = GPIO_NUM_25,
	WEIGHT_SENSOR_DATA_TWO_PIN = GPIO_NUM_26,
	WEIGHT_SENSOR_SCK_THREE_PIN = GPIO_NUM_27,
	WEIGHT_SENSOR_DATA_THREE_PIN = GPIO_NUM_14,
	WEIGHT_SENSOR_SCK_FOUR_PIN = GPIO_NUM_12,
	WEIGHT_SENSOR_DATA_FOOR_PIN = GPIO_NUM_13,
	
	ANEMO_PIN = GPIO_NUM_2,
	GIR_PIN = GPIO_NUM_36
};

typedef struct 
{
	int16_t internal_temperature;
	int16_t internal_humidity;
	
	int16_t external_temperature;
	int16_t external_humidity;
	
	int16_t weight_one;
	int16_t weight_two;
	int16_t weight_three;
	int16_t weight_foor;
	
	void	(*measures_dht11) 				(void* self);
	void 	(*measures_dht22) 				(void* self);
	void 	(*measures_weight_one) 			(void* self);
	void 	(*measures_weight_two) 			(void* self);
	void 	(*measures_weight_three) 		(void* self);
	void 	(*measures_weight_foor) 		(void* self);
	void (*test1) (void* self);
	void (*test2) (void* self);
}Hive;


Hive HIVE_Create();
void HIVE_RunTask(Hive* hive);


void measures_dht11_task(void* pvParameter);
void measures_dht22_task(void* pvParameter);
void measures_weight_one_task(void* pvParameter);
void measures_weight_two_task(void* pvParameter);
void measures_weight_three_task(void* pvParameter);
void measures_weight_foor_task(void* pvParameter);
void test_task1(void* pvParameter);
void test_task2(void* pvParameter);



#endif 
