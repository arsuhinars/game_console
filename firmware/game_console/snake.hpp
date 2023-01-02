#pragma once

#include "display.hpp"
#include "menu.hpp"
#include "game.hpp"
#include "utils.hpp"

// Размер одного тайла
#define SNAKE_TILE_SIZE     8

// Размеры игрового поля (в тайлах)
#define SNAKE_FIELD_WIDTH   (DISPLAY_WIDTH / SNAKE_TILE_SIZE)
#define SNAKE_FIELD_HEIGHT  (DISPLAY_HEIGHT / SNAKE_TILE_SIZE)

// Направления движения змейки
#define SNAKE_DIR_UP          0
#define SNAKE_DIR_RIGHT       1
#define SNAKE_DIR_DOWN        2
#define SNAKE_DIR_LEFT        3

// Начальная длина змейки
#define INITIAL_SNAKE_LENGTH  4
// Начальная позиция головы змейки
#define INITIAL_SNAKE_X       5
#define INITIAL_SNAKE_Y       4
// Изначальное направление змейки
#define INITIAL_SNAKE_DIR     SNAKE_DIR_RIGHT

// Длительность шага обновления игры в миллисекундах
#define SNAKE_UPDATE_STEP     400

// Шанс спавна дополнительной еды. Вероятность: 1/N
#define SNAKE_EXTRA_FOOD_RATE   3
// Количество шагов до исчезновения дополнительной еды
#define SNAKE_EXTRA_FOOD_STEPS  12

class Snake : public Game {
public:
  explicit Snake();

  bool update() override;

private:
  Menu _menu;

  // Запущена ли игра
  bool _is_game_started = false;
  // Время последнего обновления игры
  unsigned long _last_update = 0;

  // Двухмерный массив поля, где -1 - тайл свободен.
  // Если число >= 0, то тайл занята телом змейки, а число указывает на направление
  int8_t field[SNAKE_FIELD_WIDTH][SNAKE_FIELD_HEIGHT];

  // Позиция головы змейки
  utils::cvec2 snake_head_pos;
  // Позиция хвоста змейки
  utils::cvec2 snake_tail_pos;
  // Текущее направление движения змейки
  uint8_t snake_dir = 0;
  // Предыдущее направление движения змейки
  uint8_t last_snake_dir = 0;
  // Счетчик роста змейки
  uint8_t snake_growth = 0;

  // Координаты еды
  utils::cvec2 food_pos;

  // Положение дополнительной еды
  utils::cvec2 extra_food_pos;
  // Состояние доп. еды, где 0 - еды нет на поле
  uint8_t extra_food_state = 0;

  // Функция для получения вектора направления
  static const utils::cvec2& getDirVector(uint8_t dir);

  // Метод запуска игры
  void startGame();

  // Метод спавна еды
  void spawnFood();

  // Метод спавна дополнительной еды
  void spawnExtraFood();
};
