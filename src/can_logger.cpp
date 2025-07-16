#include "can_logger.hpp"
#include <iostream>
#include <iomanip>

CANLogger::CANLogger(const std::string& path) : log_(path, std::ios::out) {}

void CANLogger::start() { th_ = std::thread(&CANLogger::readerThread,this); }

void CANLogger::stop()  {
    { std::lock_guard lk(m_); running_ = false; }
    cv_.notify_all();
    th_.join();
}

bool CANLogger::pop(CANFrame& out) {
    std::unique_lock lk(m_);
    cv_.wait(lk,[&]{ return !q_.empty() || !running_; });
    if(q_.empty()) return false;
    out = q_.front(); q_.pop();
    return true;
}

void CANLogger::readerThread() {
    CANFrame f;
    while(running_) {
        if(!(std::cin >> std::hex >> f.id)) break;
        for(auto& b : f.data) std::cin >> std::hex >> b;
        f.ts = std::chrono::steady_clock::now();

        // log en el archivo can_log.txt
        log_ << std::chrono::duration_cast<std::chrono::microseconds>(
                 f.ts.time_since_epoch()).count()
             << ',' << std::hex << f.id;
        for(auto b : f.data) log_ << ',' << std::setw(2) << std::setfill('0') << int(b);
        log_ << '\n';

        // notificar a la UI
        {
            std::lock_guard lk(m_);
            q_.push(f);
        }
        cv_.notify_one();
    }
}
