#pragma once

#include "menu.hpp"
#include "game.hpp"
#include "utils.hpp"

// Размер одного блока в пикселях
#define TETRIS_BLOCK_SIZE     8

// Ширина и высота поля в блоках
#define TETRIS_FIELD_WIDTH    6
#define TETRIS_FIELD_HEIGHT   8

// Позиция на экране окна с следующей фигурой
#define TETRIS_PREVIEW_X      92
#define TETRIS_PREVIEW_Y      8
// Размер этого окна в блоках
#define TERTIS_PREVIEW_WIDTH  4
#define TETRIS_PREVIEW_HEIGHT 4

// Начальная позиция фигуры при её появлении
#define TETRIS_INITIAL_FIGURE_X 3
#define TETRIS_INITIAL_FIGURE_Y 1

// Индексы каждой из фигур
#define TETRIS_FIGURE_L       0
#define TETRIS_FIGURE_J       1
#define TETRIS_FIGURE_Z       2
#define TETRIS_FIGURE_S       3
#define TETRIS_FIGURE_T       4
#define TETRIS_FIGURE_I       5
#define TETRIS_FIGURE_O       6
#define TETRIS_FIGURES_COUNT  7

class Tetris : public Game {
public:
  explicit Tetris();

  bool update() override;

private:
  // Главное меню
  Menu _menu;

  // Двухмерный массив игрового поля
  // true - блок занят, false - не занят
  bool _field[TETRIS_FIELD_WIDTH][TETRIS_FIELD_HEIGHT];

  bool _is_game_started = false;  // Запущена ли игра
  uint16_t _score = 0;            // Игровой счет
  unsigned long _last_time = 0;   // Игровой таймер

  uint8_t _curr_figure;       // Индекс текущей фигуры
  uint8_t _next_figure;       // Индекс следующей фигуры
  uint8_t _curr_figure_rot;   // Поворот фигуры
  cvec2 _figure_pos;          // Её позиция на поле

  // "Сумка" с фигурами.
  bool _figures_bag[TETRIS_FIGURES_COUNT];
  uint8_t _figures_in_bag;    // Кол-во оставшихся фигур в сумке

  /// Метод запуска игры
  void startGame();

  /// Метод отрисовки окна со следующей фигурой
  void drawFigurePreview();

  /// Получить индекс следующей случайной фигуры
  /// При этом фигура вынимается из "сумки"
  uint8_t getNextFigure();
 
  /// Метод отрисовки блока на поле
  void drawBlock(int8_t x, int8_t y);

  /// Метод очистки блока на поле
  void clearBlock(int8_t x, int8_t y);
};
