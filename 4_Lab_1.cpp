#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

using namespace std;

mutex mtx; // мьютекс для защиты вывода в консоль
condition_variable cv; // условная переменная для синхронизации потоков
int countA; // счетчик для деталей A
int countB; // счетчик для деталей B
int countC ; // счетчик для деталей C

// Функция для производства детали A
void produceA(int numA) {
    for (int i = 0; i < numA; ++i) {
        // Имитация производства детали A
        this_thread::sleep_for(chrono::seconds(1));

        // Увеличиваем счетчик деталей A
        unique_lock<mutex> lck(mtx);
        ++countA;
        cout << "Деталь A произведена. Прогресс: (" << countA << "/" << numA << ")" << endl;
        cv.notify_one(); // уведомляем о завершении производства детали A
    }
}

// Функция для производства детали B
void produceB(int numB) {
    for (int i = 0; i < numB; ++i) {
        // Имитация производства детали B
        this_thread::sleep_for(chrono::seconds(2));

        // Увеличиваем счетчик деталей B
        unique_lock<mutex> lck(mtx);
        ++countB;
        cout << "Деталь B произведена. Прогресс: (" << countB << "/" << numB << ")" << endl;
        cv.notify_one(); // уведомляем о завершении производства детали B
    }
}

// Функция для производства детали C
void produceC(int numC) {
    for (int i = 0; i < numC; ++i) {
        // Имитация производства детали C
        this_thread::sleep_for(chrono::seconds(3));

        // Увеличиваем счетчик деталей C
        unique_lock<mutex> lck(mtx);
        ++countC;
        cout << "Деталь C произведена. Прогресс: (" << countC << "/" << numC << ")" << endl;
        cv.notify_one(); // уведомляем о завершении производства детали C
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    int numA = 9; // количество деталей A
    int numB = 9; // количество деталей B
    int numC = 9; // количество деталей C

    thread tA, tB, tC; // потоки для производства деталей A, B и C

    // Запускаем производство каждой детали в отдельных потоках с указанным количеством
    tA = thread(produceA, numA);
    tB = thread(produceB, numB);
    tC = thread(produceC, numC);

    // Ждем завершения производства всех деталей
    {
        unique_lock<mutex> lck(mtx);
        cv.wait(lck, [&] { return countA >= numA && countB >= numB && countC >= numC; });
    }

    // Выводим сообщение о завершении производства всех деталей
    cout << "Все детали произведены. Сборка винтика завершена." << endl;

    // Дожидаемся завершения всех потоков
    tA.join();
    tB.join();
    tC.join();

    return 0;
}
