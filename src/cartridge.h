#pragma once
#include <cstdint>
#include <string>
#include <vector>

class Cartridge {

    public:
        bool load(const std::string& path);
        uint8_t read(uint16_t address) const;
        std::size_t size() const;

    private:
        std::vector<uint8_t> rom;

};