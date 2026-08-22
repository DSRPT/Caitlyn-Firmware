idf_component_register(
    SRCS "caitlyn.c"
    INCLUDE_DIRS "include"
    REQUIRES freertos esp_timer caitlyn_config caitlyn_power tamper
)
