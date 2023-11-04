from dataclasses import dataclass
from typing import List


@dataclass(slots=True, frozen=True)
class FloorTile:
    """ Класс для хранения информации об одной плитке пола.
    x: X-координата плитки
    y: Y-координата плитки
    texture: символьный код текстуры """
    x: int
    y: int
    texture: str


@dataclass(slots=True, frozen=True)
class Wall:
    """ Класс для хранения информации об одном блоке стены.
    x: X-координата блока
    y: Y-координата блока
    textures: список символьных кодов текстур. Текстуры перечисляются в порядке TOP-RIGHT-BOTTOM-LEFT """
    x: int
    y: int
    textures: List[str]


@dataclass(slots=True, frozen=True)
class UnpackedLevel:
    """Хранит распарсенные данные уровня.
    h: высота уровня (в блоках)
    w: ширина уровня (в блоках)
    floor_tiles: список всех плиток пола
    walls: список всех стен
    player_x, player_y: начальные координаты игрока"""
    h: int
    w: int
    floor_tiles: List[FloorTile]
    walls: List[Wall]
    player_x: int
    player_y: int
