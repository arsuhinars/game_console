#include "games.hpp"

// Массив, содержащий все игры
const GameBuilder games[] = {
  // addGame<КЛАСС ИГРЫ>(),
};

// Массив с названиями игр
const char* const games_names[] = {
};

const uint8_t games_count = sizeof(games) / sizeof(GameBuilder);
