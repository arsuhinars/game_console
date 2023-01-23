#include "game.hpp"
#include "games.hpp"

// Подключаем классы игр
#include "snake.hpp"
#include "pong.hpp"
#include "dino.hpp"
#include "tetris.hpp"

#include "texts.hpp"

// Массив, содержащий все игры
const GameCreator games[] = {
  // getGameCreator<КЛАСС ИГРЫ>(),
  getGameCreator<Snake>(),
  getGameCreator<Pong>(),
  getGameCreator<Dino>(),
  getGameCreator<Tetris>()
};

// Массив с названиями игр
const char* const games_names[] PROGMEM = {
  texts::SNAKE_NAME,
  texts::PONG_NAME,
  texts::DINO_NAME,
  texts::TETRIS_NAME
};

const uint8_t games_count = sizeof(games) / sizeof(GameCreator);
