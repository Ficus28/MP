//решить задачу через условные переменные и минимальное кол-во мьютексов
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

mutex mutex1; // Мьютекс для синхронизации доступа к разделяемым данным
condition_variable cv; // Условная переменная для сигнализации о готовности другого потока
bool ready = false; // Флаг, указывающий на готовность к выводу следующей строки

// Функция безопасного вывода для родительского потока
void safeOutputParent(const string& text) {
    unique_lock<mutex> lock(mutex1); // Блокировка мьютекса
    cout << text << endl;
    ready = true; // Помечаем, что поток завершил свою работу
    lock.unlock(); // Разблокировка мьютекса
    cv.notify_one(); // Сообщаем о готовности
    this_thread::sleep_for(chrono::milliseconds(10)); // Задержка
}

// Функция безопасного вывода для дочернего потока
void safeOutputChild(const string& text) {
    unique_lock<mutex> lock(mutex1); // Блокировка мьютекса
    cout << text << endl;
    ready = true; // Помечаем, что поток завершил свою работу
    lock.unlock(); // Разблокировка мьютекса
    cv.notify_one(); // Сообщаем о готовности
    this_thread::sleep_for(chrono::milliseconds(10)); // Задержка
}

void threadFunction() {
    for (int i = 0; i < 10; ++i) {
        safeOutputChild(to_string(i + 1) + " строка текста дочернего потока");
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    thread myThread(threadFunction); // Создание потока

    for (int i = 0; i < 10; ++i) {
        unique_lock<mutex> lock(mutex1); // Блокировка мьютекса
        cout << i + 1 << " строка текста родительского потока" << endl;
        while (!ready) { // Ждем, пока дочерний поток не завершит вывод строки
            cv.wait(lock); // Ожидание сигнала о готовности
        }
        ready = false; // Сбрасываем флаг готовности
        lock.unlock(); // Разблокировка мьютекса
        this_thread::sleep_for(chrono::milliseconds(10)); // Задержка
    }

    myThread.join(); // Ожидание завершения дочернего потока

    return 0;
}
