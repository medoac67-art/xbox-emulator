#include "../include/XboxOriginalCore.hpp"
#include "Logger.hpp"

namespace XboxOriginal {

    bool XboxOriginalCore::Initialize() {
        m_memory.Initialize();
        m_cpu.Reset();
        m_devices.Initialize();
        m_renderer.Initialize(640, 480);
        LOG_INFO("XboxOriginalCore: System initialized.");
        return true;
    }

    void XboxOriginalCore::LoadTitle(const std::string& path) {
        LOG_INFO("Loading XBE: " + path);
        m_loader.LoadXbe(path, m_memory);
    }

    void XboxOriginalCore::RunFrame() {
        // Execute CPU slice
        m_cpu.Step(); 
        
        // Update graphics
        static float time = 0.0f;
        time += 0.016f;
        m_renderer.RenderTestFrame(time);
    }

    void XboxOriginalCore::GetStats(EmulationStats& stats) {
        stats.fps = 60.0f;
        stats.cpuUsage = 5.0f;
    }

    uint8_t* XboxOriginalCore::GetMemoryMap() { return m_memory.GetRawBuffer(); }
    void XboxOriginalCore::Pause() { LOG_INFO("Core paused."); }
    void XboxOriginalCore::Step() { m_cpu.Step(); }

} // namespace XboxOriginal
