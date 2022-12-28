#include "display.hpp"

const display::OLED display::oled;

void display::init() {
  oled.init();
}
