#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <iostream>
#include <string>
#include <mutex>

class Logger {
public:
    enum Level { INFO, ERROR };

    static void log(Level level, const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::ofstream logFile("myapp.log", std::ios::app);
        std::string prefix = "[" + levelToString(level) + "] ";

        // вывод в консоль
        std::cerr << prefix << message << "\n";         
        if (logFile.is_open()) {
            // вывод в файл
            logFile << prefix << message << "\n";
        }
    }

private:
    static std::string levelToString(Level level) {
        switch (level) {
            case INFO: return "INFO";
            case ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }

    static std::mutex mutex_;
};

#endif // LOGGER_H