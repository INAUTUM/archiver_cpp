#include "archiver.h"
#include "logger.h"
#include "progress.h"

#include <zlib.h>
#include <fstream>
#include <iostream>
#include <csignal>
#include <atomic>

static std::atomic<bool> stopFlag(false);

void requestStop() {
    stopFlag = true;
}

bool isStopRequested() {
    return stopFlag;
}

bool compressFile(const std::string& inputPath, const std::string& outputPath) {
    // 1 мб
    const size_t bufferSize = 1 << 20;
    char* buffer = new char[bufferSize];

    std::ifstream input(inputPath, std::ios::binary);
    gzFile output = gzopen(outputPath.c_str(), "wb");

    if (!input.is_open() || !output) {
        Logger::log(Logger::ERROR, "Cannot open files for compression.");
        delete[] buffer;
        return false;
    }

    input.seekg(0, std::ios::end);
    size_t totalSize = input.tellg();
    input.seekg(0, std::ios::beg);
    size_t processed = 0;

    Logger::log(Logger::INFO, "Compressing " + inputPath + " -> " + outputPath);

    while (!input.eof() && !isStopRequested()) {
        input.read(buffer, bufferSize);
        std::streamsize bytesRead = input.gcount();
        if (gzwrite(output, buffer, bytesRead) != bytesRead) {
            Logger::log(Logger::ERROR, "gzwrite failed during compression.");
            break;
        }
        processed += bytesRead;
        showProgress(processed, totalSize);
    }

    std::cout << "\n";

    delete[] buffer;
    input.close();
    gzclose(output);

    if (isStopRequested()) {
        Logger::log(Logger::INFO, "Compression interrupted by signal.");
        return false;
    }

    Logger::log(Logger::INFO, "Compression completed successfully.");
    return true;
}

bool decompressFile(const std::string& inputPath, const std::string& outputPath) {
    const size_t bufferSize = 1 << 20;
    char* buffer = new char[bufferSize];

    gzFile input = gzopen(inputPath.c_str(), "rb");
    std::ofstream output(outputPath, std::ios::binary);

    if (!input || !output.is_open()) {
        Logger::log(Logger::ERROR, "Cannot open files for decompression.");
        delete[] buffer;
        return false;
    }

    std::ifstream sizeCalc(inputPath, std::ios::binary | std::ios::ate);
    size_t totalSize = sizeCalc.tellg();

    Logger::log(Logger::INFO, "Decompressing " + inputPath + " -> " + outputPath);

    size_t processed = 0;
    int bytesRead;

    while ((bytesRead = gzread(input, buffer, bufferSize)) > 0 && !isStopRequested()) {
        output.write(buffer, bytesRead);
        processed += bytesRead;
        showProgress(processed, totalSize);
    }

    std::cout << "\n";

    delete[] buffer;
    gzclose(input);
    output.close();

    if (isStopRequested()) {
        Logger::log(Logger::INFO, "Decompression interrupted by signal.");
        return false;
    }

    Logger::log(Logger::INFO, "Decompression completed successfully.");
    return true;
}