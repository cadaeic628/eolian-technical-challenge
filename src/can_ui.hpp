#pragma once
#include "can_common.hpp"
#include <thread>

class CANUI {
public:
    explicit CANUI(class CANLogger& src);
    void start();
    void stop();
private:
    void uiThread();
    CANLogger& logger_;
    std::thread th_;
    bool running_ = true;
};
