#include "window.h"
#include <SDL3/SDL_log.h>
#include <algorithm>
#include <cmath>
#include <format>
#include <stdexcept>

Window::Window(WindowHost& host, const char* title, int width, int height, SDL_WindowFlags flags) : m_host(host) {
  if (!SDL_CreateWindowAndRenderer(title, width, height, flags, &m_window, &m_renderer)) {
    destroyPrimitives();
    throw std::runtime_error(std::format("CreateWindowAndRenderer failed: {}", SDL_GetError()));
  }
}

Window::Window(Window& parent, int width, int height, int offsetX, int offsetY, SDL_WindowFlags flags)
    : m_host(parent.m_host) {
  m_window = SDL_CreatePopupWindow(parent.m_window, offsetX, offsetY, width, height, flags);
  if (!m_window) {
    throw std::runtime_error(std::format("CreatePopupWindow failed: {}", SDL_GetError()));
  }
  m_renderer = SDL_CreateRenderer(m_window, nullptr);
  if (!m_renderer) {
    const std::string msg = std::format("CreateRenderer failed: {}", SDL_GetError());
    destroyPrimitives();
    throw std::runtime_error(msg);
  }
}

Window::~Window() {
  m_children.clear();
  destroyPrimitives();
}

SDL_WindowID Window::getWindowId() const noexcept { return SDL_GetWindowID(m_window); }
bool Window::isOpen() const noexcept { return m_window != nullptr; }

void Window::close() noexcept {
  if (!isOpen())
    return;
  m_children.clear();
  onClose();
  destroyPrimitives();
}

Window* Window::findWindow(SDL_WindowID id) noexcept {
  if (id == 0) {
    return nullptr;
  }
  if (isOpen() && getWindowId() == id) {
    return this;
  }
  for (const auto& child : m_children) {
    if (Window* found = child->findWindow(id)) {
      return found;
    }
  }
  return nullptr;
}

void Window::renderTree() {
  if (!isOpen()) {
    return;
  }
  doRender();
  for (const auto& child : m_children) {
    child->renderTree();
  }
}

void Window::reapClosedChildren() {
  // flagged subtrees die whole, then recurse into the survivors
  std::erase_if(m_children, [](const std::unique_ptr<Window>& c) { return c->wantsClose(); });
  for (const auto& child : m_children) {
    child->reapClosedChildren();
  }
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
