#ifndef HIVE_H_
#define HIVE_H_

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_task_wdt.h"


#include "freertos/task.h"
#include "freertos/semphr.h"

#include <dht.h>
#include <hx711.h>

// Standard lib
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>



#define RADIUS 0.045 // Rayon de l'anémomètre 


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

	float weight;
	float wend_speed;
}Hive;


Hive HIVE_Create();
void HIVE_RunTask(Hive* hive);


void periodic_timer_display_callback(void* self);

 
 
#endif 
