#include <iostream>
#include <thread>
#include <chrono> // Для работы с временем
#include <Windows.h>

using namespace std;

// Функция, которая будет выполнена в дочернем потоке
void childThreadFunction() {
    // Печать текста в цикле
    for (int i = 0; i < 50; ++i) {
        cout << "Дочерний поток: " << i << endl;
        this_thread::sleep_for(chrono::milliseconds(100)); // Пауза в полсекунды
    }
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    // Создание и запуск дочернего потока
    thread childThread(childThreadFunction);

    // Ожидание 2 секунд
    this_thread::sleep_for(chrono::seconds(2));

    // Прерывание дочернего потока
    if (childThread.joinable()) {
        childThread.detach(); // Отсоединяем дочерний поток
        cout << "Дочерний поток был прерван Родительским потоком!" << endl;
    }

    return 0;
}
