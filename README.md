# LVGL Display Drivers for ESP IDF

An component for creation and management of displays using the LVGL graphics library on ESP-IDF platforms. The component provides defined configurations for preconfigured boards. It handles display controller initialization, LVGL task and timer management, and provides a scoped mutex to manage access to LVGL components.

LVGL Display Drivers for ESP IDF is available as an ESP IDF component, and can be easily integrated into existing ESP-IDF projects using the component manager or cloned directly from the project repository. The component is built on top of the popular LVGL library and is an amalgamation of espressif/esp_lvgl_port and esp-idf native display drivers.

# Example Usage

```c++
#include "esp_system.h"
#include <controller.hpp>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#include <lvgl.h>

// Get an instance of the LVGL Display Controller
constexpr auto Display = LVGLDisplay::Controller::instance;

// Function to create a "Hello World" label on the display
void create_hello_world() {
  // LVGL operations are not thread safe. To avoid conflicts, we acquire the
  // mutex for the controller before accessing LVGL APIs. The mutex is released
  // once the lock goes out of scope.
  auto lock = LVGLDisplay::Lock();

  // Set the background color of the screen to a dark blue (#003a57)
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);

  // Create a label object with the text "Hello world"
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello world");

  // Set the text color to white
  lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xffffff), LV_PART_MAIN);

  // Align the label object to the center of the screen
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

// Main function
extern "C" void app_main(void) {
  // Initialise the display controller, allocate required resources, and start
  // the LVGL main task and timer.
  Display().initialise();

  // Turn the backlight on
  Display().backlight(true);

  // Create a "Hello World" label on the display
  create_hello_world();

  // Loop forever
  while (1) {
    // Delay the task for 1000ms (1 second)
    vTaskDelay(1000);
  }
}
```

# Supported Displays

| Board | Display Interface | Display Controller | Link |
|----------|----------|----------|----------|
| LilyGO T-Display-S3 | Intel 8080 | ST7789 | https://www.lilygo.cc/products/t-display-s3 |

# Configuration

The display used by LVGL is chosen and configured by Kconfig. When using the ESP-IDF component manager, use idf.py menuconfig and browse to Component config -> LVGL Display Configuration.

| Option                      | Default value | Range   | Description                                                                                                                          |
|-----------------------------|---------------|---------|--------------------------------------------------------------------------------------------------------------------------------------|
| `LVGL_DISPLAY_TDISPLAY_S3`  | `y`           |         | Select LilyGO T-Display-S3 display module                                                                                            |
| `LVGL_DISPLAY_PIXEL_CLOCK`  | `10`          | `1-20`  | Set the pixel clock frequency for the 8080 parallel bus.                                                                              |
| `LVGL_DISPLAY_DRAW_BUFF_LEN`| `20`          | `1-170` | Set the number of horizontal lines used as a draw buffer. Higher values use more memory. Usually this value should not be less than 20.|
| `LVGL_DISPLAY_MIRROR_X`     | `n`           |         | Mirror X orientation on display                                                                                                      |
| `LVGL_DISPLAY_MIRROR_Y`     | `y`           |         | Mirror Y orientation on display                                                                                                      |
| `LVGL_DISPLAY_TASK_PRIORITY`| `4`           |         | Set the FreeRTOS task priority for the LVGL main task.                                                                               |
| `LVGL_DISPLAY_TASK_STACK`   | `4096`        |         | Set the FreeRTOS task stack for the LVGL main task.                                                                                  |
| `LVGL_DISPLAY_TASK_AFFINITY`| `-1`          | `-1-1`  | Set the task affinity for the LVGL main task. Determines which core the task is tied to. -1 sets either core.                        |
| `LVGL_DISPLAY_TASK_MAX_SLEEP` | `500`       | `0-5000` | Set the maximum sleep time for the main LVGL task, in milliseconds.                                                                   |
| `LVGL_DISPLAY_TIMER_PERIOD` | `5`          | `0-5000` | Set the period for the LVGL timer in milliseconds.                                                                                   |

# Installation

## ESP IDF component manager

Add to your main `idf_component.yml` file, under dependencies.

```yaml
dependencies:
  esp-idf-lvgl-displays:
    version: ">=1.0.0"
    path: esp-idf-lvgl-displays
    git: https://github.com/etdds/esp-idf-lvgl-displays.git
```

# Direct component
Add the repository as a standard IDF component.
```
git clone https://github.com/etdds/esp-idf-lvgl-displays.git compoments/esp-idf-lvgl-displays
```

# Submodule component
Add the repository as a standard IDF component.
```
git submodule add https://github.com/etdds/esp-idf-lvgl-displays.git compoments/esp-idf-lvgl-displays
```

# Adding additional baords:

TODO

# Examples

The repository contains examples for all suported boards. 

To build an example:
```
# Clone the repository:
git clone https://github.com/etdds/esp-idf-lvgl-displays.git

# Build the `hello_world` example for TTGO T-display S3:
export IDF_TARGET=esp32s3 && export IDF_SDKCONFIG_DEFAULTS=ttgo-s3.defaults &&  idf.py build -C examples/hello_world

# Flash
idf.py -p (PORT) -C examples/hello_world

# Clean
idf.py fullclean -C examples/hello_world
```

# Credits

This component is an amalgamation of [espressif/esp_lvgl_port](https://components.espressif.com/components/espressif/esp_lvgl_port) and [LVGL](https://docs.lvgl.io/8.3/index.html)




