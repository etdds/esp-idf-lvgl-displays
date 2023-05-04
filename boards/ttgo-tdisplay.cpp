#include "ttgo-tdisplay.hpp"

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "driver/spi_master.h"

static const char* TAG = "ttgo-tdisplay";

#define LCD_H_RES 240
#define LCD_V_RES 135

#define LCD_BK_LIGHT_ON_LEVEL  1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL
#define PIN_NUM_BK_LIGHT       GPIO_NUM_4

// SPI bus configuration
#define LCD_SPI_BUS       SPI2_HOST
#define PIN_NUM_MOSI        GPIO_NUM_19
#define PIN_NUM_SCLK        GPIO_NUM_18
#define PIN_NUM_CS           GPIO_NUM_5
#define PIN_NUM_DC           GPIO_NUM_16
#define LCD_SPI_CLOCK_HZ  (CONFIG_LVGL_DISPLAY_SPI_CLOCK * 1000 * 1000)

#define LCD_CMD_BITS         8
#define LCD_PARAM_BITS       8

#define LCD_BUFF_LINE_COUNT CONFIG_LVGL_DISPLAY_DRAW_BUFF_LEN
#define LCD_TQUEUE_LENGTH   CONFIG_LVGL_DISPLAY_TQUEUE_DEPTH

#ifdef CONFIG_LVGL_DISPLAY_MIRROR_X
  #define LCD_MIRROR_X true
#else
  #define LCD_MIRROR_X false
#endif

#ifdef CONFIG_LVGL_DISPLAY_MIRROR_Y
  #define LCD_MIRROR_Y true
#else
  #define LCD_MIRROR_Y false
#endif

static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t* edata, void* user_ctx) {
  lv_disp_t** disp = (lv_disp_t**)user_ctx;
  lvgl_port_flush_ready(*disp);
  return false;
}

namespace LVGLDisplay {

  TTGOTDisplay::TTGOTDisplay() {
    ESP_LOGI(TAG, "Initialize backlight");
    gpio_config_t bk_gpio_config = {.pin_bit_mask = 1ULL << PIN_NUM_BK_LIGHT,
                                    .mode = GPIO_MODE_OUTPUT,
                                    .pull_up_en = GPIO_PULLUP_DISABLE,
                                    .pull_down_en = GPIO_PULLDOWN_DISABLE,
                                    .intr_type = GPIO_INTR_DISABLE};
    _err = gpio_config(&bk_gpio_config);
    if(_err != ESP_OK) {
      ESP_LOGE(TAG, "Failed to initialise backlight GPIO.");
      return;
    }
    gpio_set_level(PIN_NUM_BK_LIGHT, LCD_BK_LIGHT_OFF_LEVEL);

    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = PIN_NUM_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .max_transfer_sz = LCD_H_RES * LCD_BUFF_LINE_COUNT * sizeof(uint16_t),
        .flags = 0,
        .intr_flags = 0,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_SPI_BUS, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Install panel IO");

    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = PIN_NUM_CS,
        .dc_gpio_num = PIN_NUM_DC,
        .spi_mode = 0,
        .pclk_hz = LCD_SPI_CLOCK_HZ,
        .trans_queue_depth = LCD_TQUEUE_LENGTH,
        .on_color_trans_done = notify_lvgl_flush_ready,
        .user_ctx = &_display,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .flags = {
          .dc_low_on_data = 0,
          .octal_mode  = 0,
          .sio_mode =  0,
          .lsb_first = 0,
          .cs_high_active = 0,
        },
    };
    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_SPI_BUS, &io_config, &_io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1,
        .rgb_endian = LCD_RGB_ENDIAN_RGB,
        .bits_per_pixel = 16,
        .flags = {
          .reset_active_high = 1,
        },
        .vendor_config = NULL
    };
    _err = esp_lcd_new_panel_st7789(_io_handle, &panel_config, &_panel_handle);

    esp_lcd_panel_reset(_panel_handle);
    esp_lcd_panel_init(_panel_handle);
    esp_lcd_panel_invert_color(_panel_handle, true);
    esp_lcd_panel_set_gap(_panel_handle, 40, 53);
    esp_lcd_panel_swap_xy(_panel_handle, true);
    esp_lcd_panel_mirror(_panel_handle, LCD_MIRROR_X, LCD_MIRROR_Y);

    // user can flush pre-defined pattern to the screen before we turn on the screen or backlight
    _err = esp_lcd_panel_disp_on_off(_panel_handle, true);
    if(_err != ESP_OK) {
      ESP_LOGE(TAG, "Failed to turn display on.");
      return;
    }
  }

  TTGOTDisplay::~TTGOTDisplay() {}

  esp_err_t TTGOTDisplay::backlight(const bool enable) const {
    ESP_LOGI(TAG, "LCD backlight %s", enable ? "on" : "off");
    gpio_set_level(PIN_NUM_BK_LIGHT, LCD_BK_LIGHT_ON_LEVEL && enable);
    return ESP_OK;
  }

  esp_err_t TTGOTDisplay::error() const { return _err; }

  TTGOTDisplay& TTGOTDisplay::instance() {
    static TTGOTDisplay _instance;
    return _instance;
  }

  size_t TTGOTDisplay::buffer_size() const { return LCD_H_RES * LCD_BUFF_LINE_COUNT; }
  bool TTGOTDisplay::double_buffer() const { return true; }
  size_t TTGOTDisplay::hres() const { return LCD_H_RES; }
  size_t TTGOTDisplay::vres() const { return LCD_V_RES; }
  bool TTGOTDisplay::monochrome() const { return false; }
  bool TTGOTDisplay::swap_xy() const { return true; }
  bool TTGOTDisplay::mirror_x() const { return LCD_MIRROR_X; }
  bool TTGOTDisplay::mirror_y() const { return LCD_MIRROR_Y; }
  bool TTGOTDisplay::dma() const { return true; }
  bool TTGOTDisplay::spi_ram() const { return false; }
}  // namespace LVGLDisplay