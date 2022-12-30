#include "display.hpp"
#include "dino.hpp"

Dino::Dino() {

}

bool Dino::update() {
  display::oled.home();
  display::oled.print("Dino");
}
