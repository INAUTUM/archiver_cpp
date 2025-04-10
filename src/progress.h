#ifndef PROGRESS_H
#define PROGRESS_H

#include <iostream>
#include <iomanip>

inline void showProgress(size_t current, size_t total) {
    if (total == 0) return;
    int barWidth = 50;
    float progress = static_cast<float>(current) / total;

    std::cout << "\r[";
    int pos = static_cast<int>(barWidth * progress);
    for (int i = 0; i < barWidth; ++i) {
        std::cout << (i < pos ? "#" : ".");
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100.0) << "%";
    std::cout.flush();
}

#endif // PROGRESS_H