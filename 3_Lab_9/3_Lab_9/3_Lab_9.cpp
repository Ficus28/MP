﻿#include <iostream>
#include <list>
#include <mutex>
#include <thread>

using namespace std;

class ListPer {
public:
    // Конструктор по умолчанию (пустой список)
    ListPer() {}

    // Параметризованный конструктор (инициализируется элементами)
    ListPer(const std::initializer_list<std::string>& elements) {
        for (auto element : elements) {
            add(element); // Используйте add() для обеспечения создания мьютекса
        }
    }

    ListPer(const ListPer& other) {
        // Пройдитесь по списку источников и создайте глубокие копии с новыми мьютексами
        for (const auto& entry : other.data_) {
            data_.push_back({ entry.string });
        }
    }

    // Потокобезопасная реализация пузырьковой сортировки (с использованием data_mutex_)
    void sort() {
        bool swapped;
        do {
            swapped = false;
            auto it = data_.begin();
            auto next = std::next(it);
            for (; next != data_.end(); ++it, ++next) {
                it->mutex.lock();
                next->mutex.lock();
                if (it->string > next->string) { // Изменили условие сравнения
                    // Меняйте местами строки, удерживая блокировку
                    std::swap(it->string, next->string);
                    swapped = true;
                }
                it->mutex.unlock();
                next->mutex.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Задержка на одну секунду
            }
        } while (swapped);
    }



    // Потокобезопасный получатель размера (позволяет избежать ненужной блокировки)
    size_t size() {
        std::lock_guard<std::mutex> lock(data_mutex_);
        return data_.size();
    }
    // Потокобезопасный метод печати
    void print() {
        std::lock_guard<std::mutex> lock(data_mutex_);
        cout << "Текущее состояние списка:" << endl; // Добавляем эту строку перед выводом списка
        for (const auto& element : data_) {
            std::cout << element.string << std::endl;  // Выводим каждый элемент списка в отдельной строке
        }
    }


    // Потокобезопасный метод добавления нового элемента
    void add(std::string element) {
        std::lock_guard<std::mutex> lock(data_mutex_);  // Защищаем доступ к списку
        data_.push_back({ element }); // Добавляем новый элемент в конец списка
        sort(); // Вызываем сортировку после добавления нового элемента
    }



    struct SE {
        std::string string;
        std::mutex mutex;

        SE(const std::string& element) : string(element), mutex(std::mutex{}) {}
        SE(const SE& other) : string(other.string), mutex(std::mutex{}) {}
    };

    std::list<SE> data_;
    std::mutex data_mutex_;
};

void userinput(ListPer* list) {
    string line;
    for (int i = 0; i < 20; i++) {
        getline(cin, line);
        if (line.empty()) {
            list->print();
            continue;
        }
        else {
            list->add(line);
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    cout << "---------После ввода данных ожидайте от 4 до 5 секунд---------------" << endl;
    cout << "---------После каждой записанной строки будет времменый запрет на ввод на 50 мсек-----------" << endl;
    cout << "---------------------------Введите данные---------------------------" << endl;
    ListPer* list = new ListPer();

    thread listenToUserThread(userinput, list);

    listenToUserThread.join();

    delete list;
}
