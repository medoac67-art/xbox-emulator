#include "Win32Host.hpp"
#include "D3D11Backend.hpp"
#include "../../core_xbox_original/include/XboxOriginalCore.hpp"
#include "../../core_xbox_360/include/Xbox360Core.hpp"
#include "../../core/include/Logger.hpp"

int main() {
    XLOG_INFO("XMulti-Gen Emulator Host v1.0 - Starting...");
    
    auto host = std::make_shared<Emulator::Host::Win32Host>();
    host->CreateHostWindow(L"XMulti-Gen Emulator - Phase 4", 1280, 720);
    host->SetGraphicsBackend(std::make_shared<Emulator::Host::D3D11Backend>());
    
    // Defaulting to Xbox Original for Phase 2/3 testing
    host->AttachCore(std::make_shared<Emulator::Core::XboxOriginalCore>());
    host->RunLoop();
    return 0;
}