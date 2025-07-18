// can_writer.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <thread>
#include <random>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>

//util: timestamp legible
std::string now_str()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::tm tm;
#ifdef _MSC_VER
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream os;
    os << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
    return os.str();
}

//util: genera linea aleatoria id,b0..b7
std::string random_frame(std::mt19937& rng)
{
    std::uniform_int_distribution<int> idDist(0x000, 0x7FF);   // 11 bits
    std::uniform_int_distribution<int> byteDist(0, 255);

    std::ostringstream os;
    os << std::uppercase << std::hex << std::setw(3) << std::setfill('0')
       << idDist(rng);

    for (int i = 0; i < 8; ++i)
        os << ',' << std::setw(2) << std::setfill('0') << byteDist(rng);

    return os.str();   // ej. "123,4A,9F,..."
}

int main(int argc, char* argv[])
{
    const char* fifo_path = "/tmp/frames.fifo";

    // Crea FIFO si no existe
    if (mkfifo(fifo_path, 0666) && errno != EEXIST) {
        perror("mkfifo"); return 1;
    }

    // Abre FIFO (bloqueante) para escritura
    std::ofstream fifo(fifo_path);
    if (!fifo) {
        std::cerr << "No pude abrir " << fifo_path << "\n";
        return 1;
    }

    // MODO 1: generador aleatorio
    if (argc == 3 && std::strcmp(argv[1], "-r") == 0) {
        char* end;
        long ms = std::strtol(argv[2], &end, 10);
        if (*end || ms <= 0) {
            std::cerr << "Intervalo inválido: " << argv[2] << " (milisegundos)\n";
            return 1;
        }
        std::mt19937 rng(std::random_device{}());

        std::cout << "[can_writer] Generando frames cada "
                  << ms << " ms – Ctrl+C para salir\n";

        const auto period = std::chrono::milliseconds(ms);

        while (true) {
            std::string frame = random_frame(rng);
            fifo << now_str() << ',' << frame << '\n';
            fifo.flush();
            std::this_thread::sleep_for(period);
        }
    }

    // MODO 2: interactivo (stdin)
    if (argc != 1) {
        std::cerr << "Uso:\n"
                  << "  " << argv[0] << "            # modo interactivo\n"
                  << "  " << argv[0] << " -r N      # frames aleatorios cada N ms\n";
        return 1;
    }

    std::cout << "[can_writer] Escribe una línea por frame "
                 "(formato id,b0,..,b7) – Ctrl+D para salir\n";

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue; // ignora líneas vacías
        fifo << now_str() << ',' << line << '\n';
        fifo.flush();
    }
}

