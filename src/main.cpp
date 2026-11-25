#include <iostream>    // std::cout, std::cerr
#include <fstream>     // std::ifstream
#include <filesystem>  // std::filesystem::file_size
#include <vector>      // std::vector
#include <cstdint>     // uint8_t
#include <string>      // std::string
#include <format>      // std::format

int main(int argc, char **argv) {
    // 1. Check that a ROM path was given on the command line
    if (argc < 2) {
        std::cerr << "Usage: emu-gb <rom>\n" << std::endl;
        return 1;
    }
    std::cout << "ROM : " << argv[1] << '\n' << std::endl;

    // 2. Open the ROM in binary mode (bytes are read as-is, even on Windows)
    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Could not open " << argv[1] << '\n' << std::endl;
        return 1;
    }

    // 3. Get the file size (only once we know the file exists)
    auto size = std::filesystem::file_size(argv[1]);
    std::cout << "ROM size: " << size << " octets\n" << std::endl;

    // 4. Load the whole file into memory: one uint8_t per byte of the ROM
    std::vector<uint8_t> rom(size);
    file.read(reinterpret_cast<char *>(rom.data()), rom.size());
    std::cout << "Read: " << file.gcount() << std::endl;

    // 5. The cartridge header lives at 0x0100-0x014F: make sure the ROM is big enough
    if (rom.size() < 0x150) {
        std::cerr << "ROM size too small!\n" << std::endl;
        return 1;
    }

    // 6. Title: ASCII bytes at 0x0134-0x0143, padded with 0x00 (stop at the first one)
    std::string title;
    for (int i = 0x0134; i <= 0x0143; i++) {
        if (rom[i] == 0) {
            break;
        }
        title += static_cast<char>(rom[i]);
    }
    std::cout << "Title: " << title << std::endl;

    // 7. Cartridge type at 0x0147: which memory controller (MBC) the cartridge uses
    uint8_t type = rom[0x0147];
    std::string name;
    switch (type) {
        case 0x00: name = "ROM ONLY"; break;
        case 0x01: name = "MBC1"; break;
        case 0x03: name = "MBC1+RAM+BATTERY"; break;
        case 0x11: name = "MBC3"; break;
        case 0x13: name = "MBC3+RAM+BATTERY"; break;
        case 0x19: name = "MBC5"; break;
        case 0x1B: name = "MBC5+RAM+BATTERY"; break;
        default:   name = "Unknown";
    }
    std::cout << std::format("Type: {:02X} ({})\n", type, name);

    uint32_t expected = 32768u << rom[0x0148];
    std::cout << "Header ROM size: " << expected << (expected == rom.size() ? " (OK)" : "(MISMATCH)") << std::endl;

    uint8_t x = 0;
    for (int i = 0x0134; i <= 0x014C; i++) {
        x = static_cast<uint8_t>(x - rom[i] - 1);
    }
    std::cout << std::format("Header checksum: {:02X} ({})\n", x, x == rom[0x014D] ? "OK" : "(MISMATCH)");
    
    return 0;
}
