/*!
 * \file Hive.h
 * \brief Gestion de la ruche
 * \author Samuel.B
*/
#include "hive.h"

/*!
*  \brief Test unitaire 
*
*  Fonction qui permet d'effectuer des test unitaire
*
*  \param UniTest : Un drapeau pour activer ou désactiver des test unitaire 
*  \return void 
*/
void unit_test(UnitTest e_flags)
{
	Hive unit_test_hive = HIVE_Create();
	uint8_t flags = e_flags;

	if(flags & 1)
	{
		printf("[UNIT] WiFi Test Start\n\n");
		Hive_ConnectWiFi();
		printf("[UNIT] WiFi Test Ended\n\n");
	}

	if((flags >> 1) & 1)
	{
		printf("[UNIT] DHT Test Start\n");
		HIVE_ReadDHT(&unit_test_hive);
		printf("[UNIT] DHT Test Ended\n");
	}

	if((flags >> 2) & 1)
	{
		hx711_t dev[4] = {
		{.dout = WEIGHT_SENSOR_DATA_ONE_PIN, .pd_sck = WEIGHT_SENSOR_SCK_ONE_PIN, .gain = HX711_GAIN_A_64},
		{.dout = WEIGHT_SENSOR_DATA_TWO_PIN, .pd_sck = WEIGHT_SENSOR_SCK_TWO_PIN, .gain = HX711_GAIN_A_64},
		{.dout = WEIGHT_SENSOR_DATA_THREE_PIN, .pd_sck = WEIGHT_SENSOR_SCK_THREE_PIN, .gain = HX711_GAIN_A_64},
		{.dout = WEIGHT_SENSOR_DATA_TWO_PIN, .pd_sck = WEIGHT_SENSOR_SCK_TWO_PIN, .gain = HX711_GAIN_A_64}
		};

		size_t length = sizeof(dev) / sizeof(dev[0]);

		for(int i = 0; i < length; i++)
		{
			esp_err_t hx711_is_init = hx711_init(&dev[i]);

			if(hx711_is_init != ESP_OK)
				fprintf(stderr, "Device not found n°%d : %d (%s)\n", i, hx711_is_init, esp_err_to_name(hx711_is_init));
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}

		printf("[UNIT] Weight Test Start\n");
		HIVE_ReadHX711(&unit_test_hive, dev, sizeof(dev) / sizeof(dev[0]));
		printf("[UNIT] Weight Test Ended\n");
	}

	if((flags >> 3) & 1)
	{
		printf("[UNIT] Anemometer Test Start\n");
		HIVE_ReadAnemometer(&unit_test_hive);
		printf("[UNIT] Anemometer Test Ended\n");
	}

	if((flags >> 4) & 1)
	{
		printf("[UNIT] WEATHERCOCK Test Start\n");
		HIVE_ReadWeathercock(&unit_test_hive);
		printf("[UNIT] WEATHERCOCK Test Ended\n");
	}

	printf("[	UNIT TEST RESULT	]\n");
	HIVE_DisplayOutputStream(&unit_test_hive);
	printf("[						]\n");
}

