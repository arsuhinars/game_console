#include "display.hpp"
#include "snake.hpp"

Snake::Snake() {

}

void Snake::update() {
  display::oled.home();
  display::oled.print("Snake");
}
