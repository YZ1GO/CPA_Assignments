#pragma once

#include <chrono>
#include <string>

class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time;

public:
    void start();
    double stop(); // seconds
};