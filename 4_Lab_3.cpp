#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <chrono>

class MessageQueue {
private:
    std::queue<std::string> queue;
    mutable std::mutex mutex;
    std::condition_variable cv;
    const size_t maxQueueSize = 20;
    bool isDropped = false;
    int numProducers = 0;
    int numConsumers = 0;

public:
    void put(const std::string& msg) {
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this]() { return queue.size() < maxQueueSize || isDropped; });
            ++numProducers;
        }

        if (!isDropped) {
            std::string truncatedMsg = msg.substr(0, 80); // Сократить до 80 символов
            {
                std::lock_guard<std::mutex> lock(mutex);
                queue.push(truncatedMsg);
                std::cout << "Отправленное сообщение: " << truncatedMsg << std::endl;
            }
            cv.notify_one();
        }
    }

    std::string get() {
        std::string msg;

        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this]() { return !queue.empty() || isDropped; });
            ++numConsumers;
            if (!isDropped) {
                msg = queue.front();
                queue.pop();
                std::cout << "Получение сообщения: " << msg << std::endl;
            }
        }

        if (numConsumers >= 2) {
            cv.notify_one();
            numConsumers = 0;
        }

        return msg;
    }

    void drop() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            isDropped = true;
        }
        cv.notify_all();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }
};

void producer(MessageQueue& mq, int id) {
    for (int i = 0; i < 15; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Задержка для имитации работы
        mq.put("Сообщение от производителя " + std::to_string(id) + ", номер " + std::to_string(i));
    }
}

void consumer(MessageQueue& mq, int id) {
    while (true) {
        std::string msg = mq.get();
        if (msg.empty()) break; // Завершить цикл, если очередь удалена
        std::this_thread::sleep_for(std::chrono::milliseconds(800)); // Задержка для имитации работы
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    MessageQueue mq;

    std::thread producer1(producer, std::ref(mq), 1);
    std::thread producer2(producer, std::ref(mq), 2);
    std::thread consumer1(consumer, std::ref(mq), 1);
    std::thread consumer2(consumer, std::ref(mq), 2);

    producer1.join();
    producer2.join();

    mq.drop(); // Удалить очередь после завершения работы производителей

    consumer1.join();
    consumer2.join();

    return 0;
}
