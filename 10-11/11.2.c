/*
    Модифицируйте предыдущую программу (11-1.с) так, чтобы она 
    отображала файл, записанный этой программой, в память и считала 
    сумму квадратов чисел от 1 до 100000, которые уже находятся в этом 
    файле.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

// Определяем константу для количества структур в файле
#define N_STRTUCTS 100000

// Определение структуры A, содержащей два поля типа double
struct A 
{
    double f;   // Поле для хранения вещественного числа
    double f1;  // Поле для хранения другого вещественного числа
};

int main(void)
{
    // Определяем путь к файлу, который будем отображать в память
    char * pathname = "mapped.dat";
    
    // Открываем файл для чтения и записи
    int fd = open(pathname, O_RDWR);
    if (fd == -1) // Если ошибка при открытии
    {
        perror("open"); // Выводим сообщение об ошибке
        exit(EXIT_FAILURE); // Завершаем программу с ошибкой
    }
    
    // Проверяем информацию о файле, чтобы убедиться, что он имеет правильный размер
    struct stat stat_buffer; // Структура для хранения информации о файле
    if(fstat(fd, &stat_buffer) < 0) // Вызов fstat для получения данных о файле
    {
        perror("fstat"); // Выводим сообщение об ошибке
        close(fd);       // Закрываем файл перед выходом
        exit(EXIT_FAILURE); // Завершаем программу
    }

    // Рассчитываем ожидаемый размер файла
    size_t length = N_STRTUCTS * sizeof(struct A); // Количество структур умножаем на размер одной структуры
    if (stat_buffer.st_size != length) // Проверяем, совпадает ли реальный размер файла с ожидаемым
    {
        printf("Wrong file size. Expected %zd, got %zd\n", length, stat_buffer.st_size); // Выводим сообщение об ошибке
        close(fd); // Закрываем файл
        exit(EXIT_FAILURE); // Завершаем программу
    }
    
    // Отображаем файл в память
    struct A * ptr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) // Если отображение не удалось
    {
        perror("mmap"); // Выводим сообщение об ошибке
        close(fd);      // Закрываем файл
        exit(EXIT_FAILURE); // Завершаем программу
    }
    close(fd); // Закрываем файл, так как он больше не нужен после отображения в память

    // Создаем указатель на начало отображенного файла
    struct A * tmpptr = ptr;
    unsigned long long int sum = 0; // Переменная для накопления суммы значений поля f1
    for (int i = 1; i <= N_STRTUCTS; i++) // Проходим по всем структурам в файле
    {
        sum += tmpptr->f1; // Добавляем значение поля f1 текущей структуры к сумме
        tmpptr++; // Переходим к следующей структуре
    }

    // Освобождаем отображенную память
    munmap(ptr, length);

    // Выводим результат вычисления суммы
    printf("sum = %llu\n", sum);

    return 0; // Успешное завершение программы
}
