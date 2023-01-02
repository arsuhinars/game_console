#include <Arduino.h>

#include "controls.hpp"
#include "display.hpp"
#include "texts.hpp"
#include "pong.hpp"

#define BALL_SPRITE_SIZE 9

// Спрайт мяча
const uint8_t ball_sprite[] PROGMEM = {
	0x38, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0x38, 
	0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 
};

const __FlashStringHelper* MENU_ITEMS[] = {
  FPSTR(texts::ONE_PLAYER), FPSTR(texts::TWO_PLAYERS), FPSTR(texts::QUIT)
};

Pong::Pong() : _menu(MENU_ITEMS, 3, FPSTR(texts::PONG_NAME)) {

}

bool Pong::update() {
  bool need_redraw = false;
  auto old_ball_pos = _ball_pos;

  switch (_game_data.state) {
    case GameState::MAIN_MENU:
      _menu.update();

      if (_menu.clickedItem() == 2) {
        return false;
      } else if (_menu.clickedItem() != -1) {
        _game_data.type = 
          _menu.clickedItem() == 0 ?
          GameType::ONE_PLAYER :
          GameType::TWO_PLAYERS;
        _game_data.left_score = 0;
        _game_data.right_score = 0;

        _menu.handleClickedItem();

        startNewRound();
      }
      return true;
    case GameState::IDLE:
    case GameState::IDLE_REDRAW:
      if (millis() - _last_time > IDLE_TIME) {
        _game_data.state = GameState::ROUND;
        return true;
      }
      break;
    case GameState::ROUND:
      if (millis() - _last_time > UPDATE_STEP) {
        _last_time = millis();
        need_redraw = true;
        
        // Ограничиваем скорость мяча
        _ball_vel.x = constrain(_ball_vel.x, -BALL_MAX_SPEED, BALL_MAX_SPEED);
        _ball_vel.y = constrain(_ball_vel.y, -BALL_MAX_SPEED, BALL_MAX_SPEED);

        // Обновляем положение мяча
        _ball_pos.x += _ball_vel.x * UPDATE_STEP / 1000;
        _ball_pos.y += _ball_vel.y * UPDATE_STEP / 1000;

        int16_t ball_left = _ball_pos.x / SUBPIXELS_COUNT - BALL_SPRITE_SIZE / 2;
        int16_t ball_right = _ball_pos.x / SUBPIXELS_COUNT + BALL_SPRITE_SIZE / 2;
        int16_t ball_top = _ball_pos.y / SUBPIXELS_COUNT - BALL_SPRITE_SIZE / 2;
        int16_t ball_bottom = _ball_pos.y / SUBPIXELS_COUNT + BALL_SPRITE_SIZE / 2;

        int16_t left_racket_top = _left_racket_pos / SUBPIXELS_COUNT - RACKET_LENGTH / 2;
        int16_t left_racket_bottom = _left_racket_pos / SUBPIXELS_COUNT + RACKET_LENGTH / 2;

        int16_t right_racket_top = _right_racket_pos / SUBPIXELS_COUNT - RACKET_LENGTH / 2;
        int16_t right_racket_bottom = _right_racket_pos / SUBPIXELS_COUNT + RACKET_LENGTH / 2;

        // Обрабатываем столкновение мяча с верхней и нижней стенкой
        if ( ball_top <= 0 || ball_bottom >= DISPLAY_HEIGHT - 1) {
          _ball_vel.y *= -1;
        }

        // Обрабатываем столкновения с левой ракеткой
        if (ball_left <= RACKET_THICKNESS - 1 && (
          (ball_top <= left_racket_bottom && ball_top >= left_racket_top) ||
          (ball_bottom <= left_racket_bottom && ball_bottom >= left_racket_top)
        )) {
          _ball_pos.x = (RACKET_THICKNESS + BALL_SPRITE_SIZE / 2) * SUBPIXELS_COUNT;
          _ball_vel.x *= -1;
          _ball_vel.y += _left_racket_vel;
        }

        // Обрабатываем столковения с правой ракеткой
        if (ball_right >= DISPLAY_WIDTH - RACKET_THICKNESS && (
          (ball_top <= right_racket_bottom && ball_top >= right_racket_top) ||
          (ball_bottom <= right_racket_bottom && ball_bottom >= right_racket_top)
        )) {
          _ball_pos.x = (DISPLAY_WIDTH - 1 - RACKET_THICKNESS - BALL_SPRITE_SIZE / 2) * SUBPIXELS_COUNT;
          _ball_vel.x *= -1;
          _ball_vel.y += _left_racket_vel;
        }

        // Обработка выхода за границы экрана
        if (ball_left <= 0) {
          ++_game_data.right_score;
          startNewRound();
        } else if (ball_right >= DISPLAY_WIDTH - 1) {
          ++_game_data.left_score;
          startNewRound();
        }

        // Считываем движение левой ракетки
        int16_t left_racket_accel = 0;
        if (controls::up_button.state()) {
          left_racket_accel = -RACKET_ACCEL;
        } else if (controls::down_button.state()) {
          left_racket_accel = RACKET_ACCEL;
        }

        // Обновляем левую ракетку
        _left_racket_pos = utils::smoothMove(
          _left_racket_pos,
          _left_racket_vel,
          left_racket_accel,
          RACKET_DRAG,
          UPDATE_STEP
        );

        // Ограничиваем движение левой ракетки
        if (left_racket_top < 0) {
          _left_racket_pos = RACKET_LENGTH * SUBPIXELS_COUNT / 2;
          _left_racket_vel = 0;
        } else if (left_racket_bottom >= DISPLAY_HEIGHT) {
          _left_racket_pos = (DISPLAY_HEIGHT - 1 - RACKET_LENGTH / 2) * SUBPIXELS_COUNT;
          _left_racket_vel = 0;
        }

        int16_t right_racket_accel = 0;
        if (_game_data.type == GameType::ONE_PLAYER) {
          int16_t target_y = _ball_pos.y;
          if (_ball_pos.x / SUBPIXELS_COUNT < DISPLAY_WIDTH * 3 / 4) {
            target_y = DISPLAY_HEIGHT * SUBPIXELS_COUNT - _ball_pos.y;
          }

          // Если игра в одиночку, то правая ракетка управляется псевдо ИИ
          right_racket_accel = 
            _right_racket_pos > target_y ?
            -RACKET_ACCEL :
            RACKET_ACCEL;
        }
        // Если игра вдвоем
        else if (controls::button_b.state()) {
          right_racket_accel = -RACKET_ACCEL;
        } else if (controls::button_a.state()) {
          right_racket_accel = RACKET_ACCEL;
        }

        // Обновляем правую ракетку
        _right_racket_pos = utils::smoothMove(
          _right_racket_pos,
          _right_racket_vel,
          right_racket_accel,
          RACKET_DRAG,
          UPDATE_STEP
        );

        // Ограничиваем движение правой ракетки
        if (right_racket_top < 0) {
          _right_racket_pos = RACKET_LENGTH * SUBPIXELS_COUNT / 2;
          _right_racket_vel = 0;
        } else if (right_racket_bottom >= DISPLAY_HEIGHT) {
          _right_racket_pos = (DISPLAY_HEIGHT - 1 - RACKET_LENGTH / 2) * SUBPIXELS_COUNT;
          _right_racket_vel = 0;
        }
      }
      break;
  }

  if (_game_data.state == GameState::IDLE_REDRAW) {
    _game_data.state = GameState::IDLE;
    display::oled.clear();
  } else if (need_redraw) {
    old_ball_pos.x /= SUBPIXELS_COUNT;
    old_ball_pos.y /= SUBPIXELS_COUNT;

    // Очищаем только те области, которые изменились
    display::oled.clear(
      old_ball_pos.x - BALL_SPRITE_SIZE / 2,
      old_ball_pos.y - BALL_SPRITE_SIZE / 2,
      old_ball_pos.x + BALL_SPRITE_SIZE / 2,
      old_ball_pos.y + BALL_SPRITE_SIZE / 2
    );

    // Очищаем область у левой ракетки
    display::oled.clear(
      0,
      0,
      RACKET_THICKNESS - 1,
      _left_racket_pos / SUBPIXELS_COUNT - RACKET_LENGTH / 2
    );
    display::oled.clear(
      0,
      _left_racket_pos / SUBPIXELS_COUNT + RACKET_LENGTH / 2,
      RACKET_THICKNESS - 1,
      DISPLAY_HEIGHT - 1
    );

    // Аналогично у правой
    display::oled.clear(
      DISPLAY_WIDTH - RACKET_THICKNESS,
      0,
      DISPLAY_WIDTH - 1,
      _right_racket_pos / SUBPIXELS_COUNT - RACKET_LENGTH / 2
    );
    display::oled.clear(
      DISPLAY_WIDTH - RACKET_THICKNESS,
      _right_racket_pos / SUBPIXELS_COUNT + RACKET_LENGTH / 2,
      DISPLAY_WIDTH - 1,
      DISPLAY_HEIGHT - 1
    );
  } else {
    // Если не требуется перерисовка
    return true;
  }

  // Отрисовываем мяч
  display::oled.drawBitmap(
    _ball_pos.x / SUBPIXELS_COUNT - BALL_SPRITE_SIZE / 2,
    _ball_pos.y / SUBPIXELS_COUNT - BALL_SPRITE_SIZE / 2,
    ball_sprite,
    BALL_SPRITE_SIZE,
    BALL_SPRITE_SIZE,
    0, BUF_REPLACE
  );

  // Отрисовываем ракетки
  display::oled.rect(
    0,
    _left_racket_pos / SUBPIXELS_COUNT - RACKET_LENGTH / 2,
    RACKET_THICKNESS - 1,
    _left_racket_pos / SUBPIXELS_COUNT + RACKET_LENGTH / 2
  );
  display::oled.rect(
    DISPLAY_WIDTH - RACKET_THICKNESS,
    _right_racket_pos / SUBPIXELS_COUNT - RACKET_LENGTH / 2,
    DISPLAY_WIDTH - 1,
    _right_racket_pos / SUBPIXELS_COUNT + RACKET_LENGTH / 2
  );

  // Отрисовываем счет
  display::oled.setCursorXY(
    DISPLAY_WIDTH / 2 - DISPLAY_FONT_WIDTH * (sizeof(_score_text) - 1) / 2, 0
  );
  display::oled.print(_score_text);

  // Обновляем дисплей
  display::oled.update();

  return true;
}

