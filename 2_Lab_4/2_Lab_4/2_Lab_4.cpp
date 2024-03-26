//Доработанный код, мы прерываем программу когда нам захочеться
#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <csignal> // Для работы с сигналами
#include <atomic> // Для использования атомарных переменных

std::atomic<bool> interrupted(false); // Глобальная атомарная переменная-флаг для обработки сигнала

// Обработчик сигнала
void signalHandler(int signal) {//функция является обработчиком сигнала.Принимает один аргумент signal, который содержит номер полученного сигнала
    if (signal == SIGINT) {// проверяет, является ли полученный сигнал сигналом SIGINT.Сигнал SIGINT генерируется при нажатии клавиш Ctrl+C
        std::cout << "Программа получила сигнал SIGINT (Ctrl+C)." << std::endl;
        interrupted.store(true); // Установка флага при получении сигнала SIGINT
    }
}

void calculatePiPartialSum(int start, int end, std::atomic<double>& partialSum) {//Функция calculatePiPartialSum использует флаг interrupted для того, 
    //чтобы вовремя завершить работу при нажатии клавиш Ctrl+C
    partialSum = 0.0;//инициализирует атомарную переменную partialSum значением 0.0
    for (int i = start; i < end && !interrupted.load(); ++i) { // Добавляем частую проверку флага interrupted
        double term = (i % 2 == 0) ? 1.0 / (2 * i + 1) : -1.0 / (2 * i + 1);//строка вычисляет значение члена ряда Лейбница
        partialSum += term;//добавляет значение члена ряда к атомарной переменной

        // Добавляем частую проверку флага interrupted внутри цикла
        if (i % 1000000 == 0 && interrupted.load()) {
            break; // Если флаг установлен, выходим из цикла
        }
    }
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    // Установка обработчика сигнала
    signal(SIGINT, signalHandler);

    int numThreads = 4; // Установка значения количества потоков напрямую
    int numIterations = 9999999999; // Установка значения количества итераций напрямую

    std::vector<std::thread> threads(numThreads);
    std::vector<std::atomic<double>> partialSums(numThreads); // Вектор атомарных переменных для частичных сумм

    for (int i = 0; i < numThreads; ++i) {
        int start = i * (numIterations / numThreads);
        int end = (i == numThreads - 1) ? numIterations : (i + 1) * (numIterations / numThreads);
        threads[i] = std::thread(calculatePiPartialSum, start, end, std::ref(partialSums[i]));
        //этот цикл распределяет работу по вычислению Пи между несколькими потоками. 
        // Каждый поток получает свой диапазон итераций (start - end) и вычисляет свою частичную сумму ряда. 
        // Результат каждого потока сохраняется в соответствующем элементе вектора partialSums.
    }

    for (auto& thread : threads) {
        thread.join();
    }

    double pi = 0.0; // объявляет переменную pi типа double и инициализирует ее значением 0.0
    for (auto& partialSum : partialSums) {//цикл for, который перебирает все элементы вектора partialSum
        pi += partialSum.load(); // Суммируем значение атомарной переменной
    }
    pi *= 4.0; // умножает pi на 4.0, чтобы получить окончательное значение числа Пи.
    //ряд Лейбница сходится к 4 * Пи, поэтому необходимо умножить полученную сумму на 4, чтобы получить т

    std::cout << "Приближенное значение числа Пи: " << std::setprecision(15) << pi << std::endl;

    return 0;
}
