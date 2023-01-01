#pragma once

#include <avr/pgmspace.h>
#include <Arduino.h>

// Преобразовать PROGMEM строку к F() строке
#define FPSTR(pstr) (const __FlashStringHelper*)(pstr)

namespace utils {
  // Стуктура для хранения двухмерного вектора
  template <typename T>
  struct vec2 {
    T x;
    T y;
  };
  // Определяем все типы векторов
  typedef vec2<int16_t> ivec2;    // int
  typedef vec2<int8_t> cvec2;     // char
  typedef vec2<uint16_t> uvec2;   // unsigned int
  typedef vec2<uint8_t> ucvec2;   // unsigned char

  /// Функция вычисления длины строки, содержащей русские символы в кодировке UTF-8
  size_t rus_strlen(const char* s);

  /// Аналогично функции `rus_strlen` вычисляет длину строки,
  /// однако принимает PROGMEM строку
  size_t rus_strlen_P(PGM_P s);

  /**
  * Функция плавного движения с учетом ускорения и сопротивления движению
  * @param value текущее значение
  * @param vel ссылка на текущую скорость изменения значения
  * @param accel величина ускорения с учетом знака
  * @param drag коэффициент сопротивления движению
  * @param time_step длительность "шага" между вызовами функции в миллисекундах
  * @returns новое значение с учетом входных данных
  */
  inline int16_t smoothMove(
    int16_t value, 
    int16_t& vel,
    const int16_t accel,
    const int16_t drag,
    const int16_t time_step
  ) {
    int16_t drag_accel = -drag * vel;
    vel += (accel + drag_accel) * time_step / 1000;
    return value + vel * time_step / 1000;
  }
} // namespace utils
