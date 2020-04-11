/*!
 * \file Hive.h
 * \brief Gestion de la ruche
 * \author Samuel.B
*/

#ifndef HIVE_H
#define HIVE_H

// Bibliothèque standard du C 
#include <stdio.h> // Manipuler les flux d'entrée et de sorties
#include <stdbool.h> // Ajouter l'utilisation du type bool en C 
#include <stdlib.h> // Bibliothèque utilitaire
#include <string.h> // Algorithmes pour la manipulation de chaine C
#include <stdint.h> // typedef explicite de la tailles en mémoire des entiers
#include <math.h> // Algorithmes mathématiques

// Bibliothèque system pour le réseau
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

// Bibliothèque pour manipuler les fonctionnaliter du SDK de l'ESP 32
#include "driver/gpio.h" 
#include "driver/adc.h" 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

// Bibliothèque capteurs
#include <dht.h>  
#include <hx711.h> 

#define MINUTES_TO_MICRO(X) (X / 1.6666666666666667e-8) // Macron qui convertit des minutes en en microsecondes (µs)

#define RADIUS 0.045 // Rayon de l'anémomètre 

#define ESP_WIFI_SSID      "SFR-1c90" // Nom du réseau
#define ESP_WIFI_PASS      "QN1JHNJK3J65" // Clé WiFi
#define ESP_MAXIMUM_RETRY       5 // Nombre de tentative 

#define WIFI_CONNECTED_BIT BIT0 // BIT en cas de succès
#define WIFI_FAIL_BIT      BIT1 // BIT en cas d'erreur


/**
 * \enum UnitTest
 * \brief Constants
 *
 * UnitTest est une série de constantes prédéfinie pour effectuer
 * un mécanisme de flags pour activer ou désactiver les test unitaire qu'on 
 * souhait effectuer ou ne pas faire.
 */
typedef enum UnitTest
{
	WIFI_UNIT_TEST 			= 1,
	DHT_UNIT_TEST  			= 2,
	WEIGHT_UNIT_TEST  		= 4,
	ANEMOMETER_UNIT_TEST 	= 8,
	WEATHERCOCK_UNIT_TEST 	= 16
}UnitTest;

/**
 * \enum HivePins
 * \brief Constantes des broches.
 *
 * HivePins est une série de constantes prédéfinie pour faciliter le lien 
 * entre le capteur et sa broche.
 */
enum HivePins
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
	
	ANEMOMETER_PIN = ADC2_CHANNEL_2,
	WEATHERCOCK_PIN = ADC1_GPIO36_CHANNEL
};

/**
 * \struct Hive
 * \brief Objet représentant le dispositif éléctronique sous la ruche.
 *
 * Hive est un petit objet qui contient toutes les variables. 
 * Permetant de sauvegarder en mémoire les informations de mesures.
 */
typedef struct 
{	 
	int16_t internal_temperature;
	int16_t internal_humidity;
	
	int16_t external_temperature;
	int16_t external_humidity;

	float weight;
	float wend_speed;
	
	char direction[20];
}Hive;

/**********************************/
/************PROTOTYPES************/
/**********************************/

/*!
*  \brief Test unitaire 
*
*  Fonction qui permet d'effectuer des test unitaire
*
*  \param UniTest : Un drapeau pour activer ou désactiver des test unitaire 
*  \return void 
*/
void unit_test(UnitTest e_flags);
/*!
*  \brief Constructeur 
*
*  Fonction qui permet d'initaliser mon dispositif éléctronique
*
*  \param void : Aucun paramètre
*  \return Une structure Hive
*/
Hive HIVE_Create();
/*!
*  \brief Fonction 
*
*  Fonction qui permet de lancer l'ESP32 dans c'est mesures
*
*  \param Hive* : Un pointeur sur la structure Hive
*  \return void
*/
void HIVE_RunTask(Hive* hive);
/*!
*  \brief Fonction 
*
*  Fonction qui permet de d'afficher les données de mesures
*
*  \param Hive* : Un pointeur sur la structure Hive
*  \return void
*/
void HIVE_DisplayOutputStream(Hive* hive);
/*!
*  \brief Fonction 
*
*  Fonction qui lit les capteurs de température et d'humiter,
*  DHT11 et DHT22
*
*  \param Hive* : Un pointeur sur la structure Hive
*  \return void
*/
void HIVE_ReadDHT(Hive* hive);
/*!
*  \brief Fonction 
*
*  Fonction qui lit les jauges de contraintes
*
*  \param Hive*, hx711_t, size_t : Un pointeur sur la structure Hive, 
*                                  un tableau de hxt711_t et la taille de celui-ci
*  \return void
*/
void HIVE_ReadHX711(Hive* hive, hx711_t* dev, size_t length);
/*!
*  \brief Fonction 
*
*  Fonction qui lit la vitesse du vent
*
*  \param Hive* : Un pointeur sur la structure Hive
*  \return void
*/
void HIVE_ReadAnemometer(Hive* hive);
/*!
*  \brief Fonction 
*
*  Fonction qui lit la direction du vent
*
*  \param Hive* : Un pointeur sur la structure Hive
*  \return void
*/
void HIVE_ReadWeathercock(Hive* hive);
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
void Hive_PushToDatabase(Hive* hive);
/*!
*  \brief Fonction 
*
*  Fonction qui se connecte au WiFi
*
*  \param void :
*  \return void
*/
void Hive_ConnectWiFi();

#endif 
