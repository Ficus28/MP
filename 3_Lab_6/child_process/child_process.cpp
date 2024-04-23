#include <iostream>
#include <Windows.h>

using namespace std;

int main() {
    // Открываем именованный семафор, чтобы родительский процесс мог начать
    HANDLE semaphore = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, L"MyNamedSemaphore");
    if (semaphore == nullptr) {
        cerr << "Ошибка при открытии семафора: " << GetLastError() << endl;
        return 1;
    }

    // Цикл для вывода строк в дочернем процессе
    for (int i = 0; i < 10; ++i) {
        // Ожидаем, пока родительский процесс выведет свою строку
        WaitForSingleObject(semaphore, INFINITE);

        // Выводим строку дочернего процесса
        wcout << i + 1 << L" строка текста дочернего процесса" << endl;

        // Сигнализируем родительскому процессу, что можно вывести следующую строку
        ReleaseSemaphore(semaphore, 1, nullptr);
    }

    // Закрываем дескриптор семафора
    CloseHandle(semaphore);

    return 0;
}
