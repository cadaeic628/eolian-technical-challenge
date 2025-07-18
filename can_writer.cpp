#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <sys/stat.h>

int main() {
    const char* fifo_path = "/tmp/frames.fifo";

    // Crea el FIFO si no existe
    if (mkfifo(fifo_path, 0666) && errno != EEXIST) {
        perror("mkfifo"); return 1;
    }

    // Abre en modo bloqueante para escritura
    std::ofstream fifo(fifo_path);
    if (!fifo) { std::cerr << "No pude abrir " << fifo_path << "\n"; return 1; }

    std::string line;
    while (std::getline(std::cin, line)) {
        // Ejemplo de formato: timestamp,id,data0,..,data7
        const auto now = std::chrono::system_clock::now();
	const std::time_t t = std::chrono::system_clock::to_time_t(now);
	
	std::tm tm;
	#if defined(_MSC_VER)
		localtime_s(&tm, &t);
	#else
		localtime_r(&t, &tm);
	#endif
	std::ostringstream ts;
	ts << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");

	fifo << ts.str() << ',' << line << '\n';

        fifo.flush(); // fuerza el envío inmediato
    }
}

