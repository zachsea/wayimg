#include "application.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <filesystem>

Application::Application() {}

Application::~Application() { m_windows.clear(); }

void Application::createImageWindow(std::string filePath) {
  auto window = std::make_unique<ImageWindow>();
  SDL_WindowID id = window->getWindowId();

  if (!filePath.empty()) {
    if (!std::filesystem::exists(filePath)) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "createImageWindow: File \"%s\" does not exist", filePath.c_str());
    } else {
      window->setImage(filePath);
    }
  }

  m_windows.emplace(id, std::move(window));
}

void Application::handleEvent(SDL_Event e) {
  // handle window specific events
  SDL_Window* eventWindow = SDL_GetWindowFromEvent(&e);

  if (eventWindow) {
    SDL_WindowID eventWindowId = SDL_GetWindowID(eventWindow);

    if (!eventWindowId) {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "handleEvent: Window event had no ID: %s", SDL_GetError());
      return;
    }

    // intercept window events that need to be managed at the app level
    switch (e.type) {
      case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "close %u", eventWindowId);
        m_windows[eventWindowId]->close();
        m_windows.erase(eventWindowId);
        break;
      }
      // pass the event through for the specific window to handle on its own
      default: {
        m_windows[eventWindowId]->handleEvent(e);
        break;
      }
    }
  }
}

void Application::doRender() {
  for (auto& [id, window] : m_windows) {
    window->doRender();
  }
}
