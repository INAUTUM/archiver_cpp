#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <string>

bool compressFile(const std::string& inputPath, const std::string& outputPath);
bool decompressFile(const std::string& inputPath, const std::string& outputPath);
void requestStop();
bool isStopRequested();

#endif // ARCHIVER_H