#include "window.h"
#include <SDL3/SDL_log.h>
#include <algorithm>
#include <cmath>
#include <format>
#include <stdexcept>

Window::Window(const char* title, int width, int height, SDL_WindowFlags flags) {
  if (!SDL_CreateWindowAndRenderer(title, width, height, flags, &m_window, &m_renderer)) {
    destroyPrimitives();
    throw std::runtime_error(std::format("CreateWindowAndRenderer failed: {}", SDL_GetError()));
  }
}

Window::~Window() { destroyPrimitives(); }

SDL_WindowID Window::getWindowId() const noexcept { return SDL_GetWindowID(m_window); }
bool Window::isOpen() const noexcept { return m_window != nullptr; }

void Window::close() noexcept {
  if (!isOpen())
    return;
  onClose();
  destroyPrimitives();
}

void Window::destroyPrimitives() noexcept {
  if (m_renderer) {
    SDL_DestroyRenderer(m_renderer);
    m_renderer = nullptr;
  }
  if (m_window) {
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
  }
}

Window::ClampedSize Window::clampSizeToDisplay(int desiredW, int desiredH) const {
  float scale = 1.0f;
  SDL_DisplayID display = SDL_GetDisplayForWindow(m_window);
  if (display) {
    SDL_Rect bounds;
    if (SDL_GetDisplayUsableBounds(display, &bounds)) {
      if (desiredW > bounds.w || desiredH > bounds.h) {
        scale = std::min(bounds.w / static_cast<float>(desiredW), bounds.h / static_cast<float>(desiredH));
        desiredW = static_cast<int>(std::lround(desiredW * scale));
        desiredH = static_cast<int>(std::lround(desiredH * scale));
      }
    }
  }
  return {{desiredW, desiredH}, scale};
}
