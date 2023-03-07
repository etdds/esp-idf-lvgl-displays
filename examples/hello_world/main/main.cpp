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