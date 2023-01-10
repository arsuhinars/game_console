#include <Arduino.h>

#include "display.hpp"
#include "controls.hpp"
#include "texts.hpp"
#include "tetris.hpp"

// Координаты поля на экране
#define TETRIS_FIELD_X  ((DISPLAY_WIDTH - TETRIS_FIELD_WIDTH * TETRIS_BLOCK_SIZE) / 2)
#define TETRIS_FIELD_Y  ((DISPLAY_HEIGHT - TETRIS_FIELD_HEIGHT * TETRIS_BLOCK_SIZE) / 2)

// Максимальный размер фигуры
#define TETRIS_FIGURE_WIDTH       4
#define TETRIS_FIGURE_HEIGHT      4
// Опорная координата
#define TETRIS_FIGURE_ORIGIN_X    1
#define TETRIS_FIGURE_ORIGIN_Y    1
// Макс. кол-во вращений фигуры
#define TETRIS_FIGURE_ROTATIONS   4

using namespace utils;

// Спрайт блока
const uint8_t TETRIS_BLOCK[] PROGMEM = {
	0x00, 0x7E, 0x42, 0x5A, 0x5A, 0x42, 0x7E, 0x00, 
};

// Массив с описанием фигуры
// 1 индекс - индекс вращения
// 2 индекс - координата по горизонтали
// 3 - по вертикали
typedef bool TetrisFigure[TETRIS_FIGURE_ROTATIONS][TETRIS_FIGURE_WIDTH][TETRIS_FIGURE_HEIGHT];

// Массив со всеми фигурами
const TetrisFigure TETRIS_FIGURES[TETRIS_FIGURES_COUNT] PROGMEM = {
  // Фигура L
  {
    {
      { 0, 0, 0, 0 },
      { 1, 1, 1, 0 },
      { 0, 0, 1, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 1, 1, 0 },
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 1, 0, 0, 0 },
      { 1, 1, 1, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 1, 1, 0, 0 },
      { 0, 0, 0, 0 }
    }
  },
  // Фигура J
  {
    {
      { 0, 0, 1, 0 },
      { 1, 1, 1, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 1, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 1, 1, 1, 0 },
      { 1, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 1, 1, 0 },
      { 0, 0, 0, 0 }
    }
  },
  // Фигура Z
  {
    {
      { 1, 0, 0, 0 },
      { 1, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 0, 1, 1, 0 },
      { 1, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 1, 0, 0, 0 },
      { 1, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 0, 1, 1, 0 },
      { 1, 1, 0, 0 },
      { 0, 0, 0, 0 }
    }
  },
  // Фигура S
  {
    {
      { 0, 1, 0, 0 },
      { 1, 1, 0, 0 },
      { 1, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 1, 1, 0, 0 },
      { 0, 1, 1, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 1, 0, 0 },
      { 1, 1, 0, 0 },
      { 1, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 1, 1, 0, 0 },
      { 0, 1, 1, 0 },
      { 0, 0, 0, 0 }
    }
  },
  // Фигура T
  {
    {
      { 1, 0, 0, 0 },
      { 1, 1, 0, 0 },
      { 1, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 0, 1, 0, 0 },
      { 1, 1, 1, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 1, 0 },
      { 0, 1, 1, 0 },
      { 0, 0, 1, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 1, 1, 1, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 }
    }
  },
  // Фигура I
  {
    {
      { 0, 0, 0, 0 },
      { 1, 1, 1, 1 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 1, 1, 1, 1 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 }
    }
  },
  // Фигура O
  {
    {
      { 0, 0, 0, 0 },
      { 0, 1, 1, 0 },
      { 0, 1, 1, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 0, 1, 1, 0 },
      { 0, 1, 1, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 0, 1, 1, 0 },
      { 0, 1, 1, 0 },
      { 0, 0, 0, 0 }
    },
    {
      { 0, 0, 0, 0 },
      { 0, 1, 1, 0 },
      { 0, 1, 1, 0 },
      { 0, 0, 0, 0 }
    }
  }
};

const char* const TETRIS_MENU_ITEMS[] PROGMEM = {
  texts::START, texts::QUIT
};

Tetris::Tetris() : _menu(TETRIS_MENU_ITEMS, 2, FPSTR(texts::TETRIS_NAME)) {

}

