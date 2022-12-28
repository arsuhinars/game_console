#include "controls.hpp"
#include "display.hpp"
#include "utils.hpp"
#include "menu.hpp"

using namespace utils;

#define MENU_ICON_SIZE    8
#define MENU_TITLE_HEIGHT (16 + MENU_ITEMS_SPACING)
#define MENU_ITEM_HEIGHT  (8 + MENU_ITEMS_SPACING)

// Иконки указателей в меню размером 8x8
const uint8_t chevron_right[] PROGMEM = {
  0x00, 0x00, 0x81, 0xc3, 0x66, 0x3c, 0x18, 0x00
};
const uint8_t chevron_left[] PROGMEM = {
  0x00, 0x18, 0x3c, 0x66, 0xc3, 0x81, 0x00, 0x00
};

Menu::Menu(
  const __FlashStringHelper** items,
  uint8_t items_count,
  const __FlashStringHelper* title
) {
  _items = items;
  _items_count = items_count;
  _title = title;
}

Menu::~Menu() { }

void Menu::update() {
  // Обрабатываем нажатие вверх
  if (controls::up_button.press() && _selected_item > 0) {
    if (_selected_item == _items_scroll) {
      --_items_scroll;
    }

    --_selected_item;
    _need_redraw = true;
  }

  // Обрабатываем нажатие вниз
  if (controls::down_button.press() && _selected_item < _items_count - 1) {
    uint8_t max_display_items = 
      _title != nullptr ? 
      (DISPLAY_HEIGHT - MENU_TITLE_HEIGHT - 1) / MENU_ITEM_HEIGHT + 1 :
      (DISPLAY_HEIGHT - 1) / MENU_ITEM_HEIGHT + 1;

    if (_selected_item == (_items_scroll + max_display_items - 1)) {
      ++_items_scroll;
    }

    ++_selected_item;
    _need_redraw = true;
  }

  // Делаем отрисовку только при необходимости
  if (!_need_redraw) {
    return;
  }
  _need_redraw = false;

  // Очищаем дисплей и сбрасываем состояния
  display::oled.clear();
  display::oled.textMode(BUF_REPLACE);

  int item_y = 0;

  if (_title != nullptr) {
    size_t title_length = rus_strlen_P(reinterpret_cast<PGM_P>(_title));

    // Отрисовываем заголовок
    display::oled.setScale(2);
    display::oled.setCursorXY(DISPLAY_WIDTH / 2 - title_length * 6, 0);
    display::oled.print(_title);

    item_y = MENU_TITLE_HEIGHT;
  }

  // Отрисовываем пункты меню
  display::oled.setScale(1);
  for (size_t i = _items_scroll; i < (int)_items_count; ++i) {
    size_t item_length = rus_strlen_P(reinterpret_cast<PGM_P>(_items[i]));
    int item_x = DISPLAY_WIDTH / 2 - item_length * 3;

    display::oled.setCursorXY(item_x, item_y);
    display::oled.print(_items[i]);

    if (i == _selected_item) {
      // Отрисовываем курсор
      display::oled.drawBitmap(
        item_x - MENU_ICON_SIZE - MENU_CURSOR_SPACING,
        item_y,
        chevron_right,
        MENU_ICON_SIZE, MENU_ICON_SIZE
      );
      display::oled.drawBitmap(
        item_x + item_length * 6 + MENU_CURSOR_SPACING,
        item_y,
        chevron_left,
        MENU_ICON_SIZE, MENU_ICON_SIZE
      );
    }

    item_y += MENU_ITEM_HEIGHT;
    if (item_y >= DISPLAY_HEIGHT) {
      break;
    }
  }

  // Обновляем дисплей
  display::oled.update();
}

int Menu::clickedItem() {
  return _is_item_clicked ? _selected_item : -1;
}

void Menu::handleClickedItem() {
  _is_item_clicked = false;
}