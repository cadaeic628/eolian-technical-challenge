#pragma once
#include <array>
#include <chrono>
#include <cstdint>

struct CANFRame {
	uint32_t id;
	std::array<uint8_t,8> data;
	std::chrono::steady_clock::time_point ts;
};
