#include "display.hpp"
#include "snake.hpp"

Snake::Snake() {

}

bool Snake::update() {
  display::oled.home();
  display::oled.print("Snake");
}
