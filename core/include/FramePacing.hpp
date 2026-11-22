#pragma once

#ifdef _WIN32
#include <windows.h>
#endif
#include <thread>
#include <chrono>

namespace Emulator::Core {

class FramePacer {
public:
    FramePacer(double targetFps) {
#ifdef _WIN32
        QueryPerformanceFrequency(&m_frequency);
        QueryPerformanceCounter(&m_lastTime);
#else
        m_lastTimeTimePoint = std::chrono::high_resolution_clock::now();
#endif
        SetTargetFps(targetFps);
    }

    void SetTargetFps(double targetFps) {
        m_targetFps = targetFps;
#ifdef _WIN32
        m_targetFrameTimeTicks = static_cast<LONGLONG>(m_frequency.QuadPart / targetFps);
#else
        m_targetFrameTimeDuration = std::chrono::duration<double>(1.0 / targetFps);
#endif
    }

    void Wait() {
#ifdef _WIN32
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);

        LONGLONG elapsedTicks = currentTime.QuadPart - m_lastTime.QuadPart;
        LONGLONG remainingTicks = m_targetFrameTimeTicks - elapsedTicks;

        if (remainingTicks > 0) {
            double remainingMilliseconds = (double(remainingTicks) * 1000.0) / m_frequency.QuadPart;
            if (remainingMilliseconds > 2.0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(remainingMilliseconds - 1.5)));
            }

            do {
                QueryPerformanceCounter(&currentTime);
                elapsedTicks = currentTime.QuadPart - m_lastTime.QuadPart;
            } while (elapsedTicks < m_targetFrameTimeTicks);
        }

        m_lastTime = currentTime;
#else
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = currentTime - m_lastTimeTimePoint;
        if (elapsed < m_targetFrameTimeDuration) {
            auto sleepDuration = m_targetFrameTimeDuration - elapsed;
            if (sleepDuration > std::chrono::milliseconds(2)) {
                std::this_thread::sleep_for(sleepDuration - std::chrono::microseconds(1500));
            }
            do {
                currentTime = std::chrono::high_resolution_clock::now();
            } while ((currentTime - m_lastTimeTimePoint) < m_targetFrameTimeDuration);
        }
        m_lastTimeTimePoint = currentTime;
#endif
    }

    double GetDeltaTimeSeconds() const {
        return 1.0 / m_targetFps;
    }

private:
#ifdef _WIN32
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_lastTime;
    LONGLONG m_targetFrameTimeTicks;
#else
    std::chrono::high_resolution_clock::time_point m_lastTimeTimePoint;
    std::chrono::duration<double> m_targetFrameTimeDuration;
#endif
    double m_targetFps;
};

} // namespace Emulator::Core
@@file path="host_win/include/IGraphicsBackend.hpp"
#pragma once

#ifdef _WIN32
#include <windows.h>
#else
using HWND = void*;
#endif

namespace Emulator::Host {

class IGraphicsBackend {
public:
    virtual ~IGraphicsBackend() = default;

    virtual bool Initialize(HWND hwnd, int width, int height) = 0;
    virtual void Shutdown() = 0;
    
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    
    virtual void Resize(int width, int height) = 0;
    virtual const char* GetBackendName() const noexcept = 0;
};

} // namespace Emulator::Host
@@file path="host_win/include/D3D11Backend.hpp"
#pragma once

#include "IGraphicsBackend.hpp"
#ifdef _WIN32
#include <d3d11.h>
#include <wrl/client.h>
#endif

namespace Emulator::Host {

#ifdef _WIN32
using Microsoft::WRL::ComPtr;
#endif

class D3D11Backend : public IGraphicsBackend {
public:
    D3D11Backend() = default;
    ~D3D11Backend() override = default;

    bool Initialize(HWND hwnd, int width, int height) override;
    void Shutdown() override;
    
    void BeginFrame() override;
    void EndFrame() override;
    
    void Resize(int width, int height) override;
    const char* GetBackendName() const noexcept override { return "Direct3D 11 (High Performance)"; }

#ifdef _WIN32
    ID3D11Device* GetDevice() const { return m_device.Get(); }
    ID3D11DeviceContext* GetContext() const { return m_context.Get(); }
#endif

private:
#ifdef _WIN32
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    ComPtr<IDXGISwapChain> m_swapChain;
    ComPtr<ID3D11RenderTargetView> m_renderTargetView;
#endif
    
