import os

PRIORITY_BINS = ['boot.bin', 'boot2.bin']

def err_handler(err):
    print(f'ошибка: {err}')
    print('========= СБОРКА СОПС ПРЕРВАНА =========')
    quit(1)

def formatsize(size: int) -> str:
    if size < 1000:
        return f'{size} Б'
    byte = ['Б', "кБ", "МБ", "ГБ", "ТБ"]
    i = 0
    while size >= 1000:
        size /= 1000
        i += 1
    return format(size, '.2g') + " " + byte[i]


print('========= СБОРКА СОПС НАЧАТА =========')
print('Получение всех файлов с кодом... ', end='')
tree = os.walk('./src', onerror=err_handler)
print('успех')
print('Сборка всех файлов...')
for i in tree:
    for file in i[2]:
        print(f'Сборка {file}... ', end='')
        srcpath = i[0]
        code = os.system(f'nasm -f bin {srcpath}/{file} -o ./build/bins/{file[:-4]}.bin')
        if code == 0:
            print('успех')
        else:
            err_handler(f'Не удалось собрать файл {file}')
print('Все исходники успешно собраны.')
print('Запись бинарников на дискету...')

disk = open("./build/sops.img", 'wb')
bins = os.listdir('./build/bins')
for b in PRIORITY_BINS:
    print(f'Запись {b}... ')
    try:
        bin = open('./build/bins/'+b, 'rb')
        for byte in bin:
            disk.write(byte)
        bin.close()
    except Exception as e:
        err_handler(e)
    bins.remove(b)
for b in bins:
    print(f'Запись {b}... ')
    try:
        bin = open('./build/bins/'+b, 'rb')
        for byte in bin:
            disk.write(byte)
        bin.close()
    except Exception as e:
        err_handler(e)
disk.close()
size = os.stat('./build/sops.img').st_size
print(f'Записано {formatsize(size)}. Дополнение до 1.44 МБ... ', end="")
remsize = 512 * 2880 - size
if remsize < 0:
    print('Внимание! Объём диска превысил 1.44 МБ!')
    print('========= СБОРКА СОПС ЗАВЕРШЕНА =========')
    quit()
disk = open('./build/sops.img', 'ab')
for i in range(remsize):
    disk.write(b'\x00')
disk.close()
print('успех')
print('========= СБОРКА СОПС ЗАВЕРШЕНА =========')

