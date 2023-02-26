#pragma once

#include "display.hpp"
#include "esp_err.h"

namespace LVGLDisplay {
  /**
   * @brief A singleton class that controls the display through LVGL.
   */
  class Controller {
   public:
    /**
     * @brief Allocate resources and start the LVGL main task and timer.
     *
     * @return An esp_err_t indicating the status of the operation.
     */
    esp_err_t initialise();

    /**
     * @brief Enables or disables the display backlight.
     *
     * @param enable Whether to enable or disable the backlight.
     * @return An esp_err_t indicating the status of the operation.
     */
    esp_err_t backlight(const bool enable);

    /**
     * @brief Returns the singleton instance of the display controller.
     *
     * @return The singleton instance of the display controller.
     */
    static Controller& instance();

    /* Delete move and copy assignment operators and constuctors */
    Controller(const Controller&) = delete;
    Controller(Controller&&) = delete;
    Controller& operator=(const Controller&) = delete;
    Controller&& operator=(Controller&&) = delete;

   private:
    Controller();
    Display& _display; /**< The display being controlled. */
  };

  /**
   * @brief A class for controlling access to the display.
   * Aquires the lock using scope. Ie. the lock is acquired on consturction,
   * and released on destruction.
   */
  class Lock {
   public:
    /**
     * @brief Acquires the lock for the display.
     * This should be used before any LVGL operation
     */
    Lock();

    /**
     * @brief Releases the lock for the display.
     */
    ~Lock();
  };
};  // namespace LVGLDisplay
