#pragma once
#include "window.h"
#include <SDL3/SDL_events.h>
#include <memory>
#include <string>
#include <unordered_map>

class Application : private WindowHost {
public:
  Application() = default;
  ~Application() = default;

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;
  Application(Application&&) = delete;
  Application& operator=(Application&&) = delete;

  // Take ownership of an already-constructed top-level window
  Window& addWindow(std::unique_ptr<Window> window);
  // Open a new image window
  void createImageWindow(const std::string& filePath);
  // Route an event to the window it targets
  void handleEvent(const SDL_Event& e);
  // Render all windows
  void doRender();
  // True while any top-level window is open or waiting to be adopted
  [[nodiscard]] bool hasWindows() const noexcept { return !m_windows.empty() || !m_pending.empty(); }

private:
  void openWindow(std::unique_ptr<Window> window) override;

  [[nodiscard]] WindowHost& host() noexcept { return *this; }
  [[nodiscard]] Window* findWindow(SDL_WindowID id) noexcept;
  // Reap closed windows and adopt pending
  void settle();

  std::unordered_map<SDL_WindowID, std::unique_ptr<Window>> m_windows;
  std::vector<std::unique_ptr<Window>> m_pending;
};
