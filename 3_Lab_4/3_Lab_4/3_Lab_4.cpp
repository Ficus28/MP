//Решить задачу с помощью семафоров-счётчиков
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <Windows.h>

using namespace std;

atomic<int> parentCounter(0);
atomic<int> childCounter(0);
atomic<bool> isParentTurn(true);

// Функция, которая будет выполнена в потоке
void threadFunction() {
    for (int i = 0; i < 10; ++i) {
        // Ждем, пока родительский поток выведет свою строку
        while (isParentTurn.load()) {
            this_thread::yield(); // Даем шанс другим потокам выполниться
        }

        // Выводим строку дочернего потока
        cout << i + 1 << " cтрока текста дочернего потока" << endl;

        // Увеличиваем счетчик дочернего потока
        ++childCounter;//семафор-счётчик

        // Устанавливаем флаг, что следующая строка должна вывести родительский поток
        isParentTurn.store(true);

        // Задержка для синхронизации вывода
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    // Создание нового потока и передача ему функции threadFunction
    thread myThread(threadFunction);

    for (int i = 0; i < 10; ++i) {
        // Ждем, пока дочерний поток выведет свою строку
        while (!isParentTurn.load()) {
            this_thread::yield(); // Даем шанс другим потокам выполниться
        }

        // Выводим строку родительского потока
        cout << i + 1 << " cтрока текста родительского потока" << endl;

        // Увеличиваем счетчик родительского потока
        ++parentCounter;////семафор-счётчик

        // Устанавливаем флаг, что следующая строка должна вывести дочерний поток
        isParentTurn.store(false);

        // Задержка для синхронизации вывода
        this_thread::sleep_for(chrono::milliseconds(10));
    }

    // Ожидание завершения выполнения потока
    myThread.join();

    return 0;
}