bool Tetris::update() {
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
  
  // Обновляем игру только тогда, когда это необходимо
  if (millis() - _last_time <= controls::down_button.state() ? TETRIS_FAST_UPDATE_STEP : TETRIS_UPDATE_STEP) {
    return true;
  }
  _last_time = millis();

  // Двумерный массив с данными текущей фигуры
  bool figure_data[TETRIS_FIGURE_WIDTH][TETRIS_FIGURE_HEIGHT];
  // Получаем данные текущей фигуры
  memcpy_P(figure_data, TETRIS_FIGURES[_curr_figure][_curr_figure_rot], sizeof(figure_data));

  // Остановлена ли фигура другим блоком
  bool is_blocked = false;

  // Перебираем клетки фигуры
  cvec2 pos = { 
    _figure_pos.x - TETRIS_INITIAL_FIGURE_X,
    _figure_pos.y - TETRIS_INITIAL_FIGURE_Y
  };
  for (int8_t i = 0; i < TETRIS_FIGURE_WIDTH; ++i) {
    for (int8_t j = 0; j < TETRIS_FIGURE_HEIGHT; ++j) {
      if (!figure_data[i][j]) {
        continue;
      }

      // Если блок уже занят
      if (_field[pos.x + i][pos.y + j]) {
        // Игрок проиграл
        _is_game_started = false;
        _menu.setTitle(FPSTR(texts::YOU_LOSE));
        _menu.setTitleScale(1);
        _menu.forceRedraw();
        controls::resetStates();
        return true;
      }

      // Если блок ниже текущего занят
      if (pos.y + j + 1 == TETRIS_FIELD_HEIGHT || _field[pos.x + i][pos.y + j + 1]) {
        is_blocked = true;
      }
      
      if (pos.y + j >= 0) {
        // Отрисовываем текущий блок
        display::oled.drawBitmap(
          TETRIS_FIELD_X + (pos.x + i) * TETRIS_BLOCK_SIZE,
          TETRIS_FIELD_Y + (pos.y + j) * TETRIS_BLOCK_SIZE,
          TETRIS_BLOCK,
          TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE
        );
      }
    }
  }

  // Обновляем область фигуры
  display::oled.update(
    TETRIS_FIELD_X + pos.x * TETRIS_BLOCK_SIZE,
    TETRIS_FIELD_Y + (pos.y - 1) * TETRIS_BLOCK_SIZE,
    TETRIS_FIELD_X + (pos.x + TETRIS_FIGURE_WIDTH - 1) * TETRIS_BLOCK_SIZE,
    TETRIS_FIELD_Y + (pos.y + TETRIS_FIGURE_HEIGHT - 1) * TETRIS_BLOCK_SIZE
  );
 
  if (is_blocked) {
    // Оставляем фигуру и берем следующую
    _figure_pos = { TETRIS_INITIAL_FIGURE_X, TETRIS_INITIAL_FIGURE_Y };
    _curr_figure_rot = 0;
    _curr_figure = _next_figure;
    _next_figure = getNextFigure();
    drawFigurePreview();

    // Заполняем блоки фигуры на поле
    for (int8_t i = 0; i < TETRIS_FIGURE_WIDTH; ++i) {
      for (int8_t j = 0; j < TETRIS_FIGURE_HEIGHT; ++j) {
        if (figure_data[i][j]) {
          _field[pos.x + i][pos.y + j] = true;
        }
      }
    }

    return true;
  } else {
    // Очищаем фигуру
    for (int8_t i = 0; i < TETRIS_FIGURE_WIDTH; ++i) {
      for (int8_t j = 0; j < TETRIS_FIGURE_HEIGHT; ++j) {
        if (figure_data[i][j] && pos.y + j >= 0) {
          display::oled.clear(
            TETRIS_FIELD_X + (pos.x + i) * TETRIS_BLOCK_SIZE,
            TETRIS_FIELD_Y + (pos.y + j) * TETRIS_BLOCK_SIZE,
            TETRIS_FIELD_X + (pos.x + i + 1) * TETRIS_BLOCK_SIZE - 1,
            TETRIS_FIELD_Y + (pos.y + j + 1) * TETRIS_BLOCK_SIZE - 1
          );
        }
      }
    }
  }

  // Тип перемещения. 0 - нет, 1 - влево, 2 - вправо
  uint8_t move_type = 0;
  
  // Если нажата кнопка движения
  if (controls::left_button.press()) {
    move_type = 1;
  } else if (controls::right_button.press()) {
    move_type = 2;
  }

  if (move_type != 0) {
    switch (move_type) {
      case 1:
        pos.x -= 1;
        break;
      case 2:
        pos.x += 1;
        break;
    }

    // Проверяем фигуру на новой позиции
    for (int8_t i = 0; i < TETRIS_FIGURE_WIDTH; ++i) {
      for (int8_t j = 0; j < TETRIS_FIGURE_HEIGHT; ++j) {
        if (!figure_data[i][j]) {
          continue;
        }

        // Если вышли за край
        if (
          pos.x + i >= TETRIS_FIELD_WIDTH ||
          pos.x + i < 0
        ) {
          return true;
        }

        // Если уперлись в блок на поле
        if (_field[pos.x + i][pos.y + j]) {
          return true;
        }
      }
    }

    // Сдвигаем фигуру
    switch (move_type) {
      case 1:
        _figure_pos.x -= 1;
        break;
      case 2:
        _figure_pos.x += 1;
        break;
    }
    
    return true;
  }

  // Если нажата кнопка вращения
  if (controls::button_a.press()) {
    // Получаем данные повернутой фигуры
    bool new_figure_data[4][4];
    memcpy_P(
      new_figure_data,
      TETRIS_FIGURES[_curr_figure][(_curr_figure_rot + 1) % TETRIS_FIGURE_ROTATIONS],
      sizeof(new_figure_data)
    );

    // Проверяем новую фигуру
    for (int8_t i = 0; i < TETRIS_FIGURE_WIDTH; ++i) {
      for (int8_t j = 0; j < TETRIS_FIGURE_HEIGHT; ++j) {
        if (!new_figure_data[i][j]) {
          continue;
        }

        // Если новая фигура вышла за границу
        if (
          pos.x + i >= TETRIS_FIELD_WIDTH ||
          pos.x + i < 0 ||
          pos.y + j >= TETRIS_FIELD_HEIGHT ||
          pos.y + j < 0
        ) {
          return true;
        }

        // Если новая фигура столкнулась с блоками на поле
        if (_field[pos.x + i][pos.y + j]) {
          return true;
        }
      }
    }

    _curr_figure_rot = (_curr_figure_rot + 1) % TETRIS_FIGURE_ROTATIONS;
  }

  return true;
}

