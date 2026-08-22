idf_component_register(
    SRCS "caitlyn_power.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_timer freertos nvs_flash caitlyn_config
)
