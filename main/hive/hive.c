#include "hive.h"

Hive HIVE_Create()
{
	Hive hive;

	hive.external_temperature = 0;
	hive.external_humidity = 0;
 	hive.internal_temperature = 0;
	hive.internal_humidity = 0;
	hive.weight = 0.0f;
	hive.wend_speed = 0.0f;
 
 

	/* Configuration du convertisseur analogique numérique */
	/*
	
	The ADC driver API supports ADC1 (8 channels, attached to GPIOs 32 - 39), 
	and ADC2 (10 channels, attached to GPIOs 0, 2, 4, 12 - 15 and 25 - 27). 
	However, the usage of ADC2 has some restrictions for the application:
	Source : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html#_CPPv416hall_sensor_readv

	Je vais configurer l'ADC n°2 car mon énomomètre et au pin 2 
	
	ADC2 is used by the Wi-Fi driver. Therefore the application can only use ADC2 when the Wi-Fi driver has not started.
	GPIO 0, 2, 4 and 15 cannot be used due to external connections for different purposes
	*/


	//adc2_config_width(ADC_WIDTH_BIT_12); // Le CAN travaillera sur 12 BIT
    adc2_config_channel_atten(ADC2_CHANNEL_2, ADC_ATTEN_0db ); // Pas d'atténuation 

	return hive;
}

void HIVE_RunTask(Hive* hive)
{
 	int16_t temperature = 0;
	int16_t humidity = 0;

	hx711_t dev[4] = {
		{.dout = WEIGHT_SENSOR_DATA_ONE_PIN, .pd_sck = WEIGHT_SENSOR_SCK_ONE_PIN, .gain = HX711_GAIN_A_64},
		{.dout = WEIGHT_SENSOR_DATA_TWO_PIN, .pd_sck = WEIGHT_SENSOR_SCK_TWO_PIN, .gain = HX711_GAIN_A_64},
		{.dout = WEIGHT_SENSOR_DATA_THREE_PIN, .pd_sck = WEIGHT_SENSOR_SCK_THREE_PIN, .gain = HX711_GAIN_A_64},
		{.dout = WEIGHT_SENSOR_DATA_TWO_PIN, .pd_sck = WEIGHT_SENSOR_SCK_TWO_PIN, .gain = HX711_GAIN_A_64}
	};

	float measures[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	float calibratings[4] = {90.16, 92.16, 91.17, 114.2};

	bool continue_impulsion_couting = true;
	int64_t deltaTime = 0;
	int64_t deltaCounter = 0;
	uint32_t impulsionCounter = 0;

	while(true)
	{
		if(dht_read_data(DHT_TYPE_AM2301, DHT_22_PIN, &humidity, &temperature) == ESP_OK)
		{
			hive->external_temperature = temperature / 10;
			hive->external_humidity = humidity / 10;
		}
		
		vTaskDelay(2000 / portTICK_PERIOD_MS);

		if(dht_read_data(DHT_TYPE_DHT11, DHT_11_PIN, &humidity, &temperature) == ESP_OK)
		{
			hive->internal_temperature = temperature / 10;
			hive->internal_humidity = humidity / 10;
		}
				
		vTaskDelay(2000 / portTICK_PERIOD_MS);
		
		size_t length = sizeof(dev) / sizeof(dev[0]);

		for(int i = 0; i < length; i++)
		{
			esp_err_t hx711_is_init = hx711_init(&dev[i]);

			if(hx711_is_init != ESP_OK)
				fprintf(stderr, "Device not found n°%d : %d (%s)\n", i, hx711_is_init, esp_err_to_name(hx711_is_init));
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}

	 
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
		hive->weight = (measures[0] + measures[1]) / (length / 2) +  (measures[2] + measures[3]) / (length / 2); 

		vTaskDelay(2000 / portTICK_PERIOD_MS);
 
		while(continue_impulsion_couting)
		{
			int value = 0;
			int64_t start = esp_timer_get_time();
			esp_err_t code = adc2_get_raw(ADC2_CHANNEL_2, ADC_WIDTH_12Bit, &value);

			if(code == ESP_OK && value == 4096-1)
				impulsionCounter++;
			else if(code == ESP_ERR_TIMEOUT)
				printf("ADC2 used by Wi-Fi.\n");

			 

			if(deltaCounter > 5000000)
			{
				printf("%d\n", impulsionCounter);
				hive->wend_speed = (2*M_PI*RADIUS*impulsionCounter)/5;
				deltaCounter = 0.0f;
				impulsionCounter = 0;
 				continue_impulsion_couting = false;
			}

			vTaskDelay(10 / portTICK_PERIOD_MS);

			deltaTime = ( esp_timer_get_time() - start );
			deltaCounter += deltaTime;

	 
		}
		continue_impulsion_couting = true; 

		printf("[DHT22] Temp ext : %d / %d %%\n", hive->external_temperature, hive->external_humidity);
		printf("[DHT11] Temp int : %d / %d %%\n", hive->internal_temperature, hive->internal_humidity);
		printf("[HX711] Poids : %.2f g\n", hive->weight);
		printf("[ENEMO] Vitesse du vent : %.2f m/s\n", hive->wend_speed);

		Hive_PushToDatabse(&hive);
	}
}

void periodic_timer_display_callback(void* self)
{
	Hive* pHive = (Hive**)self;

	if(pHive == NULL)
	{
		fprintf(stderr, "[SupervisionDebug] pHive n'est pas allouer.\n");
		return;
	}

	printf("[DHT22] Temp ext : %d / %d %%\n", pHive->external_temperature, pHive->external_humidity);
	printf("[DHT11] Temp int : %d / %d %%\n", pHive->internal_temperature, pHive->internal_humidity);
	printf("[HX711] Poids : %.2f g\n", pHive->weight);
	printf("[ENEMO] Vitesse du vent : %.2f m/s\n", pHive->wend_speed);
}

void Hive_PushToDatabse(Hive* hive)
{
	struct hostent* host = gethostbyname("192.168.0.21");

    if(host == NULL)
    {
        printf("Host not found\n");
    }
    else 
    {
        printf("host found\n");
    }

    int sockid = socket(AF_INET, SOCK_STREAM, 0);

    if(sockid < 0 )
    {
        perror("socket()");
    }
    else 
    {
        printf("socket created.\n");
    }
    struct sockaddr_in sockname;

    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(8080);
    sockname.sin_addr.s_addr = inet_addr("192.168.0.21");

    if(connect(sockid, (struct sockaddr*)& sockname, sizeof(sockname)) != 0)
    {
        close(sockid);
        perror("connect()");
    }
    else 
    {
        printf("socket connected.\n");
    }
	char szHtppRequest[256];
	sprintf(szHtppRequest, "GET http://192.168.0.21:8080/Ruches/index.php?dht22_t=%d&dht22_h=%d&dht11_t=%d&dht11_h=%d&speed=%.2f&weight=%.2f&orientation=%d\r\n\r\n", hive->external_temperature, hive->external_humidity, hive->internal_temperature, hive->internal_humidity, hive->wend_speed, hive->weight, -1);
    

    if(!send(sockid, szHtppRequest, strlen(szHtppRequest), 0))
    {
        close(sockid);
        perror("send()");
    }

    char szTemp[255];
    memset(szTemp, '\0', sizeof(szTemp));
    while(recv(sockid, szTemp, 255, 0))
    {
        printf("%s\n", szTemp);
    }        
        
    shutdown(sockid, 2);
    close(sockid);
}

 
 