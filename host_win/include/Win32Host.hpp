#pragma once
#include "IGraphicsBackend.hpp"
#include "../../core/include/IEmulationCore.hpp"
#include "../../core/include/FramePacing.hpp"
#include <memory>
#include <string>

namespace Emulator::Host {

class Win32Host {
public:
    Win32Host();
    ~Win32Host();

    bool CreateHostWindow(const std::wstring& title, int width, int height);
    void AttachCore(std::shared_ptr<Emulator::Core::IEmulationCore> core);
    void RunLoop();
    void SetGraphicsBackend(std::shared_ptr<IGraphicsBackend> backend);

private:
    WNDCLASSEX m_wndClass{};
    HWND m_hwnd = nullptr;
    std::shared_ptr<Emulator::Core::IEmulationCore> m_core;
    std::shared_ptr<IGraphicsBackend> m_backend;
    Emulator::Core::FramePacer m_pacer;
};

} // namespace Emulator::Host