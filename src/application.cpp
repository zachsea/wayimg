#include "application.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>

Application::Application() {}

Application::~Application() { m_windows.clear(); }

void Application::createImageWindow(std::string filePath) {
  auto window = std::make_unique<ImageWindow>();
  SDL_WindowID id = window->getWindowId();
  if (!filePath.empty()) {
    window->setImage(filePath);
  }
  m_windows.emplace(id, std::move(window));
}

void Application::handleEvent(SDL_Event e) {
  SDL_Window* eventWindow = SDL_GetWindowFromEvent(&e);
  if (eventWindow) {
    SDL_WindowID eventWindowId = SDL_GetWindowID(eventWindow);
    if (eventWindowId) {
      m_windows[SDL_GetWindowID(eventWindow)]->handleEvent(e);
    } else {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "handleEvent: Window event had no ID: %s", SDL_GetError());
    }
  }
}

void Application::doRender() {
  for (auto& [id, window] : m_windows) {
    window->doRender();
  }
}
