#include "archiver.h"
#include "logger.h"
#include "progress.h"
#include <archive.h>
#include <archive_entry.h>
#include <csignal>
#include <atomic>
#include <fstream>
#include <filesystem>
#include <iostream>

static std::atomic<bool> stopFlag(false);

void requestStop() { stopFlag = true; }
bool isStopRequested() { return stopFlag; }

// Общая функция для обработки архивов
bool processArchive(const std::string& inputPath, 
                   const std::string& outputPath,
                   bool compress) 
{
    const size_t bufferSize = 1 << 20;
    char* buffer = new char[bufferSize];
    struct archive* a = archive_write_new();
    struct archive* ext = archive_read_new();
    int status;

    try {
        // Форматы (расширения)
        if (compress) {
            // Автоматическое определение формата по расширению
            if (outputPath.ends_with(".zip")) {
                archive_write_set_format_zip(a);
            } else if (outputPath.ends_with(".tar.gz")) {
                archive_write_set_format_gnutar(a);
                archive_write_add_filter_gzip(a);
            } else if (outputPath.ends_with(".7z")) {
                archive_write_set_format_7zip(a);
            } else if (outputPath.ends_with(".gz")) {
                archive_write_set_format_raw(a);
                archive_write_add_filter_gzip(a);
            } else {
                archive_write_set_format_filter_by_ext(a, outputPath.c_str());
            }
            
            status = archive_write_open_filename(a, outputPath.c_str());
        } else {
            archive_read_support_format_all(ext);
            archive_read_support_filter_all(ext);
            status = archive_read_open_filename(ext, inputPath.c_str(), bufferSize);
        }

        if (status != ARCHIVE_OK) {
            throw std::runtime_error(archive_error_string(compress ? a : ext));
        }

        if (compress) {
            // Создание архива
            struct archive_entry* entry = archive_entry_new();
            std::filesystem::path p(inputPath);
            archive_entry_set_pathname(entry, p.filename().c_str());
            archive_entry_set_size(entry, std::filesystem::file_size(inputPath));
            archive_entry_set_filetype(entry, AE_IFREG);
            archive_write_header(a, entry);

            std::ifstream file(inputPath, std::ios::binary);
            size_t totalProcessed = 0;
            size_t totalSize = std::filesystem::file_size(inputPath);

            while (!file.eof() && !isStopRequested()) {
                file.read(buffer, bufferSize);
                size_t bytesRead = file.gcount();
                archive_write_data(a, buffer, bytesRead);
                totalProcessed += bytesRead;
                showProgress(totalProcessed, totalSize);
            }
            std::cout << std::endl;
            archive_entry_free(entry);
        } else {
            // Распаковка архива
            struct archive_entry* entry;
            int entry_count = 0; // Проверка на 1 файл
            
            while (archive_read_next_header(ext, &entry) == ARCHIVE_OK && !isStopRequested()) {
                entry_count++;
                if(entry_count > 1) {
                    throw std::runtime_error("Archive contains multiple files - this mode supports single-file archives only");
                }
                
                // Получаем путь к родительской директории
                auto outputDir = std::filesystem::path(outputPath).parent_path();
                
                // Создаем директории (только если они указаны в outputPath)
                if(!outputDir.empty()) {
                    std::filesystem::create_directories(outputDir);
                }
                
                Logger::log(Logger::INFO, "Decompressing file to: " + outputPath);
                std::ofstream file(outputPath, std::ios::binary);
                if (!file.is_open()) {
                    throw std::runtime_error("Cannot create output file: " + outputPath);
                }

                size_t totalSize = archive_entry_size(entry);
                size_t totalProcessed = 0;

                ssize_t bytesRead;
                while ((bytesRead = archive_read_data(ext, buffer, bufferSize)) > 0) {
                    file.write(buffer, bytesRead);
                    totalProcessed += bytesRead;
                    showProgress(totalProcessed, totalSize);
                }
                std::cout << std::endl;
            }
            
            if(entry_count == 0) {
                throw std::runtime_error("Archive is empty or corrupt");
            }
        }

        delete[] buffer;
        archive_write_close(a);
        archive_read_close(ext);
        archive_write_free(a);
        archive_read_free(ext);

        if (isStopRequested()) {
            Logger::log(Logger::INFO, "Operation interrupted by signal.");
            return false;
        }

        return true;

    } catch (const std::exception& e) {
        Logger::log(Logger::ERROR, e.what());
        delete[] buffer;
        archive_write_free(a);
        archive_read_free(ext);
        return false;
    }
}

bool compressFile(const std::string& inputPath, const std::string& outputPath) {
    Logger::log(Logger::INFO, "Compressing " + inputPath + " -> " + outputPath);
    return processArchive(inputPath, outputPath, true);
}

bool decompressFile(const std::string& inputPath, const std::string& outputPath) {
    Logger::log(Logger::INFO, "Decompressing " + inputPath + " -> " + outputPath);
    return processArchive(inputPath, outputPath, false);
}
