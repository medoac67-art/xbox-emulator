#include "../include/Nv2aRenderer.hpp"
#include "Logger.hpp"
#include <cmath>

namespace XboxOriginal {

    Nv2aRenderer::Nv2aRenderer() : m_width(640), m_height(480) {}

    void Nv2aRenderer::Initialize(uint32_t width, uint32_t height) {
        m_width = width;
        m_height = height;
        m_frameBuffer.resize(m_width * m_height * 4, 0);
        LOG_INFO("NV2A Renderer Stub initialized to " + std::to_string(width) + "x" + std::to_string(height));
    }

    void Nv2aRenderer::Reset() {
        std::fill(m_frameBuffer.begin(), m_frameBuffer.end(), 0);
    }

    void Nv2aRenderer::RenderTestFrame(float timeInSeconds) {
        // Generate a high-performance dynamic checkerboard pattern with a bouncing ball
        // of varying color to demonstrate continuous rendering without runtime allocation.
        float ballX = m_width / 2.0f + std::sin(timeInSeconds * 2.0f) * (m_width / 3.0f);
        float ballY = m_height / 2.0f + std::cos(timeInSeconds * 3.14f) * (m_height / 3.0f);
        float ballRadius = 40.0f;

        for (uint32_t y = 0; y < m_height; ++y) {
            for (uint32_t x = 0; x < m_width; ++x) {
                size_t offset = (y * m_width + x) * 4;

                // Checkerboard pattern
                bool isCheck = ((x / 32) + (y / 32)) % 2 == 0;
                uint8_t r = isCheck ? 64 : 32;
                uint8_t g = isCheck ? 64 : 32;
                uint8_t b = isCheck ? 128 : 64;

                // Calculate distance to bouncing ball
                float dx = (float)x - ballX;
                float dy = (float)y - ballY;
                float distSq = dx*dx + dy*dy;

                if (distSq < ballRadius * ballRadius) {
                    // Soft glowing ball
                    float intensity = 1.0f - (distSq / (ballRadius * ballRadius));
                    r = (uint8_t)(intensity * 255.0f * (0.5f + 0.5f * std::sin(timeInSeconds)));
                    g = (uint8_t)(intensity * 255.0f * (0.5f + 0.5f * std::sin(timeInSeconds + 2.0f)));
                    b = (uint8_t)(intensity * 255.0f * (0.5f + 0.5f * std::sin(timeInSeconds + 4.0f)));
                }

                m_frameBuffer[offset + 0] = r;
                m_frameBuffer[offset + 1] = g;
                m_frameBuffer[offset + 2] = b;
                m_frameBuffer[offset + 3] = 255; // Alpha
            }
        }
    }

} // namespace XboxOriginal
