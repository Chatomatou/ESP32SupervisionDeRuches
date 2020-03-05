#include "hive/hive.h"

 

void app_main()
{
    Hive hive = HIVE_Create();
    HIVE_RunTask(&hive);
}

