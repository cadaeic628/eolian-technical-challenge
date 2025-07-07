/*  main.cpp – Prototipo consola "Eolian Data Acquisition"
 *  Autor: Sebastián Morales
 */

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <chrono>
#include <thread>

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



int main() {
	std::cout << "Hola Mundo!" << std::endl;
	return 0;
}
