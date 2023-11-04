from lcv.UnpackedLevel import UnpackedLevel
import json


def output(lvl: UnpackedLevel, texture_dict: dict, filepath: str):
    """Записывает в файл "рыбу" уровня, заполняет поля walls, floor, height, width, player-x, player-y, textures,
    остальные оставляет пустыми
    :param lvl: экземпляр UnpackedLevel с распарсенными данными об уровне
    :param texture_dict: словарь кодов текстур, где каждому коду соответствует имя файла текстуры в папке
    assets/textures
    :param filepath: имя файла для записи
    """
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

    # Сериализовать стены и пол
    try:
        for w in lvl.walls:
            wall = {"x": w.x, "y": w.y, "color": "#ffffff"}
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