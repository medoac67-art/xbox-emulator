#include "D3D11Backend.hpp"
#include <comdef.h>
#include <windows.h>

namespace Emulator::Host {

bool D3D11Backend::Initialize(void* handle) {
    HRESULT hr = E_FAIL;
    D3D11_DEVICE_CONTEXT_TYPE contextType = D3D11_DEVICE_CONTEXT_TYPE_DEFERRED;
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
                            m_device.get() ? nullptr : &m_device, &m_device, &contextType);
    if (SUCCEEDED(hr)) {
        hr = m_device->GetImmediateContext(&m_context);
    }
    return SUCCEEDED(hr);
}

void D3D11Backend::Shutdown() {
    m_rtv.reset();
    m_srv.reset();
    m_frameTexture.reset();
    m_swapChain.reset();
    m_context.reset();
    m_device.reset();
}

bool D3D11Backend::UpdateFrameBuffer(const uint32_t* buffer, uint32_t width, uint32_t height) {
    m_width = width; m_height = height;
    // Simplified: In a real scenario, we would update a Staging Texture and CopyResource
    return true;
}

void D3D11Backend::PresentFrame() {
    if (m_swapChain) {
        m_swapChain->Present(0, 0);
    }
}

} // namespace Emulator::Host