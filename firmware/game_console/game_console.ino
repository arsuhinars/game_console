#include "controls.hpp"
#include "display.hpp"
#include "games.hpp"
#include "menu.hpp"

// Меню с выбором игр
Menu main_menu = Menu(
  games_names,
  games_count
);

// Текущая запущенная игра
Game* current_game = nullptr;

void setup() {
  Serial.begin(9600);

  // Настраиваем ПГСЧ с помощью аппаратного источника шума
  uint32_t seed = 0;
  for (uint8_t i = 0; i < 16; i++) {
    seed *= 4;
    seed += analogRead(A0) & 3;
  }
  randomSeed(seed);

  // Инициализируем экран 
  display::init();
}

void loop() {
  // Обновляем ввод
  controls::update();

  if (current_game == nullptr) {
    // Отрисовываем текущее меню, если не запущена игра
    main_menu.update();

    // Обрабатываем запуск игры
    if (main_menu.clickedItem() != -1) {
      current_game = games[main_menu.clickedItem()]();

      main_menu.handleClickedItem();
    }
  } else if (!current_game->update()) {
    // Освобождаем память, если игра была закрыта
    delete current_game;
    current_game = nullptr;

    main_menu.forceRedraw();
  }
}
