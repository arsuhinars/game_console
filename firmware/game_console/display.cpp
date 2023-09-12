#include "display.hpp"

const display::OLED display::oled;

void display::init() {
  oled.init();
  oled.clear();
  oled.setContrast(DISPLAY_BRIGHTNESS);
}
