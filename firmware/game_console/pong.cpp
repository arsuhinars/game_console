#include "display.hpp"
#include "pong.hpp"

Pong::Pong() {

}

void Pong::update() {
  display::oled.home();
  display::oled.print("Pong");
}
