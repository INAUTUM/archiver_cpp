#include "archiver.h"
#include "logger.h"
#include <iostream>
#include <csignal>

void signalHandler(int signal) {
    Logger::log(Logger::INFO, "Received signal " + std::to_string(signal));
    requestStop();
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);

    if (argc != 4) {
        Logger::log(Logger::ERROR, "Invalid arguments.");
        std::cerr << "Usage:\n"
                  << "myapp a <original file> <archive file>\n"
                  << "myapp e <archive file> <original file>\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string input = argv[2];
    std::string output = argv[3];

    bool success = false;
    if (mode == "a") {
        success = compressFile(input, output);
    } else if (mode == "e") {
        success = decompressFile(input, output);
    } else {
        Logger::log(Logger::ERROR, "Unknown mode: " + mode);
        return 1;
    }

    if (!success) {
        Logger::log(Logger::ERROR, "Operation failed.");
        return 1;
    }

    Logger::log(Logger::INFO, "Operation completed.");
    return 0;
}