    int m_width = 0;
    int m_height = 0;
};

} // namespace Emulator::Host
@@file path="host_win/src/D3D11Backend.cpp"
#include "D3D11Backend.hpp"
#include "Logger.hpp"

namespace Emulator::Host {

bool D3D11Backend::Initialize(HWND hwnd, int width, int height) {
    m_width = width;
    m_height = height;

#ifdef _WIN32
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 2; 
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1; 
    scd.SampleDesc.Quality = 0;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; 

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };
    D3D_FEATURE_LEVEL featureLevel;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        &scd,
        m_swapChain.GetAddressOf(),
        m_device.GetAddressOf(),
        &featureLevel,
        m_context.GetAddressOf()
    );

    if (FAILED(hr)) {
        XLOG_ERROR("Failed to create Direct3D 11 device/swapchain.");
        return false;
    }

    XLOG_INFO("Direct3D 11 backend initialized successfully.");
    Resize(width, height);
#else
    XLOG_INFO("Direct3D 11 backend mockup initialized (Platform non-Windows fallback).");
#endif
    return true;
}

void D3D11Backend::Resize(int width, int height) {
    if (width <= 0 || height <= 0) return;
    
    m_width = width;
    m_height = height;

#ifdef _WIN32
    if (!m_context) return;
    m_context->OMSetRenderTargets(0, nullptr, nullptr);
    m_renderTargetView.Reset();

    HRESULT hr = m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) {
        XLOG_ERROR("Direct3D 11 failed to resize buffers!");
        return;
    }

    ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
#endif
    XLOG_INFO("Graphics Backbuffer resized elegantly.");
}

void D3D11Backend::BeginFrame() {
#ifdef _WIN32
    if (!m_context || !m_renderTargetView) return;
    float clearColor[4] = { 0.05f, 0.05f, 0.05f, 1.0f }; 
    m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
    m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);

    D3D11_VIEWPORT vp = {};
    vp.Width = (float)m_width;
    vp.Height = (float)m_height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    m_context->RSSetViewports(1, &vp);
#endif
}

void D3D11Backend::EndFrame() {
#ifdef _WIN32
    if (m_swapChain) {
        m_swapChain->Present(1, 0);
    }
#endif
}

void D3D11Backend::Shutdown() {
#ifdef _WIN32
    m_renderTargetView.Reset();
    m_swapChain.Reset();
    m_context.Reset();
    m_device.Reset();
#endif
    XLOG_INFO("Direct3D 11 backend shutdown complete.");
}

} // namespace Emulator::Host
@@file path="host_win/include/Win32Host.hpp"
#pragma once

#include <memory>
#include "IGraphicsBackend.hpp"
#include "IEmulationCore.hpp"

namespace Emulator::Host {

class Win32Host {
public:
    Win32Host() = default;
    ~Win32Host();

    bool CreateHostWindow(const wchar_t* title, int width, int height);
    void AttachCore(std::shared_ptr<Emulator::Core::IEmulationCore> core);
    void RunLoop();

    HWND GetHWND() const { return m_hwnd; }

private:
#ifdef _WIN32
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
    void HandleInput();

    HWND m_hwnd = nullptr;
    std::unique_ptr<IGraphicsBackend> m_graphicsBackend;
    std::shared_ptr<Emulator::Core::IEmulationCore> m_core;
    bool m_isRunning = false;
    
    int m_width = 1280;
    int m_height = 720;
};

} // namespace Emulator::Host
@@file path="host_win/src/Win32Host.cpp"
#include "Win32Host.hpp"
#include "D3D11Backend.hpp"
#include "Logger.hpp"
#include "FramePacing.hpp"

#ifdef _WIN32
#include <xinput.h>
#pragma comment(lib, "xinput.lib")
#endif

