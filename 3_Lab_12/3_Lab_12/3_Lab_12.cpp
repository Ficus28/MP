#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <random> // Для std::shuffle
#include <ctime>  // Для std::time

#define N 5

HANDLE forks[N];
HANDLE printMutex; // Мьютекс для безопасного вывода
int spaghettiCount = 10;
bool eatingDone = false; // Флаг, указывающий, завершили ли философы еду

void philosopher(LPVOID arg) {
    int id = *(int*)arg;
    int left_fork = id;
    int right_fork = (id + 1) % N;

    while (true) { // Бесконечный цикл, чтобы философы могли проверить оба условия
        Sleep(rand() % 1000);

        // Попытка захвата обеих вилок
        DWORD result;
        result = WaitForSingleObject(forks[left_fork], INFINITE);
        if (result == WAIT_OBJECT_0) {
            result = WaitForSingleObject(forks[right_fork], INFINITE);
            if (result == WAIT_OBJECT_0) {
                // Философ взял обе вилки и начинает есть
                if (eatingDone || spaghettiCount <= 0) { // Проверяем оба условия для выхода из цикла
                    ReleaseMutex(forks[left_fork]);
                    ReleaseMutex(forks[right_fork]);
                    break; // Выходим из цикла, если условия не выполняются
                }

                // Блокируем мьютекс перед выводом
                WaitForSingleObject(printMutex, INFINITE);

                printf("Философ %d взял вилки и начал есть\n", id);
                Sleep(rand() % 1000);
                printf("Философ %d закончил есть\n", id);
                --spaghettiCount;
                printf("Философ %d освободил вилки\n", id);

                // Освобождение мьютекса после вывода
                ReleaseMutex(printMutex);

                // Освобождение вилок
                ReleaseMutex(forks[left_fork]);
                ReleaseMutex(forks[right_fork]);
            }
            else {
                // Если захватить правую вилку не удалось, освобождаем левую
                ReleaseMutex(forks[left_fork]);
            }
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    HANDLE philosophers[N];
    int ids[N];

    // Создаем мьютекс для безопасного вывода
    printMutex = CreateMutex(NULL, FALSE, NULL);

    // Инициализация вилок
    for (int i = 0; i < N; ++i) {
        forks[i] = CreateMutex(NULL, FALSE, NULL);
    }

    // Создание потоков для философов
    for (int i = 0; i < N; ++i) {
        ids[i] = i;
    }

    // Перемешиваем идентификаторы философов
    std::shuffle(ids, ids + N, std::mt19937{ std::random_device{}() });

    // Запуск потоков для философов
    for (int i = 0; i < N; ++i) {
        philosophers[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)philosopher, &ids[i], 0, NULL);
    }

    // Ждем, пока философы едят
    Sleep(10000); // Допустим, философы едят 10 секунд

    // Устанавливаем флаг завершения еды
    eatingDone = true;

    // Ожидание завершения работы философов
    WaitForMultipleObjects(N, philosophers, TRUE, INFINITE);

    // Уничтожение вилок
    for (int i = 0; i < N; ++i) {
        CloseHandle(forks[i]);
    }

    // Уничтожение мьютекса
    CloseHandle(printMutex);

    return 0;
}
