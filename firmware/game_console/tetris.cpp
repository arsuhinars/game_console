#include "display.hpp"
#include "tetris.hpp"

// Координаты поля на экране
#define TETRIS_FIELD_X  ((DISPLAY_WIDTH - TETRIS_FIELD_WIDTH * TETRIS_BLOCK_SIZE) / 2)
#define TETRIS_FIELD_Y  ((DISPLAY_HEIGHT - TETRIS_FIELD_HEIGHT * TETRIS_BLOCK_SIZE) / 2)

// Спрайт блока
const uint8_t TERTIS_BLOCK[] PROGMEM = {
	0x00, 0x7E, 0x42, 0x5A, 0x5A, 0x42, 0x7E, 0x00, 
};

// Массив со всеми фигурами
const struct {
  int8_t width;       // Ширина фигуры
  int8_t height;      // Высота фигуры
  int8_t rot_count;   // Кол-во положений при вращении

  // Массив с описанием фигуры
  // 1 индекс - индекс вращения
  // 2 индекс - координата по горизонтали
  // 3 - по вертикали
  bool data[][][];
} TETRIS_FIGURES[] PROGMEM = {
  // Фигура L
  {
    4, 4, 2, {
      {
        { 0, 1, 0, 0 },
        { 0, 1, 0, 0 },
        {  }
      }
    }
  },
};

const char* const TETRIS_MENU_ITEMS[] PROGMEM = {
  texts::START, texts::QUIT
};

Tetris::Tetris() : _menu(TETRIS_MENU_ITEMS, 2, FPSTR(texts::TETRIS_NAME)) {

}

void Tetris::update() {
  // Рисуем главное меню, если игра не запущена
  if (!_is_game_started) {
    _menu.update();

    if (_menu.clickedItem() != -1) {
      // Нажата кнопка выхода из игры
      if (_menu.clickedItem() == 1) {
        return false;
      }

      // Нажата кнопка запуска игры
      startGame();
      _menu.handleClickedItem();
    }

    return true;
  }



  return true;
}

void Tetris::startGame() {
  // Очищаем дисплей и рисуем границы поля
  display::oled.clear();
  display::oled.rect(
    TETRIS_FIELD_X - 1,
    TETRIS_FIELD_Y - 1,
    TERTIS_FIELD_X + TETRIS_FIELD_WIDTH * TETRIS_BLOCK_SIZE,
    TERTIS_FIELD_Y + TETRIS_FIELD_HEIGHT * TETRIS_BLOCK_SIZE,
    OLED_STROKE
  );
  display::oled.update();

  // Очищаем поле
  for (int8_t x = 0; x < TETRIS_FIELD_WIDTH; ++x) {
    for (int8_t y = 0; y < TETRIS_FIELD_HEIGHT; ++y) {
      _field[x][y] = 0;
    }
  }

  // Сбрасываем сумку
  _figures_in_bag = TETRIS_FIGURES_COUNT;
  for (uint8_t i = 0; i < TETRIS_FIGURES_COUNT; ++i) {
    _figures_bag[i] = true;
  }

  // Сбрасываем параметры игры
  _is_game_started = true;
  _score = 0;
  _last_time = millis();

  _curr_figure = getNextFigure();
  _next_figure = getNextFigure();
  _curr_figure_rot = 0;
  
}
