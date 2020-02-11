// ESP Headers
#include "esp_log.h"
#include "driver/timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "sdkconfig.h"

// Own Headers
#include "dhtxx/dht.h"

void app_main(void)
{
	DHT sensor = DHT_InitializeSensor(DHT_11, GPIO_NUM_23);

	esp_task_wdt_deinit();
	esp_task_wdt_reset();

	while(true)
	{
		if(DHT_Read(&sensor) == true)
		{
			printf("%d %d\n", sensor.temperature, sensor.humidity);
		}
	}


}
