
//Этот код предназначен для асинхронного копирования репозитория из одного места в другое.
#include <iostream>
#include <fstream>
#include <vector>
#include <future>
#include <filesystem> // Используем стандартную файловую систему C++17
#include <Windows.h>

namespace fs = std::filesystem; // Используем файловую систему из пространства std::filesystem
using namespace std;

void copyFile(const string& src, const string& dst) { //Содержит ссылки на src и dst
    ifstream in(src, ios::binary); //Открывает исходный файл (src) для чтения в бинарном режиме
    ofstream out(dst, ios::binary); //Открывает целевой файл (dst) для записи в бинарном режиме
    out << in.rdbuf();
}
//in.rdbuf(): Возвращает буфер чтения из ifstream.
//out << : Записывает данные из буфера чтения в ofstream.

//Данная функция рекурсивно копирует весь каталог из одного места в другое
void copyDir(const string& src, const string& dst) {//Содержит ссылки на src и dst
    try {
        fs::create_directory(dst); //Создает целевую директорию(dst), если она не существует.

        std::vector<fs::path> files, directories;
        //files - вектор для хранения путей к файлам.
        //directories - вектор для хранения путей к подкаталогам.

        for (const auto& entry : fs::directory_iterator(src)) {
            if (entry.is_regular_file()) { // Проверка, является ли элемент файлом
                files.push_back(entry.path());//добавляет его путь в вектор files
            }
            else if (entry.is_directory()) { // Проверка, является ли элемент директорией (подкаталогом)
                directories.push_back(entry.path());//добавляет его путь в вектор directories
            }
        }

        std::vector<std::future<void>> fileFutures, dirFutures;

        // Запуск асинхронного копирования файлов
        for (const auto& file : files) {
            string newSrc = src + "/" + file.filename().string(); //Формирует полный путь к исходному файлу (newSrc). src - базовый каталог.
            string newDst = dst + "/" + file.filename().string(); //Формирует полный путь к целевому файлу (newDst). dst - базовый каталог назначения
            std::cout << "Copying file \nFrom: " << newSrc << "\nTo: " << newDst << std::endl;
            fileFutures.push_back(std::async(std::launch::async, copyFile, newSrc, newDst));
        }

        // Запуск асинхронного копирования директорий
        for (const auto& directory : directories) {
            string newSrc = src + "/" + directory.filename().string();//Формирует полный путь к исходному файлу (newSrc). src - базовый каталог.
            string newDst = dst + "/" + directory.filename().string();//Формирует полный путь к целевому файлу (newDst). dst - базовый каталог назначения
            std::cout << "Copying directory \nFrom: " << newSrc << "\nTo: " << newDst << std::endl;
            dirFutures.push_back(std::async(std::launch::async, copyDir, newSrc, newDst));
        }

        for (auto& future : fileFutures) {//Перебирает все будущие объекты (future) в векторе fileFutures.
            future.get();//Ждет завершения операции, связанной с future
        }
        for (auto& future : dirFutures) {//Перебирает все будущие объекты (future) в векторе dirFutures.
            future.get();//Ждет завершения операции, связанной с future
        }
    }
    catch (const std::exception& ex) {//Перехватывает исключения типа std::exception или его потомков.
        std::cerr << "Error: " << ex.what() << std::endl;//Выводит сообщение об ошибке на консоль стандартного вывода ошибок.
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    std::string source_directory = "C:\\Users\\Пользователь\\source\\repos\\2_Lab_2";
    std::string destination_directory = "C:\\Users\\Пользователь\\source\\repos\\2_Lab_2_1";
    copyDir(source_directory, destination_directory);//Рекурсивно копирует все файлы и подкаталоги из source_directory в destination_directory
    cout << "--------------------Копирование репозитория прошло успешно-----------------------------------";
    return 0;
}
//fs::directory_iterator(src): Итератор по элементам каталога src.
//entry: Переменная, которая содержит информацию о текущем элементе.
//
//fileFutures: Вектор для хранения будущих объектов (std::future<void>) копирования файлов.
//dirFutures: Вектор для хранения будущих объектов(std::future<void>) копирования подкаталогов.
//
//for (const auto& file : files) {
//Перебирает каждый элемент (file) в векторе files.
//const auto& : Делает ссылку на file константной, чтобы не менять его случайно.
//
//fileFutures.push_back(std::async(std::launch::async, copyFile, newSrc, newDst));:
//Добавляет в вектор fileFutures информацию о будущей операции копирования.
//std::async(std::launch::async, copyFile, newSrc, newDst) :
//std::launch::async : Запускает копирование асинхронно в отдельном потоке.
//copyFile : Функция, которая выполняет копирование файла.
//newSrc, newDst : Аргументы функции copyFile.
//
//Добавляет в вектор dirFutures информацию о будущей операции копирования.
//std::async(std::launch::async, copyDir, newSrc, newDst) :
//std::launch::async : Запускает копирование асинхронно в отдельном потоке.
//copyDir : Функция, которая выполняет рекурсивное копирование подкаталога(включая вложенные файлы и подкаталоги).
//newSrc, newDst : Аргументы функции copyDir.
