from typing import List

__all__ = ["read_lvl"]


def read_lvl(filepath: str, w: int, h: int) -> List[List[str]]:
    """
    Читает CSV-файл, дополняет/обрезает строки до заданной длины если необходимо, устанавливает внешние стены там, где
    граница уровня не ограждена, однако не меняет блоки стены, стоящие на границе уровня.
    :param filepath: путь к файлу .CSV
    :param w: ширина уровня в блоках
    :param h: высота уровня в блоках
    :return: Матрица строк m * n, где каждая ячейка содержит код блока/плитки
    """
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

                __compl_row(row, w)

                # Расставляем стены по верхнему, нижнему и левому краю
                if r in (0, h - 1):
                    row = [i if i else '0000' for i in row]
                else:
                    if not row[0]:
                        row[0] = '0000'

                matrix.append(row)
    return matrix


def __compl_row(row: List[str], w: int):
    """ Проверяем длину строки уровня. Если короткая -- добиваем до нужной длины пустыми ячейками и
    ставим блок стены в конце. Иначе обрезаем и тоже устанавливаем блок стены в конце, если он там отсутствует.
    :param row: ссылка на строку уровня
    :param w: ширина уровня ( в блоках)
    :return: Не возвращает ничего, изменяет значение по ссылке
    """
    if len(row) < w:
        row += [''] * (w - len(row) - 1) + ['0000']
    elif len(row) >= w:
        row = row[:w]
        if not row[-1]:
            row[-1] = '0000'
