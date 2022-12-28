#include "games.hpp"

// Подключаем классы игр
#include "snake.hpp"
#include "pong.hpp"
#include "dino.hpp"

// Массив, содержащий все игры
const GameBuilder games[] = {
  // addGame<КЛАСС ИГРЫ>(),
  addGame<Snake>(),
  addGame<Pong>(),
  addGame<Dino>(),
};

// Массив с названиями игр
const char* const games_names[] = {
  SNAKE_NAME, PONG_NAME, DINO_NAME
};

const uint8_t games_count = sizeof(games) / sizeof(GameBuilder);