void Tetris::startGame() {
  // Очищаем дисплей
  display::oled.clear();
  // Рисуем границы поля
  display::oled.rect(
    TETRIS_FIELD_X - 1,
    TETRIS_FIELD_Y - 1,
    TETRIS_FIELD_X + TETRIS_FIELD_WIDTH * TETRIS_BLOCK_SIZE,
    TETRIS_FIELD_Y + TETRIS_FIELD_HEIGHT * TETRIS_BLOCK_SIZE,
    OLED_STROKE
  );
  // Рисуем границы окна предпросмотра фигуры
  display::oled.rect(
    TETRIS_PREVIEW_X - 1,
    TETRIS_PREVIEW_Y - 1,
    TETRIS_PREVIEW_X + TETRIS_PREVIEW_WIDTH * TETRIS_BLOCK_SIZE,
    TETRIS_PREVIEW_Y + TETRIS_FIELD_HEIGHT * TETRIS_BLOCK_SIZE,
    OLED_STROKE
  );

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
  _figure_pos = { TETRIS_INITIAL_FIGURE_X, TETRIS_INITIAL_FIGURE_Y };

  drawFigurePreview();

  // Выводим всю графику на экран
  display::oled.update();
}

void Tetris::drawFigurePreview() {
  // Получаем данные о следующей фигуре
  TetrisFigure figure;
  memcpy_P(figure, TETRIS_FIGURES[_next_figure], sizeof(TetrisFigure));

  // Очищаем область
  display::oled.clear(
    TETRIS_PREVIEW_X,
    TETRIS_PREVIEW_Y,
    TETRIS_PREVIEW_X + TETRIS_PREVIEW_WIDTH * TETRIS_BLOCK_SIZE - 1,
    TETRIS_PREVIEW_Y + TETRIS_FIELD_HEIGHT * TETRIS_BLOCK_SIZE - 1
  );
  // Рисуем блоки данной фигуры
  for (uint8_t x = 0; x < TETRIS_FIGURE_WIDTH; ++x) {
    for (uint8_t y = 0; y < TETRIS_FIGURE_HEIGHT; ++y) {
      if (figure[0][x][y]) {
        display::oled.drawBitmap(
          TETRIS_PREVIEW_X + x * TETRIS_BLOCK_SIZE,
          TETRIS_PREVIEW_Y + y * TETRIS_BLOCK_SIZE,
          TETRIS_BLOCK,
          TETRIS_BLOCK_SIZE,
          TETRIS_BLOCK_SIZE
        );
      }
    }
  }
  // Выводим эту область на экран
  display::oled.update(
    TETRIS_PREVIEW_X,
    TETRIS_PREVIEW_Y,
    TETRIS_PREVIEW_X + TETRIS_PREVIEW_WIDTH * TETRIS_BLOCK_SIZE,
    TETRIS_PREVIEW_Y + TETRIS_FIELD_HEIGHT * TETRIS_BLOCK_SIZE
  );
}

uint8_t Tetris::getNextFigure() {
  if (_figures_in_bag == 0) {
    _figures_in_bag = TETRIS_FIGURES_COUNT;
    for (uint8_t i = 0; i < TETRIS_FIGURES_COUNT; ++i) {
      _figures_bag[i] = true;
    }
  }

  uint8_t figure_place = random(0, _figures_in_bag);
  uint8_t figure_index = 0;
  for (uint8_t i = 0; figure_index < figure_place; ++i) {
    if (_figures_bag[i]) {
      ++figure_index;
    }
  }

  --_figures_in_bag;
  _figures_bag[figure_index] = false;

  return figure_index;
}
