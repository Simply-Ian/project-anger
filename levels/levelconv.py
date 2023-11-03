#! /usr/bin/python3
"""Скрипт, конвертирующий .csv-файлы в JSON-разметку уровня для движка anger_engine.
Использование: ./levelconv.py <w> <h> <filename.csv> <filename.res> <filename.lvl>.
Скрипт не создает готовый к использованию уровень, некоторые поля необходимо ввести вручную"""

import json
import sys
from typing import List
from dataclasses import dataclass


@dataclass(slots=True, frozen=True)
class FloorTile:
    x: int
    y: int
    texture: str


# class TextureSet (NamedTuple):
#     Top: str
#     Right: str
#     Left: str
#     Bottom: str


@dataclass(slots=True, frozen=True)
class Wall:
    x: int
    y: int
    textures: List[str]


@dataclass(slots=True, frozen=True)
class UnpackedLevel:
    h: int
    w: int
    floor_tiles: List[FloorTile]
    walls: List[Wall]
    player_x: int
    player_y: int


@dataclass(slots=True, frozen=True)
class CellInfo:
    wall_or_tile: bool # True для wall
    is_player_spawn: bool


def read_lvl(filepath: str, w: int, h: int) -> List[List[str]]:
    matrix = []
    with open(filepath, 'r') as FILE:
        for r in range(h):
            raw_text = FILE.readline()
            # Последние пустые строки не записываются в csv- файл, поэтому, если строка не последняя,
            # заполнить ее кодом плитки по умолчанию, иначе -- блоками стены по умолчанию
            if not raw_text:
                if r < h - 1:
                    matrix.append(['0000'] + [''] * (w - 1) + ['0000'])
                else:
                    matrix.append(['0000'] * w)
            else:
                row = raw_text.replace('\n', '').split(' ')

                # Проверяем длину строки. Если короткая -- добиваем до нужной длины пустыми ячейками и
                # ставим блок стены в конце. Иначе обрезаем и тоже устанавливаем блок стены в конце.
                if len(row) < w:
                    row += [''] * (w - len(row) - 1) + ['0000']
                elif len(row) >= w:
                    row = row[:w]
                    if not row[-1]:
                        row[-1] = '0000'

                # Расставляем стены по верхнему, нижнему и левому краю
                if r in (0, h - 1):
                    row = [i if i else '0000' for i in row]
                else:
                    if not row[0]:
                        row[0] = '0000'

                matrix.append(row)
    return matrix


def reco(cell: str) -> CellInfo:
    is_wall = not (cell.endswith('f') or not cell) # Пустая строка задает плитку пола по умолчанию
    is_spawn = '>' in cell
    return CellInfo(is_wall, is_spawn)


def conv(lvl: List[List[str]]) -> UnpackedLevel:
    floor_tiles = []
    walls = []
    player_x = 1
    player_y = 1
    for y, row in enumerate(lvl):
        for x, block in enumerate(row):
            info = reco(block)
            if info.wall_or_tile:
                walls.append(Wall(x, y, [i for i in block]))
            else:
                if info.is_player_spawn:
                    player_y = y
                    player_x = x
                    block = block.replace('>', '')
                floor_tiles.append(FloorTile(x, y, block))
    return UnpackedLevel(len(lvl), len(lvl[0]), floor_tiles, walls, player_x, player_y)


def read_res(filepath: str) -> dict:
    with open(filepath, 'r') as FILE:
        raw_res = FILE.read()
    lines = [i for i in raw_res.replace(': ', ':').split('\n') if i and not i.startswith('#')]
    for_return = dict()
    for l in lines:
        kvpair = l.split(':')
        for_return[kvpair[0]] = kvpair[1]
    return for_return


def output(lvl: UnpackedLevel, texture_dict: dict, filepath: str):
    stencil = {
        "header": {
            "spec_version": "0.3",
            "lvl_name": "",
            "height": 0,
            "width": 0,
            "player-x": 0,
            "player-y": 0,
            "light-brightness": 1,
            "light-decay-factor": 100,
            "shadow-depth-factor": 0.65,
            "textures": [],
            "sky_bg": ""
        },
        "walls": [],
        "floor": []
    }
    stencil["header"]["height"], stencil["header"]["width"] = lvl.h, lvl.w
    stencil["header"]["player-x"], stencil["header"]["player-y"] = lvl.player_x, lvl.player_y
    stencil["header"]["textures"] = list(texture_dict.values())

    # Записать стены и пол
    try:
        for w in lvl.walls:
            wall = {"x": w.x, "y": w.y, "color": "#FFFFFF"}
            for i, side in enumerate(("t_top", "t_right", "t_bottom", "t_left")):
                wall[side] = texture_dict[w.textures[i]]
            stencil["walls"].append(wall)

        for f in lvl.floor_tiles:
            tile = {'x': f.x, 'y': f.y, 't': texture_dict[f.texture]}
            stencil["floor"].append(tile)
    except KeyError as e:
        print("Неизвестный код текстуры в .csv-файле", "\n", e)
        quit()
    with open(filepath, 'w') as FILE:
        json.dump(stencil, FILE, indent=4)


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