#pragma once

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "lvgl.h"

namespace LVGLDisplay {
  /**
   * @brief An abstract base class representing a display.
   */
  class Display {
   public:
    /**
     * @brief Default constructor.
     */
    Display() = default;

    /**
     * @brief Default destructor.
     */
    virtual ~Display() = default;

    /**
     * @brief Returns the size of the display buffer in bytes.
     *
     * @return The size of the display buffer in bytes.
     */
    virtual size_t buffer_size() const = 0;

    /**
     * @brief Returns whether the display uses double buffering.
     *
     * @return Whether the display uses double buffering.
     */
    virtual bool double_buffer() const = 0;

    /**
     * @brief Returns the horizontal resolution of the display.
     *
     * @return The horizontal resolution of the display.
     */
    virtual size_t hres() const = 0;

    /**
     * @brief Returns the vertical resolution of the display.
     *
     * @return The vertical resolution of the display.
     */
    virtual size_t vres() const = 0;

    /**
     * @brief Returns whether the display is monochrome.
     *
     * @return Whether the display is monochrome.
     */
    virtual bool monochrome() const = 0;

    /**
     * @brief Returns whether the display's X and Y axes are swapped.
     *
     * @return Whether the display's X and Y axes are swapped.
     */
    virtual bool swap_xy() const = 0;

    /**
     * @brief Returns whether the display's X axis is mirrored.
     *
     * @return Whether the display's X axis is mirrored.
     */
    virtual bool mirror_x() const = 0;

    /**
     * @brief Returns whether the display's Y axis is mirrored.
     *
     * @return Whether the display's Y axis is mirrored.
     */
    virtual bool mirror_y() const = 0;

    /**
     * @brief Returns whether the display uses DMA.
     *
     * @return Whether the display uses DMA.
     */
    virtual bool dma() const = 0;

    /**
     * @brief Returns whether the display uses SPI RAM.
     *
     * @return Whether the display uses SPI RAM.
     */
    virtual bool spi_ram() const = 0;

    /**
     * @brief Enables or disables the display backlight.
     *
     * @param enable Whether to enable or disable the backlight.
     *
     * @return ESP_OK on success, or an error code on failure.
     */
    virtual esp_err_t backlight(const bool enable) const = 0;

    /**
     * @brief Returns the last error code for the display.
     *
     * @return The last error code for the display.
     */
    virtual esp_err_t error() const = 0;

    /**
     * @brief Returns the panel handle for the display.
     *
     * @return The panel handle for the display.
     */
    esp_lcd_panel_handle_t panel_handle() const { return _panel_handle; }

    /**
     * @brief Returns the I/O handle for the display.
     *
     * @return The I/O handle for the display.
     */
    esp_lcd_panel_io_handle_t io_handle() const { return _io_handle; }
    /**
     * @brief Assign the display handle to the display.
     * Used for internal flush callback from LVGL controller.
     *
     * @param display The LVGL display handle.
     */
    void display(lv_disp_t* display) { _display = display; };

   protected:
    esp_lcd_panel_handle_t _panel_handle = NULL;
    esp_lcd_panel_io_handle_t _io_handle = NULL;
    lv_disp_t* _display = NULL;
  };

};  // namespace LVGLDisplay