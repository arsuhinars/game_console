#include "display.hpp"
#include "dino.hpp"

Dino::Dino() {

}

void Dino::update() {
  display::oled.home();
  display::oled.print("Dino");
}
