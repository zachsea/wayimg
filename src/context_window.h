#pragma once
#include "window.h"

// Popup menu (just has one item for now, we need a "builder" paradigm or something)
class ContextWindow final : public Window {
public:
  // (x, y) is relative to the parent's top-left corner
  ContextWindow(Window& parent, int x, int y);

  void handleEvent(const SDL_Event& e) override;
  void doRender() override;

private:
  static constexpr int WIDTH = 120;
  static constexpr int HEIGHT = 28;

  Window& m_owner; // the parent owns us, so it always outlives
  bool m_hovered{false};
};
