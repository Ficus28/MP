#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <Windows.h>

using namespace std;

// Функция, которая будет выполнена в каждом потоке
void printSequence(const vector<string>& sequence, mutex& mtx) {
    // Захватываем блокировку перед выводом
    lock_guard<mutex> lock(mtx);
    for (const string& str : sequence) {
        cout << str << endl;
    }
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // Последовательности строк для каждого потока
    vector<vector<string>> sequences = {
        {"Поток 1, строка 1", "Поток 1, строка 2", "Поток 1, строка 3"},
        {"Поток 2, строка 1", "Поток 2, строка 2", "Поток 2, строка 3"},
        {"Поток 3, строка 1", "Поток 3, строка 2", "Поток 3, строка 3"},
        {"Поток 4, строка 1", "Поток 4, строка 2", "Поток 4, строка 3"}
    };

    // Вектор для хранения потоков
    vector<thread> threads;

    // Создание блокировки для синхронизации доступа к cout
    mutex mtx;

    // Создание и запуск каждого потока
    for (const auto& sequence : sequences) {
        threads.emplace_back(printSequence, ref(sequence), ref(mtx));
    }

    // Дожидаемся завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
