#include <iostream>
#include <fstream>
#include <string>
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
        // Ejemplo de formato: timestamp,id,data0,..,data7 (timestamp esta en micro-segundos)
        auto now = std::chrono::steady_clock::now().time_since_epoch();
        auto us  = std::chrono::duration_cast<std::chrono::microseconds>(now).count();
        fifo << us << "," << line << '\n';
        fifo.flush(); // fuerza el envío inmediato
    }
}

