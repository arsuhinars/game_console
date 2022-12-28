#pragma once

#include <EncButton.h>

// Раскомментируйте для отладки ввода
// #define DEBUG_CONTROLS

// Номера пинов для кнопок
#define UP_BTN_PIN    2
#define DOWN_BTN_PIN  3
#define LEFT_BTN_PIN  4
#define RIGHT_BTN_PIN 5
#define BTN_A_PIN     6
#define BTN_B_PIN     7

namespace controls {
  // Объекты всех кнопок
  extern const EncButton<EB_TICK, UP_BTN_PIN> up_button;
  extern const EncButton<EB_TICK, DOWN_BTN_PIN> down_button;
  extern const EncButton<EB_TICK, LEFT_BTN_PIN> left_button;
  extern const EncButton<EB_TICK, RIGHT_BTN_PIN> right_button;
  extern const EncButton<EB_TICK, BTN_A_PIN> button_a;
  extern const EncButton<EB_TICK, BTN_B_PIN> button_b;

  /** 
  * Функция обновления ввода. Необходимо вызывать из функции loop().
  * Данная функция внутри себя вызывает "тикер" каждой кнопки и
  * выводит отладочные сообщения.
  */
  void update();
} // namespace controls
