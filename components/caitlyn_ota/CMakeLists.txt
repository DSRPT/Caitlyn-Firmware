idf_component_register(
    SRCS "caitlyn_ota.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_http_client esp_https_ota mbedtls caitlyn_config caitlyn_fs
)
