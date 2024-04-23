#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

mutex mutex1;
mutex mutex2;

void threadFunction() {
    for (int i = 0; i < 10; ++i) {
        // Захватываем мьютекс mutex2 перед выводом строки дочернего потока
        mutex2.lock();
        cout << i + 1 << " cтрока текста дочернего потока" << endl;
        mutex2.unlock();
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    thread myThread(threadFunction);

    for (int i = 0; i < 10; ++i) {
        // Захватываем мьютекс mutex1 перед выводом строки родительского потока
        mutex1.lock();
        cout << i + 1 << " cтрока текста родительского потока" << endl;
        mutex1.unlock();
        this_thread::sleep_for(chrono::milliseconds(10));
    }

    myThread.join();

    return 0;
}
//----------------------------------Ответ--------------------------------------------------------
// В этом алгоритме есть проблема: после того, как родительский 
// поток разблокирует мьютекс mutex1 в шаге 3, дочерний 
// поток может не сразу его заблокировать в шаге 4.
//В этот промежуток времени другой поток(например, третий) может получить 
// доступ к мьютексу mutex1 и изменить состояние данных, 
// что приведет к некорректной работе программы.
//Для решения этой проблемы необходимо использовать 
// третье средство синхронизации, например, семафор