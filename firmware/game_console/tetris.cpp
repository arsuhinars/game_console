#include <Arduino.h>

#include "display.hpp"
#include "controls.hpp"
#include "texts.hpp"
#include "utils.hpp"
#include "tetris.hpp"

// Координаты поля на экране
#define TETRIS_FIELD_X  ((DISPLAY_WIDTH - TETRIS_FIELD_WIDTH * TETRIS_BLOCK_SIZE) / 2)
#define TETRIS_FIELD_Y  ((DISPLAY_HEIGHT - TETRIS_FIELD_HEIGHT * TETRIS_BLOCK_SIZE) / 2)

// Макс. кол-во вращений фигуры
#define TETRIS_FIGURE_ROTATIONS   4

// Кол-во блоков в фигуре тетрамино.
// Содержит 4 блока по определению фигуры тетрамино
#define TETRIS_FIGURE_BLOCKS_COUNT    4

using namespace utils;

// Спрайт блока
const uint8_t TETRIS_BLOCK[] PROGMEM = {
	0x03, 0x03, 0x00,
};

// Тип для описания фигуры. Представляет из себя массив с координатами
// каждого блока фигуры относительно центра. 
typedef cvec2 TetrisFigure[TETRIS_FIGURE_BLOCKS_COUNT];

// Массив со всеми фигурами
const TetrisFigure TETRIS_FIGURES[TETRIS_FIGURES_COUNT] PROGMEM = {
  // Фигура L
  { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 1, 1 } },
  // Фигура J
  { { 0, -1 }, { 0, 0 }, { 0, 1 }, { -1, 1 } },
  // Фигура Z
  { { -1, 0 }, { 0, 0 }, { 0, 1 }, { 1, 1 } },
  // Фигура S
  { { -1, 1 }, { 0, 1 }, { 0, 0 }, { 1, 0 } },
  // Фигура T
  { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, 1 } },
  // Фигура I
  { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 0, 2 } },
  // Фигура O
  { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }
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

  // Если есть заполненные строчки
  if (
    _filled_rows_count > 0 &&
    millis() - _last_time >= TETRIS_DISAPPEAR_TIME / TETRIS_FIELD_WIDTH
  ) {
    _last_time = millis();

    // Очищаем заполненные строки по одному столбцу
    for (uint8_t i = 0; i < TETRIS_FIELD_HEIGHT; ++i) {
      if (!_filled_rows[i]) {
        continue;
      }

      display::oled.clear(
        TETRIS_FIELD_X + _disappearing_col * TETRIS_BLOCK_SIZE,
        TETRIS_FIELD_Y + i * TETRIS_BLOCK_SIZE,
        TETRIS_FIELD_X + _disappearing_col * TETRIS_BLOCK_SIZE + TETRIS_BLOCK_SIZE - 1,
        TETRIS_FIELD_Y + i * TETRIS_BLOCK_SIZE + TETRIS_BLOCK_SIZE - 1
      );
      display::oled.update(
        TETRIS_FIELD_X + _disappearing_col * TETRIS_BLOCK_SIZE,
        TETRIS_FIELD_Y + i * TETRIS_BLOCK_SIZE,
        TETRIS_FIELD_X + _disappearing_col * TETRIS_BLOCK_SIZE + TETRIS_BLOCK_SIZE,
        TETRIS_FIELD_Y + i * TETRIS_BLOCK_SIZE + TETRIS_BLOCK_SIZE
      );
    }

    // Если дошли до конца
    if (_disappearing_col == TETRIS_FIELD_WIDTH - 1) {
      uint8_t last_row = 0;
      while (true) {
        // Ищем следующую заполненную строку
        uint8_t offset = 1;
        uint8_t row = TETRIS_FIELD_HEIGHT;
        for (uint8_t i = last_row; i < TETRIS_FIELD_HEIGHT; ++i) {
          if (_filled_rows[i]) {
            // Если строки идут подряд, то просто увеличиваем смещение
            if (i + 1 < TETRIS_FIELD_HEIGHT && _filled_rows[i + 1]) {
              ++offset;
              continue;
            }

            row = i;
            break;
          }
        }

        // Если обошли все строчки
        if (row == TETRIS_FIELD_HEIGHT) {
          break;
        }

        // Смещаем все блоки до найденной строки
        for (int8_t y = row; y >= 0; --y) {
          for (int8_t x = 0; x < TETRIS_FIELD_WIDTH; ++x) {
            _field[x][y] = y >= offset ? _field[x][y - offset] : false;
          }
        }

        last_row = row + 1;
      }

      // Отрисовываем все поле заново
      for (uint8_t x = 0; x < TETRIS_FIELD_WIDTH; ++x) {
        for (uint8_t y = 0; y < TETRIS_FIELD_HEIGHT; ++y) {
          if (_field[x][y]) {
            // Рисуем текущий блок
            display::oled.drawBitmap(
              TETRIS_FIELD_X + x * TETRIS_BLOCK_SIZE,
              TETRIS_FIELD_Y + y * TETRIS_BLOCK_SIZE,
              TETRIS_BLOCK,
              TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE
            );
          } else {
            // Очищаем его, если он пустой
            display::oled.clear(
              TETRIS_FIELD_X + x * TETRIS_BLOCK_SIZE,
              TETRIS_FIELD_Y + y * TETRIS_BLOCK_SIZE,
              TETRIS_FIELD_X + x * TETRIS_BLOCK_SIZE + TETRIS_BLOCK_SIZE - 1,
              TETRIS_FIELD_Y + y * TETRIS_BLOCK_SIZE + TETRIS_BLOCK_SIZE - 1
            );
          }
        }
      }

      // Начисляем игроку очки за заполенные строки
      switch (_filled_rows_count) {
        case 2:
          _score += 3;
        case 3:
          _score += 7;
        case 4:
          _score += 15;
        default:
          _score += 1;
      }

      // Обновляем текст счета
      display::oled.setCursorXY(
        TETRIS_SCORE_X,
        TETRIS_SCORE_Y + DISPLAY_FONT_HEIGHT
      );
      size_t length = display::oled.print(_score);
      length += display::oled.print(F("00"));
      // Обновляем область текста на экране
      display::oled.update(
        TETRIS_SCORE_X,
        TETRIS_SCORE_Y + DISPLAY_FONT_HEIGHT,
        TETRIS_SCORE_X + length * DISPLAY_FONT_WIDTH,
        TETRIS_SCORE_Y + DISPLAY_FONT_HEIGHT * 2
      );

      // Обновляем нужную область на экране
      display::oled.update(
        TETRIS_FIELD_X,
        TETRIS_FIELD_Y,
        TETRIS_FIELD_X + TETRIS_FIELD_WIDTH * TETRIS_BLOCK_SIZE,
        TETRIS_FIELD_Y + TETRIS_FIELD_HEIGHT * TETRIS_BLOCK_SIZE
      );

      // Сбрасываем заполненные строки
      _filled_rows_count = 0;
    }

    ++_disappearing_col;

    return true;
  }

  // Обновляем игру только тогда, когда это необходимо
  if (millis() - _last_time <= (controls::down_button.state() ? TETRIS_FAST_UPDATE_STEP : TETRIS_UPDATE_STEP)) {
    return true;
  }
  _last_time = millis();

  // Получаем данные текущей фигуры
  TetrisFigure figure;
  // Получаем данные текущей фигуры
  memcpy_P(figure, TETRIS_FIGURES[_curr_figure], sizeof(TetrisFigure));

  // Границы для обновления экрана
  auto draw_bound_min = _bound_min;
  auto draw_bound_max = _bound_max;

  // Перебираем блоки фигуры
  bool is_dropped = false;
  for (uint8_t i = 0; i < TETRIS_FIGURE_BLOCKS_COUNT; ++i) {
    // Позиция текущего блока на поле
    auto pos = transformPoint(_figure_pos, _figure_rot, figure[i]);

    // Обновляем границы
    draw_bound_min = {
      min(draw_bound_min.x, pos.x),
      min(draw_bound_min.y, pos.y)
    };
    draw_bound_max = {
      max(draw_bound_max.x, pos.x),
      max(draw_bound_max.y, pos.y)
    };
    if (i == 0) {
      _bound_min = pos;
      _bound_max = pos;
    } else {
      _bound_min = {
        min(_bound_min.x, pos.x),
        min(_bound_min.y, pos.y)
      };
      _bound_max = {
        max(_bound_max.x, pos.x),
        max(_bound_max.y, pos.y)
      };
    }

    // Если блок уже занят
    if (pos.y >= 0 && _field[pos.x][pos.y]) {
      // Игрок проиграл
      _is_game_started = false;
      _menu.setTitle(FPSTR(texts::YOU_LOSE));
      _menu.setTitleScale(1);
      _menu.forceRedraw();
      controls::resetStates();
      return true;
    }

    // Если блок ниже текущего занят
    if (pos.y + 1 == TETRIS_FIELD_HEIGHT || _field[pos.x][pos.y + 1]) {
      is_dropped = true;
    }

    if (pos.y >= 0) {
      // Рисуем текущий блок
      display::oled.drawBitmap(
        TETRIS_FIELD_X + pos.x * TETRIS_BLOCK_SIZE,
        TETRIS_FIELD_Y + pos.y * TETRIS_BLOCK_SIZE,
        TETRIS_BLOCK,
        TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE
      );
    }
  }

  if (_was_dropped) {
    draw_bound_min = _bound_min;
    draw_bound_max = _bound_max;
  }
  _was_dropped = is_dropped;

  // Обновляем только изменившуюся область
  display::oled.update(
    TETRIS_FIELD_X + draw_bound_min.x * TETRIS_BLOCK_SIZE,
    TETRIS_FIELD_Y + draw_bound_min.y * TETRIS_BLOCK_SIZE,
    TETRIS_FIELD_X + draw_bound_max.x * TETRIS_BLOCK_SIZE + TETRIS_BLOCK_SIZE,
    TETRIS_FIELD_Y + draw_bound_max.y * TETRIS_BLOCK_SIZE + TETRIS_BLOCK_SIZE
  );
 
  if (is_dropped) {
    // Заполняем блоки фигуры на поле
    for (uint8_t i = 0; i < TETRIS_FIGURE_BLOCKS_COUNT; ++i) {
      auto pos = transformPoint(_figure_pos, _figure_rot, figure[i]);
      _field[pos.x][pos.y] = true;
    }

    // Оставляем фигуру и берем следующую
    _figure_pos = { TETRIS_INITIAL_FIGURE_X, TETRIS_INITIAL_FIGURE_Y };
    _figure_rot = 0;
    _curr_figure = _next_figure;
    _next_figure = getNextFigure();
    drawFigurePreview();

    // Ищем заполненные строки
    for (uint8_t y = _bound_min.y; y <= _bound_max.y; ++y) {
      bool is_filled = true;
      for (uint8_t x = 0; x < TETRIS_FIELD_WIDTH; ++x) {
        if (!_field[x][y]) {
          is_filled = false;
          break;
        }
      }

      // Записываем эти строки в массиве
      _filled_rows[y] = is_filled;
      // Сбрасываем счетчик исчезающих столбцов
      if (is_filled) {
        ++_filled_rows_count;
        _disappearing_col = 0;
      }
    }

    return true;
  } else {
    // Очищаем фигуру
    for (uint8_t i = 0; i < TETRIS_FIGURE_BLOCKS_COUNT; ++i) {
      auto pos = transformPoint(_figure_pos, _figure_rot, figure[i]);
      if (pos.y >= 0) {
        display::oled.clear(
          TETRIS_FIELD_X + pos.x * TETRIS_BLOCK_SIZE,
          TETRIS_FIELD_Y + pos.y * TETRIS_BLOCK_SIZE,
          TETRIS_FIELD_X + pos.x * TETRIS_BLOCK_SIZE + TETRIS_BLOCK_SIZE - 1,
          TETRIS_FIELD_Y + pos.y * TETRIS_BLOCK_SIZE + TETRIS_BLOCK_SIZE - 1
        );
      }
    }

    // Смещаем фигуру вниз
    ++_figure_pos.y;
  }

  // Тип перемещения. 0 - нет, 1 - влево, 2 - вправо
  uint8_t move_type = 0;
  
  // Если нажата кнопка движения
  if (controls::left_button.press() || controls::left_button.state()) {
    move_type = 1;
  } else if (controls::right_button.press() || controls::right_button.state()) {
    move_type = 2;
  }

  if (move_type != 0) {
    cvec2 new_figure_pos = _figure_pos;
    switch (move_type) {
      case 1:
        new_figure_pos.x -= 1;
        break;
      case 2:
        new_figure_pos.x += 1;
        break;
    }

    // Проверяем фигуру на новой позиции
    for (uint8_t i = 0; i < TETRIS_FIGURE_BLOCKS_COUNT; ++i) {
      auto pos = transformPoint(new_figure_pos, _figure_rot, figure[i]);

      // Если вышли за край или уперлись в блок на поле
      if (
        pos.x >= TETRIS_FIELD_WIDTH ||
        pos.x < 0 ||
        _field[pos.x][pos.y]
      ) {
        return true;
      }
    }

    // Сдвигаем фигуру
    _figure_pos = new_figure_pos;
  }

  // Если нажата кнопка вращения
  if (controls::button_a.press() || controls::button_a.state()) {
    uint8_t figure_rotations = TETRIS_FIGURE_ROTATIONS;
    // Добавляем ограничения на вращения определенных фигур
    switch (_curr_figure) {
      case TETRIS_FIGURE_I:
        figure_rotations = 2;
      case TETRIS_FIGURE_O:
        figure_rotations = 1;
    }

    // Проверяем новую фигуру
    for (uint8_t i = 0; i < TETRIS_FIGURE_BLOCKS_COUNT; ++i) {
      auto pos = transformPoint(
        _figure_pos,
        (_figure_rot + 1) % TETRIS_FIGURE_ROTATIONS,
        figure[i]
      );
      
      // Если новая фигура вышла за границу или столкнулась с блоками на поле
      if (
        pos.x >= TETRIS_FIELD_WIDTH ||
        pos.x < 0 ||
        pos.y >= TETRIS_FIELD_HEIGHT ||
        _field[pos.x][pos.y]
      ) {
        return true;
      }
    }

    // Применяем вращение
    _figure_rot = (_figure_rot + 1) % TETRIS_FIGURE_ROTATIONS;
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
    TETRIS_PREVIEW_Y + TETRIS_PREVIEW_HEIGHT * TETRIS_BLOCK_SIZE,
    OLED_STROKE
  );
  // Рисуем текст с очками игрока
  display::oled.textMode(BUF_REPLACE);
  display::oled.setScale(1);
  display::oled.setCursorXY(TETRIS_SCORE_X, TETRIS_SCORE_Y);
  display::oled.println(FPSTR(texts::SCORE));
  display::oled.print('0');

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
  _figure_rot = 0;
  _figure_pos = { TETRIS_INITIAL_FIGURE_X, TETRIS_INITIAL_FIGURE_Y };
  _was_dropped = true;
  _filled_rows_count = 0;

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
    TETRIS_PREVIEW_Y + TETRIS_PREVIEW_HEIGHT * TETRIS_BLOCK_SIZE - 1
  );

  // Рисуем блоки данной фигуры
  for (uint8_t i = 0; i < TETRIS_FIGURE_BLOCKS_COUNT; ++i) {
    display::oled.drawBitmap(
      TETRIS_PREVIEW_X + (figure[i].x + TETRIS_PREVIEW_FIGURE_X) * TETRIS_BLOCK_SIZE,
      TETRIS_PREVIEW_Y + (figure[i].y + TETRIS_PREVIEW_FIGURE_Y) * TETRIS_BLOCK_SIZE,
      TETRIS_BLOCK,
      TETRIS_BLOCK_SIZE,
      TETRIS_BLOCK_SIZE
    );
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

  uint8_t figure_offset = random(0, _figures_in_bag);
  uint8_t offset = 0;
  uint8_t figure_index;
  for (uint8_t i = 0; offset <= figure_offset; ++i) {
    if (_figures_bag[i]) {
      ++offset;
      figure_index = i;
    }
  }

  --_figures_in_bag;
  _figures_bag[figure_index] = false;

  return figure_index;
}

cvec2 Tetris::transformPoint(const cvec2& pos, uint8_t rot, const cvec2& v) {
  switch (rot) {
    case 1:
      return { pos.x - v.y, pos.y + v.x };
    case 2:
      return { pos.x - v.x, pos.y - v.y };
    case 3:
      return { pos.x + v.y, pos.y - v.x };
    default:
      return { pos.x + v.x, pos.y + v.y };
  }
}
