#! /usr/bin/python3
""" Скрипт, конвертирующий .csv-файлы в JSON-разметку уровня для движка anger_engine.
Использование: ./levelconv.py <w> <h> <filename.csv> <filename.res> <filename.lvl>.
Скрипт не создает готовый к использованию уровень, некоторые поля необходимо ввести вручную
ВНИМАНИЕ: для запуска необходим Python 3.10 или выше"""

import sys
from lcv.output import output
from lcv.read_lvl import read_lvl
from lcv.conv import conv
from lcv.read_res import read_res

if __name__ == '__main__':
    if len(sys.argv) == 6:
        try:
            raw_lvl = read_lvl(sys.argv[3], int(sys.argv[1]), int(sys.argv[2]))
        except ValueError as e:
            print('Некорректное значение аргумента!', '\n', e)
            quit()
        lvl = conv(raw_lvl)
        texture_dict = read_res(sys.argv[4])
        output(lvl, texture_dict, sys.argv[5])
        print('Уровень успешно сконвертирован. \nТеперь, чтобы уровень можно было использовать в игре, Вам необходимо '
              'вручную заполнить поля "lvl_name", "sky_bg" в полученном файле .lvl; также допишите в список "textures" '
              'имя файла skybox-изображения. \n'
              'Приятной игры!')
