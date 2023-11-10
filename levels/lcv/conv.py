from typing import List
from dataclasses import dataclass
from lcv.UnpackedLevel import *

__all__ = ["conv"]


@dataclass(slots=True, frozen=True)
class __Cellinfo:
    """ Хранит информацию о ячейке:
    wall_or_tile: тип клетки (True для wall);
    is_player_spawn: начинает ли игрок игру с этой клетки"""
    wall_or_tile: bool  # True для wall
    is_player_spawn: bool


def __reco(cell: str) -> __Cellinfo:
    """ По символьному коду определяет, является ли данная ячейка плиткой пола или стеной, содержит ли игрока.
    :param cell: символьный код ячейки
    :return: экземпляр __CellInfo """
    is_wall = not (cell.endswith('f') or not cell)  # Пустая строка задает плитку пола по умолчанию
    is_spawn = '>' in cell
    return __Cellinfo(is_wall, is_spawn)


def conv(lvl: List[List[str]]) -> UnpackedLevel:
    """ Преобразует символьные коды CSV файла в массив данных о стенах, массив данных о поле, данные о начальной
     позиции игрока
    :param lvl: матрица клеток уровня
    :return: экземпляр датакласса UnpackedLevel """
    floor_tiles = []
    walls = []
    player_x = 1
    player_y = 1
    for y, row in enumerate(lvl):
        for x, block in enumerate(row):
            info = __reco(block)
            if info.wall_or_tile:
                walls.append(Wall(x, len(lvl) - y, [i for i in block]))
            else:
                if info.is_player_spawn:
                    player_y = len(lvl) - y
                    player_x = x
                    block = block.replace('>', '')
                floor_tiles.append(FloorTile(x, len(lvl) - y, block))
    return UnpackedLevel(len(lvl), len(lvl[0]), floor_tiles, walls, player_x, player_y)