void Pong::startNewRound() {
  // Если одна из сторон победила
  if (_game_data.left_score >= MAX_SCORE || _game_data.right_score >= MAX_SCORE) {
    __FlashStringHelper* menu_title;

    // Меняем заголовок в зависимости от того, кто выиграл
    if (_game_data.left_score == _game_data.right_score) {
      _menu.setTitle(FPSTR(texts::DRAW));
    } else if (_game_data.left_score > _game_data.right_score) {
      _menu.setTitle(
        _game_data.type == GameType::ONE_PLAYER ?
        FPSTR(texts::YOU_WIN) :
        FPSTR(texts::LEFT_WIN)
      );
    } else {
      _menu.setTitle(
        _game_data.type == GameType::ONE_PLAYER ?
        FPSTR(texts::YOU_LOSE) :
        FPSTR(texts::RIGHT_WIN)
      );
    }

    controls::resetStates();

    _menu.setTitleScale(1);
    _menu.forceRedraw();

    // Возвращаемся в главное меню
    _game_data.state = GameState::MAIN_MENU;
    return;
  }
  
  // Сбрасываем состояние игры
  _game_data.state = GameState::IDLE_REDRAW;
  _last_time = millis();

  // Сбрасываем положения и скорости ракеток
  _left_racket_pos = DISPLAY_HEIGHT / 2 * SUBPIXELS_COUNT;
  _left_racket_vel = 0;
  _right_racket_pos = DISPLAY_HEIGHT / 2 * SUBPIXELS_COUNT;
  _right_racket_vel = 0;

  // Сбрасываем мяч и генерируем случайное направление движения
  _ball_pos.x = DISPLAY_WIDTH / 2 * SUBPIXELS_COUNT;
  _ball_pos.y = DISPLAY_HEIGHT  / 2 * SUBPIXELS_COUNT;
  _ball_vel.x = random(BALL_MAX_SPEED / 5, BALL_MAX_SPEED + 1) * SUBPIXELS_COUNT;
  _ball_vel.y = random(BALL_MAX_SPEED / 5, BALL_MAX_SPEED + 1) * SUBPIXELS_COUNT;
  if (random(2)) {
    _ball_vel.x *= -1;
  }
  if (random(2)) {
    _ball_vel.y *= -1;
  }

  // Формируем строку с текстом счета
  _score_text[0] = _game_data.left_score < 10 ? ' ' : '0' + _game_data.left_score / 10;
  _score_text[1] = '0' + _game_data.left_score % 10;
  _score_text[2] = '-';
  if (_game_data.right_score < 10) {
    _score_text[3] = '0' + _game_data.right_score % 10;
    _score_text[4] = ' ';
  } else {
    _score_text[3] = '0' + _game_data.right_score / 10;
    _score_text[4] = '0' + _game_data.right_score % 10;
  }
  _score_text[5] = '\0';
}
