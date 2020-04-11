/*
	Biczo Samuel - Hive Supervision
	
    This project was make in school, for BTS SNIR.

	License GPL
	~~~~~~~
	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>
	
	Last Updated: 10/04/2020
*/
/**
 * \file main.c
 * \brief Programme de tests.
 * \author Samuel.B
 * \version 0.3
 * \date 10 avril 2020
 *
 * Programme pour la supervision de la ruche.
 *
 */

#include "hive/hive.h"

#define UNIT_TEST 0

void app_main(void)
{
#if UNIT_TEST == 1 
    UnitTest flags = DHT_UNIT_TEST | WEIGHT_UNIT_TEST | ANEMOMETER_UNIT_TEST;
    unit_test(flags);
    while(true)
        vTaskDelay(1000 / portTICK_PERIOD_MS);
#else 
    Hive hive = HIVE_Create();
    HIVE_RunTask(&hive);
#endif
}
