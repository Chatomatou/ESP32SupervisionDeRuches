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
	//xTaskCreate(hive->measures_dht22, "test2", STACK_SIZE, hive, 5, NULL);
 	//xTaskCreate(hive->measures_dht11, "test1", STACK_SIZE, hive, 5, NULL);
	DHT dht22 = DHT_InitializeSensor(DHT_22, DHT_22_PIN);
	DHT dht11 = DHT_InitializeSensor(DHT_11, DHT_11_PIN);

	while(true)
	{
 
		if(DHT_Read(&dht22))
		{
			hive->external_temperature = dht22.temperature;
			hive->external_humidity = dht22.humidity;
		}

        vTaskDelay(2000 / portTICK_RATE_MS);

		if(DHT_Read(&dht11))
		{
			hive->internal_temperature = dht11.temperature;
			hive->internal_humidity = dht11.humidity;
		}
 
 
		printf("DHT22 [%d %d]\n", hive->external_temperature, hive->external_humidity);
		printf("DHT11 [%d %d]\n", hive->internal_temperature, hive->internal_humidity);
	}
}


void measures_dht11_task(void* pvParameter)
{
	DHT dht11 = DHT_InitializeSensor(DHT_11, DHT_11_PIN);
	Hive* pHive = (Hive**)pvParameter;

	if(pHive == NULL)
	{
		fprintf(stderr, "FAILURE::HIVE::IS_NULL\n");
	}

	while(true)
	{
        vTaskDelay(2000 / portTICK_RATE_MS);

		if(DHT_Read(&dht11))
		{
			pHive->internal_temperature = dht11.temperature;
			pHive->internal_humidity = dht11.humidity;
		}
 

		printf("DHT11 [%d %d]\n", pHive->internal_temperature, pHive->internal_humidity);
	}
}
void measures_dht22_task(void* pvParameter)
{
	DHT dht22 = DHT_InitializeSensor(DHT_22, DHT_22_PIN);
	Hive* pHive = (Hive**)pvParameter;

	if(pHive == NULL)
	{
		fprintf(stderr, "FAILURE::HIVE::IS_NULL\n");
	}

	while(true)
	{
		vTaskDelay(2000 / portTICK_RATE_MS);
 

		if(DHT_Read(&dht22))
		{
			pHive->external_temperature = dht22.temperature;
			pHive->external_humidity = dht22.humidity;
		}

 
		printf("DHT22 [%d %d]\n", pHive->external_temperature, pHive->external_humidity);
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