#include <Arduino.h>

#include "controls.hpp"

#ifdef DEBUG_CONTROLS
/// Вспомогательная функция при отладке. Возвращает строку с названием данного состояния
const __FlashStringHelper* getStateName(uint8_t state) {
  switch (state) {
    case 0: // idle
      return F("idle");
    case 5: // click
      return F("click");
    case 6: // held
      return F("held");
    case 7: // step
      return F("step");
    case 8: // press
      return F("press");
  }
}
#endif

const EncButton<EB_TICK, UP_BTN_PIN> controls::up_button = EncButton<EB_TICK, UP_BTN_PIN>(INPUT_PULLUP);
const EncButton<EB_TICK, DOWN_BTN_PIN> controls::down_button = EncButton<EB_TICK, DOWN_BTN_PIN>(INPUT_PULLUP);
const EncButton<EB_TICK, LEFT_BTN_PIN> controls::left_button = EncButton<EB_TICK, LEFT_BTN_PIN>(INPUT_PULLUP);
const EncButton<EB_TICK, RIGHT_BTN_PIN> controls::right_button = EncButton<EB_TICK, RIGHT_BTN_PIN>(INPUT_PULLUP);
const EncButton<EB_TICK, BTN_A_PIN> controls::button_a = EncButton<EB_TICK, BTN_A_PIN>(INPUT_PULLUP);
const EncButton<EB_TICK, BTN_B_PIN> controls::button_b = EncButton<EB_TICK, BTN_B_PIN>(INPUT_PULLUP);

void controls::update() {
  up_button.tick();
  down_button.tick();
  left_button.tick();
  right_button.tick();
  button_a.tick();
  button_b.tick();

  #ifdef DEBUG_CONTROLS
  static uint8_t up_btn_last_state = 0;
  static uint8_t down_btn_last_state = 0;
  static uint8_t left_btn_last_state = 0;
  static uint8_t right_btn_last_state = 0;
  static uint8_t btn_a_last_state = 0;
  static uint8_t btn_b_last_state = 0;

  if (
    up_button.getState() != up_btn_last_state ||
    down_button.getState() != down_btn_last_state ||
    left_button.getState() != left_btn_last_state ||
    right_button.getState() != right_btn_last_state ||
    button_a.getState() != btn_a_last_state ||
    button_b.getState() != btn_b_last_state) {
    
    up_btn_last_state = up_button.getState();
    down_btn_last_state = down_button.getState();
    left_btn_last_state = left_button.getState();
    right_btn_last_state = right_button.getState();
    btn_a_last_state = button_a.getState();
    btn_b_last_state = button_b.getState();

    Serial.println(F("Controls debug:"));

    Serial.print(F("up_button:\t"));
    Serial.println(getStateName(up_btn_last_state));
    
    Serial.print(F("down_button:\t"));
    Serial.println(getStateName(down_btn_last_state));

    Serial.print(F("left_button:\t"));
    Serial.println(getStateName(left_btn_last_state));
    
    Serial.print(F("right_button:\t"));
    Serial.println(getStateName(right_btn_last_state));
    
    Serial.print(F("button_a:\t"));
    Serial.println(getStateName(btn_a_last_state));

    Serial.print(F("button_b:\t"));
    Serial.println(getStateName(btn_b_last_state));

    Serial.println();
  }
  #endif
}
