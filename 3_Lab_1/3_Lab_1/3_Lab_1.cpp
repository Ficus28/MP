// Сделать сихронизированный вывод строк
#include <iostream>
#include <thread>
#include <atomic>
#include <Windows.h>

using namespace std;

// Флаг для контроля очередности вывода строк
atomic<bool> isChildThreadReady(false);

// Функция, которая будет выполнена в потоке
void threadFunction() {
    for (int i = 0; i < 10; ++i) {
        // Ждем, пока родительский поток выведет свою строку
        while (!isChildThreadReady.load()) {
            this_thread::yield(); // Даем шанс другим потокам выполниться
        }

        // Выводим строку дочернего потока
        cout << i + 1 << " cтрока текста дочернего потока" << endl;

        // Сигнализируем родительскому потоку, что можно выводить следующую строку
        isChildThreadReady.store(false);

        // Задержка для синхронизации вывода
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    // Создание нового потока и передача ему функции threadFunction
    thread myThread(threadFunction);

    for (int i = 0; i < 10; ++i) {
        // Выводим строку родительского потока
        cout << i + 1 << " cтрока текста родительского потока" << endl;

        // Сигнализируем дочернему потоку, что можно выводить следующую строку
        isChildThreadReady.store(true);

        // Задержка для синхронизации вывода
        this_thread::sleep_for(chrono::milliseconds(10));

        // Ждем, пока дочерний поток выведет свою строку
        while (isChildThreadReady.load()) {
            this_thread::yield(); // Даем шанс другим потокам выполниться
        }
    }

    // Ожидание завершения выполнения потока
    myThread.join();

    return 0;
}
