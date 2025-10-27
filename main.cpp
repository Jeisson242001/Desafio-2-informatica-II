#include <iostream>
#include "Menu.h"
#include "MemoryTracker.h"
#include "Iteration.h"

int main() {
    std::cout << "== UdeA Tunes ==\n";
    std::cout << "Mem inicio: " << MemTrack::current() << " bytes\n";
    ITER_RESET();

    // Carga desde ./data (si faltan archivos, se cargan datos de demo)
    Menu menu;
    menu.loadOrSeed("data");
    menu.run();

    std::cout << "\n=== Resumen ===\n";
    std::cout << "Iteraciones: " << ITER_VALUE() << "\n";
    std::cout << "bytes aun en uso: " << MemTrack::current()
              << "  \nbytes solicitados en total: " << MemTrack::totalAllocated()
              << "  \nbytes liberados: " << MemTrack::totalFreed() << "\n";
    std::cout << "== Fin ==\n";
    return 0;
}
