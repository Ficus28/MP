#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <list>
#include <algorithm>
#include <chrono>

using namespace std;

// Глобальный список и мьютекс для синхронизации доступа к нему
list<string> globalList;
mutex listMutex;

// Функция для считывания ввода от пользователя и добавления в список
void readInput() {
    while (true) {
        string input;
        getline(cin, input);

        // Если введена пустая строка, вывести текущее состояние списка
        if (input.empty()) {
            listMutex.lock();
            cout << "Текущее состояние списка:" << endl;
            for (const auto& item : globalList) {
                cout << item << endl;
            }
            cout << endl;
            listMutex.unlock();
        }
        else {
            // Добавить строку в список
            listMutex.lock();
            globalList.push_front(input);
            listMutex.unlock();
        }
    }
}

// Функция для сортировки списка по длине строк каждые 5 секунд
void sortList() {
    while (true) {
        // Задержка на 5 секунд
        this_thread::sleep_for(chrono::seconds(5));

        // Сортировка списка пузырьковой сортировкой по длине строк
        listMutex.lock();
        bool sorted = false;
        while (!sorted) {
            sorted = true;
            for (auto it = globalList.begin(); it != prev(globalList.end()); ++it) {
                if (it->length() > next(it)->length()) {
                    iter_swap(it, next(it));
                    sorted = false;
                }
            }
        }
        listMutex.unlock();
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    cout << "------------------Введите данные:---------------" << endl;
    // Создание потоков для чтения ввода и сортировки списка
    thread inputThread(readInput);
    thread sortThread(sortList);

    // Ожидание завершения работы потоков
    inputThread.join();
    sortThread.join();

    return 0;
}
