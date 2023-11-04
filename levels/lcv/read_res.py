def read_res(filepath: str) -> dict:
    """ Читает файл ресурсов. Проверяет, существуют ли файлы текстур. Если хотя бы один не существует,
    выбрасывает FileNotFoundError
    :param filepath: путь к файлу
    :return: словарь, где каждому строковому коду соответствует путь к текстуре """
    with open(filepath, 'r') as FILE:
        raw_res = FILE.read()
    lines = [i for i in raw_res.replace(': ', ':').split('\n') if i and not i.startswith('#')]
    for_return = dict()
    for l in lines:
        kvpair = l.split(':')
        for_return[kvpair[0]] = kvpair[1]
    return for_return
