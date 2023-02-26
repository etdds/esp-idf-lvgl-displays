/**
 * @file display_factory.hpp
 * @brief Defines the LVGLDisplay::DisplayFactory class.
 */

#pragma once

#include <display.hpp>

namespace LVGLDisplay {

  class DisplayFactory {
   public:
    /**
     * @brief Returns the active display.
     * The active display is selected using Kconfig, change it using menuconfig.
     * The display is automatically added to the controller during initialisation.
     * @return A reference to the active display.
     */
    static Display& active_display();
  };

}  // namespace LVGLDisplay