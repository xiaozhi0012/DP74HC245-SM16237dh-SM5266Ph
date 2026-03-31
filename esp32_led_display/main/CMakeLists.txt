idf_component_register(SRCS "main.cpp" "LED_Matrix.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_gpio esp_heap)
