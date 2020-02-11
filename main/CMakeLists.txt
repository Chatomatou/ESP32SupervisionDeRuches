idf_component_register(SRCS "main.c" "dhtxx/dht.c" 
                    INCLUDE_DIRS ".")
                    
add_library(dhtxx STATIC IMPORTED)
set_property(TARGET dhtxx PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/dhtxx/dhtxx.a)

 