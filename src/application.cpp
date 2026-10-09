#include "application.h"
#include "image_window.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>
#include <filesystem>

Window& Application::addWindow(std::unique_ptr<Window> window) {
  const SDL_WindowID id = window->getWindowId();
  auto [it, inserted] = m_windows.try_emplace(id, std::move(window));
  return *it->second;
}

void Application::createImageWindow(const std::string& filePath) {
  std::unique_ptr<ImageWindow> window = std::make_unique<ImageWindow>(host());

  if (!filePath.empty()) {
    if (!std::filesystem::exists(filePath)) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "createImageWindow: File \"%s\" does not exist", filePath.c_str());
    } else {
      window->setImage(filePath);
    }
  }

  addWindow(std::move(window));
}

void Application::openWindow(std::unique_ptr<Window> window) {
  if (window) {
    m_pending.push_back(std::move(window));
  }
}

Window* Application::findWindow(SDL_WindowID id) noexcept {
  if (const auto it = m_windows.find(id); it != m_windows.end()) {
    return it->second.get();
  }
  for (const auto& [rootId, root] : m_windows) {
    if (Window* found = root->findWindow(id)) {
      return found;
    }
  }
  return nullptr;
}

void Application::handleEvent(const SDL_Event& e) {
  SDL_Window* eventWindow = SDL_GetWindowFromEvent(&e);
  if (!eventWindow) {
    return;
  }

  const SDL_WindowID eventWindowId = SDL_GetWindowID(eventWindow);
  if (!eventWindowId) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "handleEvent: Window event had no ID: %s", SDL_GetError());
    return;
  }

  // Late events for an already-closed window are expected, hence debug level
  Window* target = findWindow(eventWindowId);
  if (!target) {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "handleEvent: no window with ID %u", eventWindowId);
    return;
  }

  switch (e.type) {
    // intercept events that need to be managed at the app level
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
      SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "close %u", eventWindowId);
      target->requestClose();
      break;
    }
    // pass the event through for the specific window to handle on its own
    default: {
      target->handleEvent(e);
      break;
    }
  }

  settle();
}

void Application::doRender() {
  for (const auto& [id, window] : m_windows) {
    window->renderTree();
  }
  settle();
}

void Application::settle() {
  std::erase_if(m_windows, [](const auto& entry) { return entry.second->wantsClose(); });
  for (const auto& [id, window] : m_windows) {
    window->reapClosedChildren();
  }

  if (!m_pending.empty()) {
    // swap out first so adoption can't interact with new requests
    std::vector<std::unique_ptr<Window>> pending = std::move(m_pending);
    m_pending.clear();
    for (auto& window : pending) {
      addWindow(std::move(window));
    }
  }
}
