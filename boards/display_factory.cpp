#include "display_factory.hpp"

#include "t-display-s3.hpp"

namespace LVGLDisplay {
  Display& DisplayFactory::active_display() {
#ifdef CONFIG_LVGL_DISPLAY_TDISPLAY_S3
    static TDisplayS3 display;
#else
  #error No active display defined.
#endif
    return display;
  }
};  // namespace LVGLDisplay