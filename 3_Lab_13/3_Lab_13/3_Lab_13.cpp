#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <mutex>

using namespace std;

// Структура для узла связного списка
struct Node {
    string data;
    Node* next;

    Node(const string& str) : data(str), next(nullptr) {}
};

// Глобальная переменная для указателя на голову списка
Node* head = nullptr;

// Мьютекс для защиты доступа к списку
mutex list_mutex;

// Функция для вставки нового узла в конец списка
void insertNode(const string& str) {
    Node* newNode = new Node(str); // Создаем новый узел

    list_mutex.lock(); // Захватываем мьютекс для безопасного доступа к списку

    if (!head) { // Если список пуст
        head = newNode; // Устанавливаем новый узел как голову списка
    }
    else {
        Node* temp = head;
        while (temp->next) {
            temp = temp->next; // Перемещаемся к последнему узлу
        }
        temp->next = newNode; // Добавляем новый узел в конец списка
    }

    list_mutex.unlock(); // Освобождаем мьютекс
}

// Функция для безопасного вывода списка
void printList() {
    list_mutex.lock(); // Захватываем мьютекс для безопасного доступа к списку

    Node* temp = head;
    while (temp) {
        cout << temp->data << endl; // Выводим данные узла
        temp = temp->next; // Переходим к следующему узлу
    }

    list_mutex.unlock(); // Освобождаем мьютекс
}

// Функция для генерации случайных чисел
mt19937 rng(random_device{}());

// Функция для генерации случайной строки заданной длины
string generate_random_string(int length) {
    uniform_int_distribution<int> distribution('a', 'z');
    string result;
    result.reserve(length);
    for (int i = 0; i < length; ++i) {
        result.push_back(static_cast<char>(distribution(rng)));
    }
    return result;
}

// Функция для сортировки строки по длине с использованием sleepsort
void sleepsort(const vector<string>& strings) {
    vector<thread> threads;

    for (const auto& str : strings) {
        threads.emplace_back([&]() {
            this_thread::sleep_for(chrono::milliseconds(str.length() * 100));
            insertNode(str); // Вставляем строку в список
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

int main() {
    vector<string> strings;
    int num_strings = 100;
    int max_length = 30;

    for (int i = 0; i < num_strings; ++i) {
        int length = uniform_int_distribution<int>(1, max_length)(rng);
        strings.push_back(generate_random_string(length));
    }

    sleepsort(strings); // Сортируем строки

    printList(); // Выводим отсортированный список

    return 0;
}