/*!
*  \brief Constructeur 
*
*  Fonction qui permet d'initaliser mon dispositif éléctronique
*
*  \param void : Aucun paramètre
*  \return Une structure Hive
*/
Hive HIVE_Create()
{
	Hive hive;
	memset(&hive, 0, sizeof(hive));
	
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

	// Configuration de l'ADC1 
	adc1_config_width(ADC_WIDTH_BIT_12); // CAN travaillant sur 12 Bit
    adc1_config_channel_atten(WEATHERCOCK_PIN, ADC_ATTEN_DB_11); // Atténuation 

	// Configuration de l'ADC2
    adc2_config_channel_atten(ADC2_CHANNEL_2, ADC_ATTEN_0db ); // Pas d'atténuation 

	return hive;
}
/*!
*  \brief Fonction 
*
*  Fonction qui permet de lancer l'ESP32 dans c'est mesures
*
*  \param Hive* : Un pointeur sur la structure Hive
*  \return void
*/
void HIVE_RunTask(Hive* hive)
{
	hx711_t dev[4] = {
		{.dout = WEIGHT_SENSOR_DATA_ONE_PIN, .pd_sck = WEIGHT_SENSOR_SCK_ONE_PIN, .gain = HX711_GAIN_A_64},
		{.dout = WEIGHT_SENSOR_DATA_TWO_PIN, .pd_sck = WEIGHT_SENSOR_SCK_TWO_PIN, .gain = HX711_GAIN_A_64},
		{.dout = WEIGHT_SENSOR_DATA_THREE_PIN, .pd_sck = WEIGHT_SENSOR_SCK_THREE_PIN, .gain = HX711_GAIN_A_64},
		{.dout = WEIGHT_SENSOR_DATA_TWO_PIN, .pd_sck = WEIGHT_SENSOR_SCK_TWO_PIN, .gain = HX711_GAIN_A_64}
	};
	size_t length = sizeof(dev) / sizeof(dev[0]);

	for(int i = 0; i < length; i++)
	{
		esp_err_t hx711_is_init = hx711_init(&dev[i]);

		if(hx711_is_init != ESP_OK)
			fprintf(stderr, "Device not found n°%d : %d (%s)\n", i, hx711_is_init, esp_err_to_name(hx711_is_init));
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}

	int64_t dt = 0;
	int64_t counter = 0;

	while(true)
	{
		int64_t start = esp_timer_get_time();

		// Lectures
		HIVE_ReadDHT(hive);
		HIVE_ReadHX711(hive, dev, length);
		HIVE_ReadAnemometer(hive);
	 	HIVE_ReadWeathercock(hive);
	
		// Affichage des mesures
		//HIVE_DisplayOutputStream(hive);

		dt = esp_timer_get_time() - start;
		counter += dt;

		if(counter > MINUTES_TO_MICRO(1))
		{
			printf("1 minutes\n");
			counter = 0;
			Hive_ConnectWiFi();
			Hive_PushToDatabase(hive);
		}
	}
}
/*!
*  \brief Fonction 
*
*  Fonction qui permet de d'afficher les données de mesures
*
*  \param Hive* : Un pointeur sur la structure Hive
*  \return void
*/
void HIVE_DisplayOutputStream(Hive* hive)
{
	printf("[DHT22] Temp ext : %d / %d %%\n", hive->external_temperature, hive->external_humidity);
	printf("[DHT11] Temp int : %d / %d %%\n", hive->internal_temperature, hive->internal_humidity);
	printf("[HX711] Poids : %.2f g\n", hive->weight);
	printf("[ANEMOMETER] Vitesse du vent : %.2f m/s\n", hive->wend_speed);
	printf("[WEATHERCOCK] Direction du vent : %s\n", hive->direction);
}
/*!
*  \brief Fonction 
*
*  Fonction qui lit les capteurs de température et d'humiter,
*  DHT11 et DHT22
*
*  \param Hive* : Un pointeur sur la structure Hive
*  \return void
*/
void HIVE_ReadDHT(Hive* hive)
{
	int16_t temperature = 0;
	int16_t humidity = 0;

	if(dht_read_data(DHT_TYPE_AM2301, DHT_22_PIN, &humidity, &temperature) == ESP_OK)
	{
		hive->external_temperature = temperature / 10;
		hive->external_humidity = humidity / 10;
	}
	
	vTaskDelay(100 / portTICK_PERIOD_MS);

	if(dht_read_data(DHT_TYPE_DHT11, DHT_11_PIN, &humidity, &temperature) == ESP_OK)
	{
		hive->internal_temperature = temperature / 10;
		hive->internal_humidity = humidity / 10;
	}
}
/*!
*  \brief Fonction 
*
*  Fonction qui lit les jauges de contraintes
*
*  \param Hive*, hx711_t, size_t : Un pointeur sur la structure Hive, un tableau de hxt711_t et la taille de celui-ci
*  \return void
*/
void HIVE_ReadHX711(Hive* hive, hx711_t* dev, size_t length)
{
	float measures[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	float calibratings[4] = {90.16, 92.16, 91.17, 114.2};

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
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}
	}
	hive->weight = (measures[0] + measures[1]) / (length / 2) +  (measures[2] + measures[3]) / (length / 2); 
}
/*!
*  \brief Fonction 
*
*  Fonction qui lit la vitesse du vent
*
*  \param Hive* : Un pointeur sur la structure Hive
*  \return void
*/
void HIVE_ReadAnemometer(Hive* hive)
{
	bool continue_impulsion_couting = true;
	int64_t deltaTime = 0;
	int64_t deltaCounter = 0;
	uint32_t impulsionCounter = 0;

	while(continue_impulsion_couting)
	{
		int value = 0;
		int64_t start = esp_timer_get_time();
		esp_err_t code = adc2_get_raw(ANEMOMETER_PIN, ADC_WIDTH_12Bit, &value);

		if(code == ESP_OK && value == 4096-1)
			impulsionCounter++;
		else if(code == ESP_ERR_TIMEOUT)
			printf("ADC2 used by Wi-Fi.\n");

		if(deltaCounter > 5000000)
		{
			hive->wend_speed = (2*M_PI*RADIUS*impulsionCounter)/5;
			deltaCounter = 0.0f;
			impulsionCounter = 0;
			continue_impulsion_couting = false;
		}

		vTaskDelay(10 / portTICK_PERIOD_MS);

		deltaTime = (esp_timer_get_time() - start);
		deltaCounter += deltaTime;
	}
	continue_impulsion_couting = true; 
}
/*!
*  \brief Fonction 
*
*  Fonction qui lit la direction du vent
*
*  \param Hive* : Un pointeur sur la structure Hive
*  \return void
*/
void HIVE_ReadWeathercock(Hive* hive)
{
	char* direction[20] = {"Nord","Nord Ouest","Ouest","Sud Ouest","Sud","Sud Est","Est","Nord Est"};
	char error[] = "Error";

	uint16_t sum = 0;
	uint16_t average_value = 0;
	int direction_index = 0;

	for(int i = 0; i < 4; i++)
	{
		sum += adc1_get_raw(WEATHERCOCK_PIN);
		vTaskDelay(5 / portTICK_PERIOD_MS);
	}

	average_value = sum / 4;
	
	if(average_value==0)
		direction_index = -1;
	else if(average_value < 400)
		direction_index = 2;
	else if(average_value >= 400 && average_value<700)
		direction_index = 1;
	else if(average_value >= 700 && average_value<1500)
		direction_index = 0;
	else if(average_value >= 1500 && average_value<2000)
		direction_index = 3;
	else if(average_value >= 2000 && average_value<2800)
		direction_index = 7;
	else if(average_value >= 2800 && average_value<3300)
		direction_index = 4;
	else if(average_value >= 3300 && average_value<3800)
		direction_index = 5;
	else 
		direction_index = 6;

	if(direction_index >= 0)
		strcpy(hive->direction, direction[direction_index]);
	else 
		strcpy(hive->direction, error);
	
}
/*!
*  \brief Fonction 
*
*  Fonction qui lit se connecte a un serveur web pour
*  enregristrer les données de mesures dans la base de données par 
*  une requêtes HTTP
*
*  \param Hive* : Un pointeur sur la structure Hive
*  \return void
*/
void Hive_PushToDatabase(Hive* hive)
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
	sprintf(szHtppRequest, "GET http://192.168.0.21:8080/Ruches/index.php?dht22_t=%d&dht22_h=%d&dht11_t=%d&dht11_h=%d&speed=%.2f&weight=%.2f&orientation=%s\r\n\r\n", hive->external_temperature, hive->external_humidity, hive->internal_temperature, hive->internal_humidity, hive->wend_speed, hive->weight, hive->direction);
    

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

	ESP_ERROR_CHECK(esp_wifi_stop());
	ESP_ERROR_CHECK(esp_wifi_deinit());
}


