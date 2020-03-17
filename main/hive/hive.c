#include "hive.h"

Hive HIVE_Create()
{
	esp_task_wdt_deinit();
	esp_task_wdt_reset();
	
	
	Hive hive;

	memset(&hive, 0, sizeof(hive));

	hive.measures_dht11 = measures_dht11_task;
	hive.measures_dht22 = measures_dht22_task;
	hive.measures_weight = measures_weight_task;
 
	hive.test1 = test_task1;
	hive.test2 = test_task2;
 	
	return hive;
}
void HIVE_RunTask(Hive* hive)
{
	xTaskCreate(hive->measures_dht11, "dht_11", configMINIMAL_STACK_SIZE * 3, hive, 5, NULL);
    xTaskCreate(hive->measures_dht22, "dht_22", configMINIMAL_STACK_SIZE * 3, hive, 5, NULL);
    xTaskCreate(hive->measures_weight, "weight", configMINIMAL_STACK_SIZE * 3, hive, 5, NULL);
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

void measures_weight_task(void* pvParameter)
{
	Hive* pHive = (Hive**)pvParameter;

	hx711_t dev[4] = {
		{.dout = WEIGHT_SENSOR_DATA_ONE_PIN, .pd_sck = WEIGHT_SENSOR_SCK_ONE_PIN, .gain = HX711_GAIN_A_64},
		{.dout = WEIGHT_SENSOR_DATA_TWO_PIN, .pd_sck = WEIGHT_SENSOR_SCK_TWO_PIN, .gain = HX711_GAIN_A_64},
		{.dout = WEIGHT_SENSOR_DATA_THREE_PIN, .pd_sck = WEIGHT_SENSOR_SCK_THREE_PIN, .gain = HX711_GAIN_A_64},
		{.dout = WEIGHT_SENSOR_DATA_TWO_PIN, .pd_sck = WEIGHT_SENSOR_SCK_TWO_PIN, .gain = HX711_GAIN_A_64}
	};

	float measures[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	float calibratings[4] = {90.16, 92.16, 91.17, 114.2};

	size_t length = sizeof(dev) / sizeof(dev[0]);

	for(int i = 0; i < length; i++)
	{
		esp_err_t hx711_is_init = hx711_init(&dev[i]);

		if(hx711_is_init != ESP_OK)
			fprintf(stderr, "Device not found n°%d : %d (%s)\n", i, hx711_is_init, esp_err_to_name(hx711_is_init));
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}

	while(true)
	{
		for(int i = 0; i < length; i++)
		{
			esp_err_t wait = hx711_wait(&dev[i], 500);

			if(wait != ESP_OK)
			{
				fprintf(stderr, "Device not found: %d (%s)\n", wait, esp_err_to_name(wait));
			}
			else 
			{
				int32_t data = 0;

				esp_err_t read_ok = hx711_read_data(&dev[i], &data);

				if(read_ok != ESP_OK)
				{
					fprintf(stderr, "Could not read data: %d (%s)\n", read_ok, esp_err_to_name(read_ok));
					continue;
				}

				measures[i] = (data < 0) ? -data / calibratings[i] : data / calibratings[i];
				vTaskDelay(2000 / portTICK_PERIOD_MS);
			}
		}

		pHive->weight = (measures[0] + measures[1]) / (length / 2) +  (measures[2] + measures[3]) / (length / 2); 
		printf("Weight: %.2fg\n", pHive->weight);
	}
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
 