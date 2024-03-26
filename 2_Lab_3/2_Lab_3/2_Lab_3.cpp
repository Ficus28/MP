//Напишите программу, которая вычисляет число Пи при помощи ряда Лейбница:
//π = 4 * (1 - 1 / 3 + 1 / 5 - 1 / 7 + 1 / 9 - ...)
#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <iomanip>
#include <sstream>

void calculatePiPartialSum(int start, int end, double& partialSum) {
    partialSum = 0.0;
    for (int i = start; i < end; ++i) {
        double term = (i % 2 == 0) ? 1.0 / (2 * i + 1) : -1.0 / (2 * i + 1);
        partialSum += term;
    }
    //вычисляет частичную сумму ряда Лейбница в указанном диапазоне и сохраняет результат в переменной partialSum
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");//руссифкатор для консоли
    int numThreads = 10; // Установка значения количества потоков напрямую
    int numIterations = 1000000; // Установка значения количества итераций напрямую

    std::vector<std::thread> threads(numThreads);//
    std::vector<double> partialSums(numThreads);

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

    double pi = 0.0;//объявляет переменную pi типа double и инициализирует ее значением 0.0
    for (double partialSum : partialSums) {//цикл for, который перебирает все элементы вектора partialSums
        pi += partialSum;//происходит суммирование всех частичных сумм (partialSum), полученных в результате работы потоков.
    }
    pi *= 4.0;//умножает pi на 4.0, чтобы получить окончательное значение числа Пи.
    //ряд Лейбница сходится к 4 * Пи, поэтому необходимо умножить полученную сумму на 4, чтобы получить точное значение.
    std::cout << "Приближенное значение числа Пи: " << std::setprecision(15) << pi << std::endl;

    return 0;
}
//std::vector<std::thread> threads(numThreads);
//
//std::vector: Это класс - шаблон в C++ Standard Library, который представляет собой динамический массив.
//std::thread : Это класс, представляющий собой поток выполнения в C++.
//numThreads : Это переменная, которая содержит количество потоков, которые будут созданы.
//
//std::vector<double> partialSums(numThreads);
// 
//std::vector: Это класс - шаблон в C++ Standard Library, который представляет собой динамический массив.
//double : Это тип данных, который представляет собой число с плавающей запятой двойной точности.
//numThreads : Это переменная, которая содержит количество потоков, которые будут созданы.