namespace Emulator::Host {

Win32Host::~Win32Host() {
    if (m_graphicsBackend) {
        m_graphicsBackend->Shutdown();
    }
}

bool Win32Host::CreateHostWindow(const wchar_t* title, int width, int height) {
    m_width = width;
    m_height = height;
    
#ifdef _WIN32
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"XHostWindowClass";

    if (!RegisterClassExW(&wc)) {
        XLOG_ERROR("Failed to register Win32 Window Class.");
        return false;
    }

    RECT wr = { 0, 0, width, height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindowExW(
        0,
        L"XHostWindowClass",
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wr.right - wr.left,
        wr.bottom - wr.top,
        nullptr,
        nullptr,
        hInstance,
        this
    );

    if (!m_hwnd) {
        XLOG_ERROR("Failed to create Host Win32 Window.");
        return false;
    }

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
#else
    XLOG_INFO("Platform mockup: Simulated Win32 Host Window creation.");
#endif

    m_graphicsBackend = std::make_unique<D3D11Backend>();
    if (!m_graphicsBackend->Initialize(m_hwnd, width, height)) {
        XLOG_ERROR("Renderer initialization failed!");
        return false;
    }

    XLOG_INFO("Host Window & Graphics Layer up and running.");
    return true;
}

void Win32Host::AttachCore(std::shared_ptr<Emulator::Core::IEmulationCore> core) {
    m_core = core;
    if (m_core) {
        m_core->Initialize();
        XLOG_INFO("Emulation Core successfully linked.");
    }
}

#ifdef _WIN32
LRESULT CALLBACK Win32Host::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Win32Host* host = nullptr;
    if (msg == WM_NCCREATE) {
        CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
        host = reinterpret_cast<Win32Host*>(create->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(host));
    } else {
        host = reinterpret_cast<Win32Host*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (host) {
        switch (msg) {
            case WM_SIZE: {
                int w = LOWORD(lParam);
                int h = HIWORD(lParam);
                if (host->m_graphicsBackend) {
                    host->m_graphicsBackend->Resize(w, h);
                }
                return 0;
            }
            case WM_DESTROY:
                host->m_isRunning = false;
                PostQuitMessage(0);
                return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
#endif

void Win32Host::HandleInput() {
#ifdef _WIN32
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    for (DWORD i = 0; i < 4; ++i) {
        if (XInputGetState(i, &state) == ERROR_SUCCESS) {
            // Process Input Direct to current active Core
        }
    }
#endif
}

void Win32Host::RunLoop() {
    m_isRunning = true;
    
    Emulator::Core::FramePacer pacer(60.0);

#ifdef _WIN32
    MSG msg = {};
    while (m_isRunning) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            if (msg.message == WM_QUIT) {
                m_isRunning = false;
            }
        }

        if (!m_isRunning) break;

        HandleInput();

        if (m_core) {
            m_core->RunFrame();
        }

        m_graphicsBackend->BeginFrame();
        m_graphicsBackend->EndFrame();

        pacer.Wait();
    }
#else
    XLOG_INFO("Running simulated Emulation frame-loop (3 frames for testing)...");
    for (int i = 0; i < 3; ++i) {
        if (m_core) m_core->RunFrame();
        m_graphicsBackend->BeginFrame();
        m_graphicsBackend->EndFrame();
        pacer.Wait();
    }
#endif
}

} // namespace Emulator::Host
@@file path="host_win/src/main.cpp"
#include "Win32Host.hpp"
#include "IEmulationCore.hpp"
#include "Logger.hpp"

class MockXboxOriginalCore : public Emulator::Core::IEmulationCore {
public:
    bool Initialize() override {
        XLOG_INFO("Mock Xbox Original Core Initialized.");
        return true;
    }
    void Shutdown() override {}
    bool LoadTitle(const std::string& filePath) override {
        XLOG_INFO("Loading game disc image: " + filePath);
        return true;
    }
    void RunFrame() override {
        // High frequency frame tasks (no memory allocations allowed here!)
    }
    void Pause() override {}
    void Step() override {}
    void Reset() override {}
    std::vector<Emulator::Core::MemoryRegion> GetMemoryMap() const override { return {}; }
    bool ReadMemory(uint64_t, void*, size_t) override { return true; }
    bool WriteMemory(uint64_t, const void*, size_t) override { return true; }
    Emulator::Core::CoreStats GetStats() const override { return {}; }
    const char* GetCoreName() const noexcept override { return "Original Xbox (Chihiro)"; }
};

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#else
int main() {
#endif
    XLOG_INFO("Starting multi-generational Xbox Emulator Host...");

    auto host = std::make_unique<Emulator::Host::Win32Host>();
    if (!host->CreateHostWindow(L"XMulti-Gen Emulator - Phase 1 Core Host", 1280, 720)) {
        return -1;
    }

    auto dummyCore = std::make_shared<MockXboxOriginalCore>();
    host->AttachCore(dummyCore);

    host->RunLoop();

    return 0;
}