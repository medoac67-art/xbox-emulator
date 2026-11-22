#pragma once
#include "../../core/include/IEmulationCore.hpp"
class XboxSeriesCore : public IEmulationCore {
public:
    void Initialize() override;
    void LoadTitle(const char* path) override;
    void RunFrame() override;
    void Pause() override;
    void Step() override;
    uint8_t* GetMemoryMap() override { return nullptr; }
    uint64_t ReadMemory(uint64_t addr) override { return 0; }
    void WriteMemory(uint64_t addr, uint64_t val) override {}
    EmulationStats GetStats() override { return {}; }
};
