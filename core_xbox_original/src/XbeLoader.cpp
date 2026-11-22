#include "XbeLoader.hpp"
#include "Logger.hpp"
#include <fstream>
#include <cstring>

namespace Emulator::Core::XboxOriginal {

XbeLoader::XbeLoader(MemoryBus& memoryBus) : m_memoryBus(memoryBus) {}

bool XbeLoader::LoadFromFile(const std::string& filePath) {
    XLOG_INFO("Attempting to load XBE file from: " + filePath);
    
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        XLOG_ERROR("Could not open XBE file: " + filePath);
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        XLOG_ERROR("Failed to read complete XBE file content.");
        return false;
    }

    return ParseAndLoad(buffer);
}

bool XbeLoader::ParseAndLoad(const std::vector<uint8_t>& fileData) {
    if (fileData.size() < sizeof(XbeHeader)) {
        XLOG_ERROR("File size too small to contain an XBE header.");
        return false;
    }

    const XbeHeader* header = reinterpret_cast<const XbeHeader*>(fileData.data());
    
    // Magic for "XBEH" is 0x48454258 in Little Endian
    if (header->magic != 0x48454258) {
        XLOG_ERROR("Invalid XBE magic signature.");
        return false;
    }

    m_baseAddress = header->baseAddress;
    m_entryPoint = header->entryPoint ^ 0x5B5A4321; // Standard Xbox entry point XOR key

    XLOG_INFO("Parsed XBE successfully: BaseAddress=" + std::to_string(m_baseAddress) + 
              ", EntryPoint=" + std::to_string(m_entryPoint) + 
              ", Sections=" + std::to_string(header->numberOfSections));

    // For each section, parse header and write to MemoryBus
    uint32_t secHeaderOffset = header->sectionHeadersAddress - header->baseAddress;
    if (secHeaderOffset + header->numberOfSections * sizeof(XbeSectionHeader) > fileData.size()) {
        XLOG_ERROR("Section headers address is out of range.");
        return false;
    }

    for (uint32_t i = 0; i < header->numberOfSections; ++i) {
        const XbeSectionHeader* sec = reinterpret_cast<const XbeSectionHeader*>(
            fileData.data() + secHeaderOffset + (i * sizeof(XbeSectionHeader))
        );

        if (sec->rawAddress + sec->rawSize > fileData.size()) {
            XLOG_ERROR("Section raw data is out of range.");
            continue;
        }

        XLOG_INFO("Loading section " + std::to_string(i) + 
                  ": VirtualAddress=" + std::to_string(sec->virtualAddress) + 
                  ", VirtualSize=" + std::to_string(sec->virtualSize));

        // Write raw data to virtual memory
        m_memoryBus.WriteBytes(sec->virtualAddress, fileData.data() + sec->rawAddress, sec->rawSize);
    }

    return true;
}

bool XbeLoader::LoadMockXbe() {
    XLOG_INFO("Loading high-efficiency Mock XBE into memory for Boot/Smoke Testing...");
    
    // We will simulate a small diagnostic loop in memory.
    // Assembly instructions:
    // MOV EAX, 0x1000
    // ADD EAX, 0x2000
    // INC EBX
    // JMP to start of mock code (Looping frame action)
    
    // Byte sequence for:
    // 0x00010000: B8 00 10 00 00 (mov eax, 0x1000)
    // 0x00010005: 05 00 20 00 00 (add eax, 0x2000)
    // 0x0001000A: 43             (inc ebx)
    // 0x0001000B: EB F3          (jmp -13, back to mov eax)
    uint8_t mockProgram[] = {
        0xB8, 0x00, 0x10, 0x00, 0x00,
        0x05, 0x00, 0x20, 0x00, 0x00,
        0x43,
        0xEB, 0xF3
    };

    m_baseAddress = 0x00010000;
    m_entryPoint = m_baseAddress;

    m_memoryBus.WriteBytes(m_baseAddress, mockProgram, sizeof(mockProgram));
    XLOG_INFO("Mock XBE loaded at " + std::to_string(m_baseAddress) + " (EntryPoint=" + std::to_string(m_entryPoint) + ").");
    return true;
}

} // namespace Emulator::Core::XboxOriginal