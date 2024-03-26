//Реализуйте уникальный алгоритм сортировки 
// sleepsort с асимптотикой O(N) (по процессорному времени).

//На стандартный вход программы подается не более 100 строк
//  различной длины.Вам необходимо вывести эти строки, отсортированные 
// по длине.Строки одинаковой длины могут выводиться в произвольном порядке.

//Для каждой входной строки, создайте поток и передайте ей эту строку
//  в качестве параметра.Поток должна вызвать sleep(2) 
// или usleep(2) с параметром, пропорциональным длине этой
//  строки.Затем поток выводит строку в стандартный поток вывода и
//  завершается.Не следует выбирать коэффициент пропорциональности 
// слишком маленьким, вы рискуете получить некорректную сортировку.

// Данный код случайно генерирует строки длиной от 1 до 20, а затем сортирует строки по длине от меньшего к большему 

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <mutex>

using namespace std;

// Генератор случайных чисел
mt19937 rng(random_device{}()); //определяем генератор случайных чисел rng с использованием random_device

// Функция для генерации случайной строки заданной длины
string generate_random_string(int length) {
    uniform_int_distribution<int> distribution('a', 'z');
    string result;//храниться генерируемая случайная строка
    result.reserve(length);//Резервируется память для result в размере length символов, чтобы заранее выделить необходимое пространство и повысить эффективность.
    for (int i = 0; i < length; ++i) {
        result.push_back(static_cast<char>(distribution(rng)));
    }
    return result;
}

// Функция для безопасного вывода строки в стандартный поток вывода
void print(const string& str, mutex& mtx) {
    lock_guard<mutex> lock(mtx); // Захватываем мьютекс для безопасного доступа к cout
    cout << str << endl;
}// Пока поток не завершится, следующий поток будет ожидать завершения текущего

// Функция для сортировки строки по длине с использованием sleepsort
void sleepsort(const vector<string>& strings) {
    vector<thread> threads;
    mutex mtx; // Мьютекс для защиты вывода

    for (const auto& str : strings) {
        threads.emplace_back([&]() {//создает новый поток и добавляет его в вектор threads
            // Вычисляем время ожидания пропорционально длине строки
            this_thread::sleep_for(chrono::milliseconds(str.length() * 100));
            // Выводим строку в стандартный поток вывода с помощью безопасной функции print
            print(str, mtx);
            });
    }

    // Дожидаемся завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
}

int main() {
    // Генерация случайных строк
    vector<string> strings;
    int num_strings = 100; // Указанное количество строк
    int max_length = 20; // Максимальная длина строки


    for (int i = 0; i < num_strings; ++i) {
        int length = uniform_int_distribution<int>(1, max_length)(rng);
        strings.push_back(generate_random_string(length));
    }

    // Вызываем функцию сортировки
    sleepsort(strings);

    return 0;
}
//Создается объект distribution для генерации равномерно распределенных 
//случайных чисел между кодами букв 'a' и 'z'. Это позволит получать случайные строчные буквы английского алфавита.
//
//result.push_back(static_cast<char>(distribution(rng)));
//Генерируется случайное число функцией distribution(rng) в диапазоне между кодами букв 'a' и 'z'.
//Это число преобразуется в символ(char) с помощью static_cast.
//Сгенерированный символ добавляется в конец строки result с помощью метода push_back
//
// threads.emplace_back([&]() { - создает новый поток и добавляет его в вектор threads
//Тело нового потока определяется лямбда-выражением, которое захватывает все 
// переменные из контекста, в котором оно было создано.
//
//int length = uniform_int_distribution<int>(1, max_length)(rng);:
//Эта строка генерирует случайное целое число в диапазоне от 1 до max_length используя генератор случайных чисел rng.
//uniform_int_distribution<int>(1, max_length) :
// Это объект класса uniform_int_distribution, который используется для генерации равномерно распределенных случайных чисел в заданном диапазоне.
//
// sleepsort(strings); - Эта строка кода сортирует вектор strings по длине, используя алгоритм sleepsort.
