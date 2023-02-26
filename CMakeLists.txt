set(COMPONET_SRC 
    "controller.cpp"
    "boards/display_factory.cpp"
)

if(CONFIG_LVGL_DISPLAY_TDISPLAY_S3)
    list(APPEND COMPONET_SRC "boards/t-display-s3.cpp")
endif()

idf_component_register(
    SRCS 
        ${COMPONET_SRC}
    REQUIRES 
        "driver" 
        "esp_timer"
    INCLUDE_DIRS 
        "include"
)
