/*
    Напишите, откомпилируйте и прогоните программу, распечатывающую 
    список файлов, входящих в каталог, с указанием их типов. Имя каталога
    задается как параметр командной строки. Если оно отсутствует, то 
    выбирается текущий каталог.
*/

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_PATH 1024 // Максимальная длина пути к файлу/каталогу

// Функция для определения типа файла (каталог, обычный файл и т.д.)
char get_file_type(mode_t mode) {
    if (S_ISDIR(mode))  return 'd';  // Каталог
    if (S_ISREG(mode))  return '-';  // Обычный файл
    if (S_ISCHR(mode))  return 'c';  // Символьный файл
    if (S_ISBLK(mode))  return 'b';  // Блочный файл
    if (S_ISFIFO(mode)) return 'p'; // Канал (FIFO)
    if (S_ISLNK(mode))  return 'l';  // Символьная ссылка

    return '?';                     // Неизвестный тип
}

int main(int argc, char *argv[])
{
    // Проверяем количество аргументов командной строки
    if (argc > 2) {
        fprintf(stderr, "Usage: %s [directory]\n", argv[0]);
        exit(EXIT_FAILURE);  // Если аргументов больше, чем нужно, завершение программы
    }

    // Если аргумент не передан, устанавливаем текущую директорию как путь
    char *pathname = (argc == 1) ? "." : argv[1];
    // Открываем указанную директорию
    DIR *directory = opendir(pathname);
    if (!directory) {
        perror("Can't open directory");  // Ошибка при открытии директории
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;     // Структура для хранения информации о файле/каталоге

    // Читаем содержимое каталога
    while ((entry = readdir(directory)) != NULL) {
        // Формируем полный путь к текущему элементу
        char full_path[MAX_PATH]; // Буфер для формирования полного пути к файлу
        snprintf(full_path, sizeof(full_path), "%s/%s", pathname, entry->d_name);

        struct stat stat_buffer;  // Буфер для хранения информации о файле/каталоге]
        // Получаем информацию о текущем элементе
        if (stat(full_path, &stat_buffer) != 0) {
            // Если произошла ошибка при вызове stat, выводим сообщение и завершаем программу
            perror("stat");
            exit(EXIT_FAILURE);
        }

        // Выводим тип файла/каталога
        printf("%c", get_file_type(stat_buffer.st_mode));
        // Выводим имя файла/каталога
        printf(" %s\n", entry->d_name);
    }

    // Закрываем каталог
    closedir(directory);
    return 0;
}
