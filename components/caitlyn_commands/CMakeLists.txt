idf_component_register(
    SRCS "caitlyn_commands.c"
    INCLUDE_DIRS "include"
    REQUIRES caitlyn caitlyn_fs tamper caitlyn_power
)
