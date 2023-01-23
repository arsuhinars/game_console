#pragma once

#include "menu.hpp"
#include "game.hpp"
#include "utils.hpp"

// Время шага обновления игры
#define TETRIS_UPDATE_STEP      300
// Время быстрого шага
#define TETRIS_FAST_UPDATE_STEP 100
// Время полного исчезновения фигур
#define TETRIS_DISAPPEAR_TIME   500

// Размер одного блока в пикселях
#define TETRIS_BLOCK_SIZE     3

// Ширина и высота поля в блоках
#define TETRIS_FIELD_WIDTH    10
#define TETRIS_FIELD_HEIGHT   20

// Позиция на экране окна с следующей фигурой
#define TETRIS_PREVIEW_X        100
#define TETRIS_PREVIEW_Y        8
// Размер этого окна в блоках
#define TETRIS_PREVIEW_WIDTH    5
#define TETRIS_PREVIEW_HEIGHT   6
// Расположение фигуры в этом окне
#define TETRIS_PREVIEW_FIGURE_X 2
#define TETRIS_PREVIEW_FIGURE_Y 2

// Начальная позиция фигуры при её появлении
#define TETRIS_INITIAL_FIGURE_X 5
#define TETRIS_INITIAL_FIGURE_Y 0

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
  uint8_t _figure_rot;        // Поворот фигуры
  utils::cvec2 _figure_pos;   // Её позиция на поле
  bool _was_dropped;          // Была ли сброшена фигура

  // Границы предыдущей фигуры
  utils::cvec2 _bound_min;
  utils::cvec2 _bound_max;

  // "Сумка" с фигурами.
  bool _figures_bag[TETRIS_FIGURES_COUNT];
  uint8_t _figures_in_bag;    // Кол-во оставшихся фигур в сумке

  // Полностью заполенные строки
  bool _filled_rows[TETRIS_FIELD_HEIGHT];
  // Текущий исчезающий столбец
  uint8_t _disappearing_col;

  /// Метод запуска игры
  void startGame();

  /// Метод отрисовки окна со следующей фигурой
  void drawFigurePreview();

  /// Получить индекс следующей случайной фигуры
  /// При этом фигура вынимается из "сумки"
  uint8_t getNextFigure();

  /// Метод трансформации точки с учетом вращения
  static utils::cvec2 transformPoint(const utils::cvec2& pos, uint8_t rot, const utils::cvec2& v);
};
