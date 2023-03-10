#pragma once

#include <Arduino.h>

// Размеры отступов между пунктами меню
#define MENU_ITEMS_SPACING    4
// Отступ между текстом пункта меню и курсором
#define MENU_CURSOR_SPACING   4
// Толщина ползунка
#define MENU_SLIDER_THICKNESS 2
// Минимальный размер ползунка
#define MENU_SLIDER_MIN_SIZE  8

// Класс для создания и отображения наэкранного меню.
class Menu {
public:
  /**
  * Конструктор класса
  * @param items PROGMEM(!) массив F() строк элементов меню
  * @param items_count размер массива элементов
  * @param title F() строка с текстом заголовка меню. Не является обязательной.
  * @param title_scale Маштаб текста заголовка. По умолчанию - 2
  */
  explicit Menu(
    const char* const *items,
    uint8_t items_count,
    const __FlashStringHelper* title = nullptr,
    uint8_t title_scale = 2
  );
  ~Menu();

  /// Изменить заголовок меню
  void setTitle(const __FlashStringHelper* title);
  
  /// Изменить маштаб шрифта заголовка
  void setTitleScale(uint8_t title_scale);

  /// Метод для отрисовки и обновления меню на экране
  void update();

  /**
  * Возвращает индекс элемента при клике на него.
  * Если элемент не был нажат, то возвращает -1.
  * Данная функция используется в основном цикле после вызов update()
  */
  int clickedItem();

  /**
  * Метод обработки и сброса состояния нажатого элемента. 
  * Необходимо вызывать после его обработки.
  */
  void handleClickedItem();

  /**
  * Установить флаг вынужденной переотрисовки.
  * Необходим, если меню было закрыто и открыто вновь.
  */
  void forceRedraw();

private:
  const char* const * _items;
  uint8_t _items_count;
  const __FlashStringHelper* _title;
  uint8_t _title_scale; 

  bool _need_redraw = true;
  uint8_t _items_scroll = 0;
  uint8_t _selected_item = 0;
  bool _is_item_clicked = false;
};
