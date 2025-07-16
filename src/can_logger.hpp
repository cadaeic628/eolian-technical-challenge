#pragma once
#include "can_common.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <fstream>

class CANLogger {
public:
	explicit CANLogger(const std::string& log_path);
	void start();
	void stop();
	bool pop(CANFrame& out);

private:
	void readerThread();
	std::thread th_;
	std::ofstream log_;
	std::queue<CANFrame> q_;
	std::mutex m_;
	std::condition_variable cv_;
	bool running_ = true;
};

