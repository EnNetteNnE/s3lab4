#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>
#include <iomanip>
#include <string>
#include <chrono>
#include <random>

using namespace std;

struct Date {
    int day;
    int month;
    int year;

    void print() const {
        std::cout << day << "/" << month << "/" << year << std::endl;
    }
};

// для генерации случайной даты
Date randomDate() {
    Date date;
    date.year = rand() % 21 + 2000; 
    date.month = rand() % 12 + 1;   
    date.day = rand() % 31 + 1;      
    
    
    if (date.month == 2) {
        date.day = rand() % 28 + 1; 
    } else if (date.month == 4 || date.month == 6 || date.month == 9 || date.month == 11) {
        date.day = rand() % 30 + 1; 
    }

    return date;
}

// находится ли дата в заданном диапазоне
bool isDateInRange(const Date& date, const Date& start, const Date& end) {
    
    if (date.year < start.year || date.year > end.year) return false;
    if (date.year == start.year && (date.month < start.month || (date.month == start.month && date.day < start.day))) return false;
    if (date.year == end.year && (date.month > end.month || (date.month == end.month && date.day > end.day))) return false;
    return true;
}

// Функция для проверки, находится ли дата в заданном диапазоне
bool isDateInRange1(const Date& date, const Date& start, const Date& end) {
    if (date.year < start.year || date.year > end.year) return false;
    if (date.year == start.year && (date.month < start.month || (date.month == start.month && date.day < start.day))) return false;
    if (date.year == end.year && (date.month > end.month || (date.month == end.month && date.day > end.day))) return false;
    return true;
}

// Функция для обработки части массива дат
void filterDates(const std::vector<Date>& dates, const Date& startDate, const Date& endDate, std::vector<Date>& result, std::mutex& mtx) {
    for (const auto& date : dates) {
        if (isDateInRange(date, startDate, endDate)) {
            std::lock_guard<std::mutex> lock(mtx);
            result.push_back(date);
        }
    }
}


int main() {
    srand(static_cast<unsigned>(time(0))); // случайных чисел

    const int SIZE = 1000000; // сколько
    const int THREAD_COUNT = 4; // Количество потоков
    std::vector<Date> dates(SIZE);

    // Генерация
    for (int i = 0; i < SIZE; ++i) {
        dates[i] = randomDate();
    }

    

    // Задание диапазона
    Date startDate = {1, 1, 2010}; // Начало диапазона
    Date endDate = {31, 12, 2015}; // Конец диапазона

    // Выбор дат в заданном диапазоне
    std::cout << "Даты в диапазоне с " << startDate.day << "/" << startDate.month << "/" << startDate.year 
              << " по " << endDate.day << "/" << endDate.month << "/" << endDate.year << ":\n   ";

    
    // Многопоточное вычисление
    auto startMulti = chrono::high_resolution_clock::now(); //таймер для начала
    std::vector<Date> filteredDates;
    std::mutex mtx;

    // Создание потоков для фильтрации дат
    std::vector<std::thread> threads;
    int chunkSize = SIZE / THREAD_COUNT;

    for (int i = 0; i < THREAD_COUNT; ++i) {
        int startIdx = i * chunkSize;
        int endIdx = (i == THREAD_COUNT - 1) ? SIZE : startIdx + chunkSize; // Обработка последнего чанка

        threads.emplace_back(filterDates, std::ref(dates), startDate, endDate, std::ref(filteredDates), std::ref(mtx));
    }

    // Ожидание завершения всех потоков
    for (auto& th : threads) {
        th.join();
    }
    auto endMulti = chrono::high_resolution_clock::now();//таймер для конца
    chrono::duration<double> elapsedMulti = endMulti - startMulti; // находим разницу таймеров в секундах
    
    // Однопоточное вычисление
    auto startSingle = chrono::high_resolution_clock::now(); // таймер для начала
    for (const auto& date : dates) {
        if (isDateInRange(date, startDate, endDate)) {
            date.print();
        }
    }
    auto endSingle = chrono::high_resolution_clock::now(); // таймер для конца
    chrono::duration<double> elapsedSingle = endSingle - startSingle; // находим разницу таймером в секундах

    // Вывод результатов
    cout << "(однопоточно): за " << elapsedSingle.count() << " секунд\n";
    cout << "(многопоточно): за " << elapsedMulti.count() << " секунд\n";

    return 0;
}
