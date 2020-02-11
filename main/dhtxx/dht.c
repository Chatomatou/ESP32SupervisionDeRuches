/*
 * dht.c
 *
 *  Created on: 8 févr. 2020
 *      Author: adolphefilsdedieu
 */

#include "dht.h"

static int64_t last_read_time = -2000000;

static int _dht_wait_or_timeout(const DHT* dht, uint16_t microSeconds, int level)
{
    int micros_ticks = 0;

    while(gpio_get_level(dht->pin) == level) {
        if(micros_ticks++ > microSeconds)
            return DHT_TIMEOUT_ERROR;
        ets_delay_us(1);
    }
    return micros_ticks;
}

static int _dht__check_response(const DHT* dht) {
    /* Wait for next step ~80us*/
    if(_dht_wait_or_timeout(dht, 80, 0) == DHT_TIMEOUT_ERROR)
        return DHT_TIMEOUT_ERROR;

    /* Wait for next step ~80us*/
    if(_dht_wait_or_timeout(dht, 80, 1) == DHT_TIMEOUT_ERROR)
        return DHT_TIMEOUT_ERROR;

    return DHT_OK;
}

static DHT_STATUS _dht_checksum(const uint8_t data[])
{
    if(data[4] == (data[0] + data[1] + data[2] + data[3]))
        return DHT_OK;
    return DHT_CRC_ERROR;
}

static void _dht_start_signal(const DHT* dht)
{
	gpio_set_direction(dht->pin, GPIO_MODE_OUTPUT);
	gpio_set_level(dht->pin, 0);
	ets_delay_us(20000);
	gpio_set_level(dht->pin, 1);
	ets_delay_us(40);
	gpio_set_direction(dht->pin, GPIO_MODE_INPUT);
}


DHT DHT_InitializeSensor(const DHT_TYPE type, const gpio_num_t pin)
{
	DHT sensor;

	sensor.type = type;
	sensor.pin = pin;
	sensor.status = DHT_OK;
	sensor.humidity = -1;
	sensor.temperature = -1;

    vTaskDelay(1000 / portTICK_PERIOD_MS);
	return sensor;
}
bool DHT_Read(DHT* dht)
{
	if(dht == NULL)
		return false;

	if(esp_timer_get_time() - 2000000 < last_read_time)
		return true;

	last_read_time = esp_timer_get_time();
	uint8_t data[5] = {0, 0, 0, 0, 0};


	_dht_start_signal(dht);

	if(_dht__check_response(dht) == DHT_TIMEOUT_ERROR)
	{
		dht->status = DHT_TIMEOUT_ERROR;
		dht->humidity = -1;
		dht->temperature = -1;
		return false;
	}


	/* Read response */
	for(int i = 0; i < 40; i++) {
		/* Initial data */
		if(_dht_wait_or_timeout(dht, 50, 0) == DHT_TIMEOUT_ERROR)
		{
			dht->status = DHT_TIMEOUT_ERROR;
			dht->humidity = -1;
			dht->temperature = -1;
			return false;
		}

		if(_dht_wait_or_timeout(dht, 70, 1) > 28) {
			data[i/8] |= (1 << (7-(i%8)));
		}
	}

	if(_dht_checksum(data) != DHT_CRC_ERROR)
	{
		dht->status = DHT_OK;

		if(dht->type == DHT_11)
		{
			dht->temperature = data[2];
			dht->humidity = data[0];
		}
		else
		{
 			  float fh = data[0];
			  fh *= 256;
			  fh += data[1];
			  fh *= 0.1;
			  dht->humidity = fh;

			  float ft = data[2] & 0x7f;
			  ft *= 256;
			  ft += data[3];
			  ft *= 0.1;
			  if (data[2] & 0x80) {
			    ft *= -1;
			  }
			  dht->temperature = ft;
		}

		return true;
	}
	return false;
}
