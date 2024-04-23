#include <iostream>
#include <list>
#include <shared_mutex>
#include <thread>

using namespace std;

class ListPer {
public:
    // Конструктор по умолчанию (пустой список)
    ListPer() {}

    // Параметризованный конструктор (инициализируется элементами)
    ListPer(const std::initializer_list<std::string>& elements) {
        for (auto element : elements) {
            add(element); // Используйте add() для обеспечения создания блокировки чтения-записи
        }
    }

    ListPer(const ListPer& other) {
        // Пройдитесь по списку источников и создайте глубокие копии с новыми блокировками чтения-записи
        for (const auto& entry : other.data_) {
            add(entry.string);
        }
    }

    // Потокобезопасная реализация пузырьковой сортировки (с использованием data_rwlock_)
    void sort() {
        bool swapped;
        do {
            swapped = false;
            auto it = data_.begin();
            auto next = std::next(it);
            for (; next != data_.end(); ++it, ++next) {
                std::unique_lock<std::shared_mutex> lock1(it->mutex, std::defer_lock);
                std::unique_lock<std::shared_mutex> lock2(next->mutex, std::defer_lock);
                std::lock(lock1, lock2);
                if (it->string > next->string) {
                    std::swap(it->string, next->string);
                    swapped = true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Задержка на 50 миллисекунд
            }
        } while (swapped);
    }


    // Потокобезопасный метод печати
    void print() {
        std::shared_lock<std::shared_mutex> lock(data_rwlock_);
        cout << "Текущее состояние списка:" << endl; // Добавляем эту строку перед выводом списка
        for (const auto& element : data_) {
            std::cout << element.string << std::endl;  // Выводим каждый элемент списка в отдельной строке
        }
    }

    // Потокобезопасный метод добавления нового элемента
    void add(std::string element) {
        std::unique_lock<std::shared_mutex> lock(data_rwlock_);  // Защищаем доступ к списку
        data_.push_back({ element }); // Добавляем новый элемент в конец списка
        sort(); // Вызываем сортировку после добавления нового элемента
    }

    struct SE {
        std::string string;
        mutable std::shared_mutex mutex; // Используем блокировку чтения-записи

        SE(const std::string& element) : string(element), mutex() {}
        SE(const SE& other) : string(other.string), mutex() {}
    };

    std::list<SE> data_;
    mutable std::shared_mutex data_rwlock_;
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
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Задержка на 50 миллисекунд
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    cout << "---------После ввода данных ожидайте от 4 до 5 секунд---------------" << endl;
    cout << "---------После каждой записанной строки будет временный запрет на ввод на 50 миллисекунд-----------" << endl;
    cout << "---------------------------Введите данные---------------------------" << endl;
    ListPer* list = new ListPer();

    thread listenToUserThread(userinput, list);

    listenToUserThread.join();

    delete list;
}
