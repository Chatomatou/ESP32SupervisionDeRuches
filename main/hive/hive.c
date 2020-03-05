#include "hive.h"

Hive HIVE_Create()
{
	esp_task_wdt_deinit();
	esp_task_wdt_reset();
	
	
	Hive hive;

	memset(&hive, 0, sizeof(hive));

	hive.measures_dht11 = measures_dht11_task;
	hive.measures_dht22 = measures_dht22_task;
	hive.measures_weight_one = measures_weight_one_task;
	hive.measures_weight_two = measures_weight_two_task;
	hive.measures_weight_three = measures_weight_three_task;
	hive.measures_weight_foor = measures_weight_foor_task;
	hive.test1 = test_task1;
	hive.test2 = test_task2;

 	
	return hive;
}
void HIVE_RunTask(Hive* hive)
{
	xTaskCreate(hive->measures_dht11, "dht_11", configMINIMAL_STACK_SIZE * 3, hive, 5, NULL);
    xTaskCreate(hive->measures_dht22, "dht_22", configMINIMAL_STACK_SIZE * 3, hive, 5, NULL);
}


void measures_dht11_task(void* pvParameter)
{
	Hive* pHive = (Hive**)pvParameter;

 	while (true)
    {
		int16_t temperature = 0;
		int16_t humidity = 0;

        if (dht_read_data(DHT_TYPE_AM2301, DHT_22_PIN, &humidity, &temperature) == ESP_OK)
       	{
			pHive->external_temperature = temperature / 10;
			pHive->external_humidity = humidity / 10;
			printf("DHT22 > %d%% %d°C\n", pHive->external_humidity, pHive->external_temperature);
	   	}
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }	 
}
void measures_dht22_task(void* pvParameter)
{
	Hive* pHive = (Hive**)pvParameter;

 	while (true)
    {
		int16_t temperature = 0;
		int16_t humidity = 0;

        if (dht_read_data(DHT_TYPE_DHT11, DHT_11_PIN, &humidity, &temperature) == ESP_OK)
		{
			pHive->internal_temperature = temperature / 10;
			pHive->internal_humidity = humidity / 10;
			printf("DHT11 > %d%% %d°C\n", pHive->internal_humidity, pHive->internal_temperature);
		}
			
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }	 
}

void measures_weight_one_task(void* pvParameter)
{
	fprintf(stderr, "ERRROR::NOT_IMPLEMENTED\n");
}
void measures_weight_two_task(void* pvParameter)
{
	fprintf(stderr, "ERRROR::NOT_IMPLEMENTED\n");
}
void measures_weight_three_task(void* pvParameter)
{
	fprintf(stderr, "ERRROR::NOT_IMPLEMENTED\n");
}
void measures_weight_foor_task(void* pvParameter)
{
	fprintf(stderr, "ERRROR::NOT_IMPLEMENTED\n");
}

void test_task1(void* pvParameter)
{
	while(true)
	{
		printf("first_task executed\n");
	}
}

void test_task2(void* pvParameter)
{
	//Hive* pHive = (Hive**)pvParameter;

 
	while(true)
	{
 
		printf("second_task executed\n");
	}
}