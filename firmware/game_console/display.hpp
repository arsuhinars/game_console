#pragma once

#include <GyverOLED.h>

// Разрешение дисплея
#define DISPLAY_WIDTH     128
#define DISPLAY_HEIGHT    64

// Размеры шрифта на экране
#define DISPLAY_FONT_WIDTH  6
#define DISPLAY_FONT_HEIGHT 8

namespace display {
  typedef GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> OLED;

  // Объект OLED дисплея
  extern const OLED oled;

  /// Метод инициализации дисплея
  void init();

} // namespace display
