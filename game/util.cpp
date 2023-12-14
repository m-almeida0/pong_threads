#include"util.h"
#include<chrono>

double getCurrentTimeInSeconds() {
    auto currentTime = std::chrono::system_clock::now();
    auto timeInSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime.time_since_epoch());

    return timeInSeconds.count();
}