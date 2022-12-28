#include "controls.hpp"
#include "display.hpp"
#include "menu.hpp"

#define FPSTR(pstr) (const __FlashStringHelper*)(pstr)

const char item_1[] PROGMEM = "Змейка";
const char item_2[] PROGMEM = "Pong";
const char item_3[] PROGMEM = "Dino";
const char item_4[] PROGMEM = "Тетрис";

const char title[] PROGMEM = "Игра";

const __FlashStringHelper* items[] = {
  FPSTR(item_1), FPSTR(item_2), FPSTR(item_3), FPSTR(item_4)
};

Menu main_menu = Menu(items, 4, FPSTR(title));

void setup() {
  Serial.begin(9600);

  // Инициализируем экран 
  display::init();
}

void loop() {
  // Обновляем ввод
  controls::update();

  main_menu.update();
}
