#include <iostream>
#include <thread>
#include <Windows.h>

using namespace std;

// Функция, которая будет выполнена в потоке
void threadFunction() {
    for (int i = 0; i < 10; ++i) {
        cout << i + 1 << " cтрока текста дочернего потока\n";
    }
}

int main() {
    // Установка кодировки для кириллицы
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // Создание нового потока и передача ему функции threadFunction
    thread myThread(threadFunction);

    // Ожидание завершения выполнения потока
    myThread.join();

    return 0;
}
