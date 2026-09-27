#pragma once
#include "image_window.h"
#include <SDL3/SDL_render.h>
#include <memory>
#include <string>
#include <unordered_map>

class Application {
public:
  Application();
  ~Application();
  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;
  Application(Application&&) = delete;
  Application& operator=(Application&&) = delete;

  // Open a new image window
  void createImageWindow(std::string filePath);
  // Handle events
  void handleEvent(SDL_Event e);
  // Render the main window
  void doRender();

private:
  std::unordered_map<SDL_WindowID, std::unique_ptr<ImageWindow>> m_windows;
};
