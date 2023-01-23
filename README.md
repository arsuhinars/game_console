# Arduino Game Console
Проект портативной игровой консоли на Arduino. Консоль имеет 4 кнопки
направления (D-Pad), 2 кнопки действия (A и B), OLED дисплей SSD1306 I2C 128x64
для отображения графики. Прошивка содержит 4 встроенных игры.

> Здесь будут фотографии собранного устройства

## Встроенные игры
- Змейка
- Pong
- Dino
- Tetris

## Используемые библиотеки
[EncButton](https://github.com/GyverLibs/EncButton),
[GyverOLED](https://github.com/GyverLibs/GyverOLED),
[microWire](https://github.com/GyverLibs/microWire)

## Компоненты
- [Atmega328p](https://aliexpress.ru/item/32887927630.html)
    (либо [Arduino Uno](https://aliexpress.ru/item/32556087234.htm), если на схема собрана макетной плате)
- [OLED I2C дисплей SSD1306 128x64](https://aliexpress.ru/item/32643950109.html) 
- [Бесшумные тактовые SMD кнопки 6x6x5](https://aliexpress.ru/item/4000030024707.html) 6 шт.
- [SMD переключатель](https://aliexpress.ru/item/4000030382277.html)
- [Холдер для батарейки CR2032](https://aliexpress.ru/item/4000829320626.html)

## Схемы
Принципиальная схема:<br>
<img src="https://raw.githubusercontent.com/arsuhinars/game_console/master/schemas/Schematic.png" width="600px">

Схема на основе макетной платы:<br>
<img src="https://raw.githubusercontent.com/arsuhinars/game_console/master/schemas/Breadboard.png" width="300px">

Для изготовления платы вы можете использовать [`Gerber_PCB.zip`](schemas/Gerber_PCB.zip) из директории
`schemas`. Также проект доступен на
[EasyEDA](https://oshwlab.com/arsuhinars/game-console). 

## Прошивка
Перед прошивкой необходимо установить все библиотеки из директории `libraries`.

Для прошивки микроконтроллера на плате обязательно использовать ядро
[`GyverCore`](https://github.com/AlexGyver/GyverCore). При прошивке
микроконтроллера в настройках необходимо выставить частоту процессора на
внутренний источник тактирования на 8 МГц (`Clock` -> `Internal 8 MHz`).
__ВНИМАНИЕ! Для прошивки обычной Arduino этого делать не надо!!__. Также можно
выставить новую версию компилятора для ускорения компиляции (`Compiler version` -> `AVR-GCC v8.3.0`).

## Добавление своих игр
Для создания своей игры создайте класс, наследуемый от `Game` из
`game.hpp`. В нем необходимо переопределить метод `update()`. Этот метод
вызывается в основном цикле программы. Если метод вернет `false`, то основная
программа остановит работу игры и освободит её ресурсы.

Чтобы игра появилась в главном меню:
- Добавьте строку с названием игры в `texts.hpp` и `texts.cpp`
- В файле `games.cpp` подключите включаемый файл с классом игры (`#include <hpp_файл_игры>`)
- Добавьте игру в массив `games`, добавив `getGameCreator<НАЗВАНИЕ КЛАССА>()`
- Добавьте название игры в массив `games_names`
