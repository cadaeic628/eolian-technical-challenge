/*  main.cpp – Prototipo "Eolian CAN System Logger & UI"
 *  Autor: Sebastián Morales
 */

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <chrono>
#include <thread>
#include <array>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <iomanip>
#include <fstream>

// Modelos
struct CANFrame {
	uint32_t id;
	uint8_t data[8];	
};


// Simulador CAN
class CANSimulator {
public:
	// Devuelve true si se leyó un frame desde stdin o se generó aleatoriamente
	bool getNext(CANFrame& out) {
		std::string line;
		if (std::getline(std::cin, line) && !line.empty()) {
			std::cout << "line readed!" << std::endl;
			// Formato esperado: id dato0 dato1 ... dato7 (hexadecimal)
		    	std::istringstream iss(line);
		    	iss >> std::hex >> out.id;
		    	for (int i = 0; i < 8; ++i) iss >> std::hex >> out.data[i];
		    	return true;
		}
		// Si no hay stdin -> generar aleatorio
		out.id = (rand() & 0x7FF);
		for (auto& b : out.data) b = rand()  & 0xFF;
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		return true;
	}

};


// Logger
class Logger {
std::ofstream file;
public:
	explicit Logger(const std::string& path) : file(path, std::ios::app) {
		if (!file) {
        		std::cerr << "ERROR: no se pudo abrir " << path << '\n';
   		}
	}
	void log(const CANFrame& f) {
		auto t = std::time(nullptr);
		file << std::put_time(std::localtime(&t), "%F %T") << " "
			<< std::hex << std::setw(3) << f.id << " ";
		for (auto b : f.data) file << std::setw(2) << std::setfill('0') << int(b) << " ";
		file << "\n";
		file.flush();
	}
};


// Data Engine
class DataEngine {
	struct VarState { uint64_t timestampMs; std::array<uint8_t,8> data; };
	std::unordered_map<uint32_t, VarState> latest;
	std::vector<std::function<void()>> observers;
public:
	void onFrame(const CANFrame& f) {
		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now(),time_since_epoch()).count();
		latest[f.id] = { now, std::to_array(f.data) };
		notify();
	}

	const auto& getState() const { return latest; }

	void subscribe(const std::function<void()>& cb) { observers.push_back(cb); }

private:
	void notify() { for (auto& fn : observers) fn(); }
};


int main() {

	// semilla para inicializar los procesos aleatorios
	srand(unsigned(std::time(nullptr)));

	CANSimulator sim;
	Logger log("src/can_log.txt");
	DataEngine engine;

	CANFrame frame{};
	while (sim.getNext(frame)) {
		engine.onFrame(frame);
		log.log(frame);
	}
	return 0;
}
