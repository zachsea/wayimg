#pragma once
#include <memory>

class Window;

class WindowHost {
public:
  virtual ~WindowHost() = default;

  // Give higher up context ownership of the window
  virtual void openWindow(std::unique_ptr<Window> window) = 0;
};
