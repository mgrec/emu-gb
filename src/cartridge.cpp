#include "cartridge.h"
#include <filesystem>
#include <iostream>
#include <fstream>

uint8_t Cartridge::read(uint16_t address) const {
    if (address < rom.size()) {
        return rom[address];
    }
    return 0xFF;
}

bool Cartridge::load(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Could not open " << path << '\n';
        return false;   // failure: main will decide what to do
    }

    // Get the size, then resize the rom member to hold the whole file
    auto size = std::filesystem::file_size(path);
    rom.resize(size);

    // Fill rom with the file's bytes (same line as in main.cpp)
    file.read(reinterpret_cast<char*>(rom.data()), rom.size());

    // The header ends at 0x014F: a smaller file is not a valid ROM
    if (rom.size() < 0x150) {
        std::cerr << "ROM too small\n";
        return false;
    }

    // success
    return true;
}

std::size_t Cartridge::size() const {
    return rom.size();
}