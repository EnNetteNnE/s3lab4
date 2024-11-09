#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

using namespace std;

struct Date {
    int day;
    int month;
    int year;

    // Функция для проверки корректности даты
    bool isValid() {
        if (year < 1) return false;
        if (month < 1 || month > 12) return false;
        int daysInMonth[] = { 31, 28 + (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        return day >= 1 && day <= daysInMonth[month - 1];
    }

    // Оператор сравнения для сортировки и проверки диапазона
    bool operator<(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }

    // Метод для увеличения даты на один день
    void increment() {
        day++;
        if (day > 30) { // Упрощенная проверка на количество дней
            day = 1;
            month++;
        }
        if (month > 12) {
            month = 1;
            year++;
        }
    }

    // Метод для вывода даты
    void print() const {
        std::cout << day << "/" << month << "/" << year << std::endl;
    }
};

// Функция для вывода даты
void printDate(const Date& date) {
    std::cout << std::setw(2) << std::setfill('0') << date.day << "/"
              << std::setw(2) << std::setfill('0') << date.month << "/"
              << date.year << std::endl;
}

// Функция для увеличения даты на один день
void nextDay(Date& date) {
    date.day++;
    int daysInMonth[] = { 31, 28 + (date.year % 4 == 0 && (date.year % 100 != 0 || date.year % 400 == 0)), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (date.day > daysInMonth[date.month - 1]) {
        date.day = 1;
        date.month++;
        if (date.month > 12) {
            date.month = 1;
            date.year++;
        }
    }
}

// Функция для вывода всех дат в диапазоне(1 поток)
void DateRange(const Date& start, const Date& end) {
    Date current = start;

    while (current.year < end.year || 
           (current.year == end.year && (current.month < end.month || 
           (current.month == end.month && current.day <= end.day)))) {
        printDate(current);
        nextDay(current);
    }

}


// Функция для вывода всех дат в диапазоне(многопоток)
void generateDates(Date start, Date end, std::vector<Date>& dates, std::mutex& mtx) {
    while (start < end) {
        std::lock_guard<std::mutex> lock(mtx);
        dates.push_back(start);
        start.increment();
    }
}



int main() {
    Date start = {1, 1, 2023}; // Начальная дата
    Date end = {13, 1, 2023}; // Конечная дата

    const int numThreads = 4; // Количество потоков
    std::vector<Date> dates;
    std::mutex mtx;

    if (!start.isValid() || !end.isValid()) {
        std::cout << "Неверные даты." << std::endl;
        return 1;
    }

    if ((end.year < start.year) || 
        (end.year == start.year && (end.month < start.month || 
        (end.month == start.month && end.day < start.day)))) {
        std::cout << "Вторая дата должна быть позже первой." << std::endl;
        return 1;
    }

    // Однопоточное вычисление
    auto startSingle = chrono::high_resolution_clock::now(); // таймер для начала
    DateRange(start, end);
    auto endSingle = chrono::high_resolution_clock::now(); // таймер для конца
    chrono::duration<double> elapsedSingle = endSingle - startSingle; // находим разницу таймером в секундах

    std::vector<std::thread> threads;

    // Многопоточное вычисление
    auto startMulti = chrono::high_resolution_clock::now(); //таймер для начала
    // Определяем диапазон для каждого потока
    int totalDays = (end.year - start.year) * 365 + (end.month - start.month) * 30 + (end.day - start.day);
    int daysPerThread = totalDays / numThreads;

    for (int i = 0; i < numThreads; ++i) {
        Date threadStart = start;
        for (int j = 0; j < i * daysPerThread; ++j) {
            threadStart.increment();
        }
        
        Date threadEnd = threadStart;
        for (int j = 0; j < daysPerThread; ++j) {
            threadEnd.increment();
            //if (threadEnd > end) break; // Если достигли конца диапазона
        }

        threads.emplace_back(generateDates, threadStart, threadEnd, std::ref(dates), std::ref(mtx));
    }

    // Ожидание завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
    auto endMulti = chrono::high_resolution_clock::now();//таймер для конца
    chrono::duration<double> elapsedMulti = endMulti - startMulti; // находим разницу таймеров в секундах

    // Вывод результатов
    cout << "(однопоточно): за " << elapsedSingle.count() << " секунд\n";
    cout << "(многопоточно): за " << elapsedMulti.count() << " секунд\n";

    return 0;
}