#include "Win32Host.hpp"
#include "../../core/include/Logger.hpp"
#include <windows.h>
#include <dwmapi.h>

namespace Emulator::Host {

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

Win32Host::Win32Host() = default;
Win32Host::~Win32Host() { if (m_hwnd) DestroyWindow(m_hwnd); }

bool Win32Host::CreateHostWindow(const std::wstring& title, int w, int h) {
    WNDCLASSEX wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"XEmuHostClass";
    RegisterClassEx(&wc);
    m_hwnd = CreateWindow(wc.lpszClassName, title.c_str(), WS_OVERLAPPEDWINDOW, 0, 0, w, h, nullptr, nullptr, wc.hInstance, nullptr);
    if (m_hwnd) {
        ShowWindow(m_hwnd, SW_SHOW);
        return true;
    }
    return false;
}

void Win32Host::AttachCore(std::shared_ptr<Emulator::Core::IEmulationCore> core) {
    m_core = core;
}

void Win32Host::SetGraphicsBackend(std::shared_ptr<IGraphicsBackend> backend) {
    m_backend = backend;
    if (m_backend && m_hwnd) {
        m_backend->Initialize((void*)m_hwnd);
    }
}

void Win32Host::RunLoop() {
    if (!m_core || !m_core->Initialize()) {
        XLOG_ERROR("Failed to initialize core.");
        return;
    }
    m_core->LoadTitle("test.xbe");

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        m_pacer.SleepToNextFrame();
        m_core->RunFrame();
        if (m_backend) m_backend->PresentFrame();
    }
}

} // namespace Emulator::Host