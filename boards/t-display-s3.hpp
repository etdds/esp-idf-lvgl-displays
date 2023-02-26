#pragma once

#include <display.hpp>

namespace LVGLDisplay {
  class TDisplayS3 : public Display {
   public:
    TDisplayS3();
    ~TDisplayS3();
    static TDisplayS3& instance();
    size_t buffer_size() const override;
    bool double_buffer() const override;
    size_t hres() const override;
    size_t vres() const override;
    bool monochrome() const override;
    bool swap_xy() const override;
    bool mirror_x() const override;
    bool mirror_y() const override;
    bool dma() const override;
    bool spi_ram() const override;
    esp_err_t backlight(const bool enable) const override;
    esp_err_t error() const override;

   private:
    esp_err_t _err;
  };
};  // namespace LVGLDisplay