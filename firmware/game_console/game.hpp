#pragma once

// Базовый класс для всех игр
class Game {
public:
  /**
  * Метод обновления игры. Вызывается в основном цикле
  * @returns `true` если игра еще запущена, иначе - `false`.
  */
  virtual bool update() = 0;
};

// Тип функции, создающей объект класса игры
// и возвращающей его в виде указателя.
typedef Game* (*GameCreator)();

template <class T>
constexpr GameCreator getGameCreator() {
  return [] {
    return static_cast<Game*>(new T());
  };
}
