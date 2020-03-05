
idf_component_register(SRCS "main.c" "dhtxx/dht.c" "hive/hive.c" 
                    INCLUDE_DIRS ".")
 
add_library(dhtxx STATIC IMPORTED)
set_property(TARGET dhtxx PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/dhtxx/dhtxx.a)
 
 
add_library(hive STATIC IMPORTED)
set_property(TARGET hive PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/hive/hive.a)