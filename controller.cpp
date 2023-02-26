#include <stdio.h>

#include <controller.hpp>
#include <display.hpp>

#include "boards/display_factory.hpp"
#include "esp_log.h"
#include "esp_lvgl_port.h"

static const char* TAG = "display-controller";

namespace LVGLDisplay {

  esp_err_t Controller::initialise() {
    ESP_LOGI(TAG, "Initialize LVGL library");
    if(_display.error()) {
      return _display.error();
    }

    lvgl_port_display_cfg_t disp_cfg = {.io_handle = _display.io_handle(),
                                        .panel_handle = _display.panel_handle(),
                                        .buffer_size = _display.buffer_size(),
                                        .double_buffer = _display.double_buffer(),
                                        .hres = _display.hres(),
                                        .vres = _display.vres(),
                                        .monochrome = _display.monochrome(),
                                        .rotation =
                                          {
                                            .swap_xy = _display.swap_xy(),
                                            .mirror_x = _display.mirror_x(),
                                            .mirror_y = _display.mirror_y(),
                                          },
                                        .flags = {
                                          .buff_dma = _display.dma(),
                                          .buff_spiram = _display.spi_ram(),
                                        }};

    lvgl_port_cfg_t lvgl_cfg = {
      .task_priority = CONFIG_LVGL_DISPLAY_TASK_PRIORITY,
      .task_stack = CONFIG_LVGL_DISPLAY_TASK_STACK,
      .task_affinity = CONFIG_LVGL_DISPLAY_TASK_AFFINITY,
      .task_max_sleep_ms = CONFIG_LVGL_DISPLAY_TASK_MAX_SLEEP,
      .timer_period_ms = CONFIG_LVGL_DISPLAY_TIMER_PERIOD,
    };

    esp_err_t err = lvgl_port_init(&lvgl_cfg);
    if(err != ESP_OK) {
      return err;
    }

    auto disp = lvgl_port_add_disp(&disp_cfg);
    _display.display(disp);
    return err;
  }

  esp_err_t Controller::backlight(const bool enable) {
    if(_display.error()) {
      return _display.error();
    }
    return _display.backlight(enable);
  }

  Controller& Controller::instance() {
    static Controller _instance;
    return _instance;
  }

  Controller::Controller() : _display(DisplayFactory::active_display()) {}

  Lock::Lock() { lvgl_port_lock(0); }

  Lock::~Lock() { lvgl_port_unlock(); }

};  // namespace LVGLDisplay
