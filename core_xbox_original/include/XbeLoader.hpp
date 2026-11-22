#pragma once

#include "MemoryBus.hpp"
#include <string>
#include <vector>
#include <cstdint>

namespace Emulator::Core::XboxOriginal {

#pragma pack(push, 1)

struct XbeHeader {
    uint32_t magic;                 // "XBEH"
    uint8_t  signature[256];
    uint32_t baseAddress;
    uint32_t sizeOfHeaders;
    uint32_t sizeOfImage;
    uint32_t sizeOfImageHeader;
    uint32_t timeDateStamp;
    uint32_t certificateAddress;
    uint32_t numberOfSections;
    uint32_t sectionHeadersAddress;
    uint32_t initializationFlags;
    uint32_t entryPoint;
    // Additional fields ignored for basic PoC loading
};

struct XbeSectionHeader {
    uint32_t flags;
    uint32_t virtualAddress;
    uint32_t virtualSize;
    uint32_t rawAddress;
    uint32_t rawSize;
    uint32_t sectionNameAddress;
    uint32_t sectionNameRefCount;
    uint32_t headSharedPageRefCountAddress;
    uint32_t tailSharedPageRefCountAddress;
    uint8_t  digest[20];
};

#pragma pack(pop)

class XbeLoader {
public:
    XbeLoader(MemoryBus& memoryBus);
    ~XbeLoader() = default;

    bool LoadFromFile(const std::string& filePath);
    bool LoadMockXbe(); // For Boot / Smoke Test without real file

    uint32_t GetEntryPoint() const { return m_entryPoint; }
    uint32_t GetBaseAddress() const { return m_baseAddress; }

private:
    MemoryBus& m_memoryBus;
    uint32_t m_entryPoint = 0;
    uint32_t m_baseAddress = 0;

    bool ParseAndLoad(const std::vector<uint8_t>& fileData);
};

} // namespace Emulator::Core::XboxOriginal