static EventGroupHandle_t s_wifi_event_group; // Evenement de groupe
static const char *TAG = "WiFi"; // Label
static int s_retry_num = 0; // Compteur nombre de tentative de connexion

/*!
*  \brief Fonction 
*
*  Fonction qui gére les événement lié au WiFi,
*  si un connexion a pus être établie ou une déconnexion
*  ou une impossibiliter de se connecté au au WiFi

*  \param void*, esp_event_base_t, int32_t, void* : sert a rien ici, le type de l'événement, le flags, les données   
*  \return void
*/
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
	{
        esp_wifi_connect();
    } 
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
        if (s_retry_num < ESP_MAXIMUM_RETRY) 
		{
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "Tentative de connexion n°%d.", s_retry_num);
        } 
		else 
		{
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"Echec de connexion !");
    } 
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
	{
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
/*!
*  \brief Fonction 
*
*  Fonction qui se connecte au WiFi
*
*  \param void :
*  \return void
*/
void Hive_ConnectWiFi()
{
	// Initialise le NS 
	esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

	// Création d'un group d'événement
	s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

	// Application d'un config par défaut pour le WIFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	// Enregristrer 2 événement  
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID, // Nom du réseau
            .password = ESP_WIFI_PASS // Clé WiFi 
        },
    };

	// Mettre a jour la configuration
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    // Attendre que la connexion soit établie ou échecs
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    // Gestion des erreurs
    if (bits & WIFI_CONNECTED_BIT) 
	{
        ESP_LOGI(TAG, "Connexion au WiFi réussis SSID:%s password:%s", ESP_WIFI_SSID, ESP_WIFI_PASS);
    } 
	else if (bits & WIFI_FAIL_BIT) 
	{
        ESP_LOGI(TAG, "Impossible de connecter au WiFi SSID:%s, password:%s", ESP_WIFI_SSID, ESP_WIFI_PASS);
    } else 
	{
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

	// Supprimer les événement
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
    vEventGroupDelete(s_wifi_event_group);
}