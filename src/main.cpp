#include <iostream>    // std::cout, std::cerr
#include <cstdint>     // uint8_t
#include <string>      // std::string
#include <format>      // std::format
#include "cartridge.h"

int main(int argc, char **argv) {
    // 1. Check that a ROM path was given on the command line
    if (argc < 2) {
        std::cerr << "Usage: emu-gb <rom>\n" << std::endl;
        return 1;
    }
    std::cout << "ROM : " << argv[1] << '\n' << std::endl;

    // 2. Load the ROM into the cartridge (open, read, check size)
    Cartridge cart;
    // creates the object, like new Cartridge()
    if (!cart.load(argv[1])) {
        // load() already printed the reason
        return 1;
    }
    std::cout << "ROM size: " << cart.size() << " octets\n";

    // 6. Title: ASCII bytes at 0x0134-0x0143, padded with 0x00 (stop at the first one)
    std::string title;
    for (int i = 0x0134; i <= 0x0143; i++) {
        if (cart.read(i) == 0) {
            break;
        }
        title += static_cast<char>(cart.read(i));
    }
    std::cout << "Title: " << title << std::endl;

    // 7. Cartridge type at 0x0147: which memory controller (MBC) the cartridge uses
    uint8_t type = cart.read(0x0147);
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

    uint32_t expected = 32768u << cart.read(0x0148);
    std::cout << "Header ROM size: " << expected << (expected == cart.size() ? " (OK)" : "(MISMATCH)") << std::endl;

    uint8_t x = 0;
    for (int i = 0x0134; i <= 0x014C; i++) {
        x = static_cast<uint8_t>(x - cart.read(i) - 1);
    }
    std::cout << std::format("Header checksum: {:02X} ({})\n", x, x == cart.read(0x014D) ? "OK" : "(MISMATCH)");
    
    return 0;
}
