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
  auto window = std::make_unique<ImageWindow>();

  if (!filePath.empty()) {
    if (!std::filesystem::exists(filePath)) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "createImageWindow: File \"%s\" does not exist", filePath.c_str());
    } else {
      window->setImage(filePath);
    }
  }

  addWindow(std::move(window));
}

void Application::handleEvent(const SDL_Event& e) {
  // handle window specific events
  SDL_Window* eventWindow = SDL_GetWindowFromEvent(&e);
  if (!eventWindow) {
    return;
  }

  const SDL_WindowID eventWindowId = SDL_GetWindowID(eventWindow);
  if (!eventWindowId) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "handleEvent: Window event had no ID: %s", SDL_GetError());
    return;
  }

  const auto it = m_windows.find(eventWindowId);
  if (it == m_windows.end()) {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "handleEvent: no window with ID %u", eventWindowId);
    return;
  }

  // intercept window events that need to be managed at the app level
  switch (e.type) {
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
      SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "close %u", eventWindowId);
      it->second->close();
      m_windows.erase(it);
      break;
    }
    // pass the event through for the specific window to handle on its own
    default: {
      it->second->handleEvent(e);
      break;
    }
  }
}

void Application::doRender() {
  for (const auto& [id, window] : m_windows) {
    window->doRender();
  }
}
