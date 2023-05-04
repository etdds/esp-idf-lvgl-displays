# LVGL Display Drivers for ESP IDF

An component for creation and management of displays using the LVGL graphics library on ESP-IDF platforms. The component provides defined configurations for preconfigured boards. It handles display controller initialization, LVGL task and timer management, and provides a scoped mutex to manage access to LVGL components.

LVGL Display Drivers for ESP IDF is available as an ESP IDF component, and can be easily integrated into existing ESP-IDF projects using the component manager or cloned directly from the project repository. The component is built on top of the popular LVGL library and is an amalgamation of espressif/esp_lvgl_port and esp-idf native display drivers.

# Example Usage

```c++
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lvgl.h>
#include <stdio.h>

#include <controller.hpp>

#include "esp_system.h"

// Get an instance of the LVGL Display Controller
constexpr auto Display = LVGLDisplay::Controller::instance;

// The screen label to change.
lv_obj_t* label;

// Function to create a "Hello World" label on the display
void create_hello_world() {
  // LVGL operations are not thread safe. To avoid conflicts, we acquire the
  // mutex for the controller before accessing LVGL APIs. The mutex is released
  // once the lock goes out of scope.
  auto lock = LVGLDisplay::Lock();

  // Set the background color of the screen to a black.
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);

  // Add a red border around the edge of the screen to help set offsets.
  lv_obj_set_style_border_color(lv_scr_act(), lv_color_hex(0xFF00000), LV_PART_MAIN);
  lv_obj_set_style_border_width(lv_scr_act(), 1, LV_PART_MAIN);
  


  // Create a label object with the text "Hello world"
  label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "");

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

  size_t seconds = 0;
  char text[32] = "";

  // Loop forever
  while(1) {
    snprintf(text, 32, "Hello world %u", seconds++);

    // Update the display with the current elapsed seconds.
    // Use a static lock, which does not rely on scope.
    LVGLDisplay::Lock::acquire();
    lv_label_set_text(label, text);
    LVGLDisplay::Lock::release();

    // Delay the task for 1000ms (1 second)
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
```

# Supported Displays

| Board | Display Interface | Display Controller | Link |
|----------|----------|----------|----------|
| LilyGO T-Display-S3 | Intel 8080 | ST7789 | https://www.lilygo.cc/products/t-display-s3 |
| LilyGO TTGO-TDisplay| SPI | ST7789 |https://www.lilygo.cc/products/lilygo%C2%AE-ttgo-t-display-1-14-inch-lcd-esp32-control-board  |

# Configuration

The display used by LVGL is chosen and configured by Kconfig. When using the ESP-IDF component manager, use idf.py menuconfig and browse to Component config -> LVGL Display Configuration.

| Option                      | Default value | Range   | Description                                                                                                                          |
|-----------------------------|---------------|---------|--------------------------------------------------------------------------------------------------------------------------------------|
| `LVGL_DISPLAY_TDISPLAY_S3`  | `y`           |         | Select LilyGO T-Display-S3 display module                                                                                            |
| `LVGL_DISPLAY_TTGO_TDISPLAY`  | `n`           |         | Select LilyGO TTGO-tdisplay display module                                                                                            |
| `LVGL_DISPLAY_PIXEL_CLOCK`  | `10`          | `1-80`  | Set the pixel clock frequency for the 8080 parallel bus.                                                                              |
| `LVGL_DISPLAY_TQUEUE_DEPTH`  | `10`          | `1-100`  | Set the length of the LCD transaction queue.                                                                              |
| `LVGL_DISPLAY_DRAW_BUFF_LEN`| `20`          | `1-170` | Set the number of horizontal lines used as a draw buffer. Higher values use more memory. Usually this value should not be less than 20.|
| `LVGL_DISPLAY_SPI_CLOCK`  | `20`          | `1-40`  | Set the SPI clock frequency for SPI based controllers bus.                                                                              |
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
    git: https://github.com/etdds/esp-idf-lvgl-displays.git
```

# Direct component
Add the repository as a standard IDF component.
```
git clone https://github.com/etdds/esp-idf-lvgl-displays.git components/esp-idf-lvgl-displays
```

# Submodule component
Add the repository as a standard IDF component.
```
git submodule add https://github.com/etdds/esp-idf-lvgl-displays.git components/esp-idf-lvgl-displays
```

# Adding additional baords:

1. Add the display driver which implements `include/display.hpp : Display`
2. Add a static instance to `display_factory.cpp`
3. Add a guarded entry to `CMakelists.txt`
4. Add Kconfig options
5. Add display information and any additional configuration to `README.md`
6. Add default configuration to each example.
7. Add build configuration to `.vscode/tasks.json`
8. Add github action target.


# Examples

The repository contains examples for all suported boards. 

To build an example:
```
# Clone the repository:
git clone https://github.com/etdds/esp-idf-lvgl-displays.git

# Configure for tdiplay-s3, using the 'hello world' example
cd examples/hello_world && rm -f sdkconfig* && cp tdisplay-s3.defaults sdkconfig.defaults && idf.py set-target esp32s3
# OR

# Configure for ttgo-tdiplay, using the 'hello world' example
cd examples/hello_world && rm -f sdkconfig* && cp ttgo-tdisplay.defaults sdkconfig.defaults && idf.py set-target esp32

# Build the example (from the examples/hello_world directory)
idf.py build

# Flash
idf.py -p (PORT) 

# Clean
idf.py fullclean
```

# Credits

This component is an amalgamation of [espressif/esp_lvgl_port](https://components.espressif.com/components/espressif/esp_lvgl_port) and [LVGL](https://docs.lvgl.io/8.3/index.html)
