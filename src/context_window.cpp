#include "context_window.h"

ContextWindow::ContextWindow(Window& parent, int x, int y)
    : Window(parent, WIDTH, HEIGHT, x, y, SDL_WINDOW_POPUP_MENU), m_owner(parent) {}

void ContextWindow::handleEvent(const SDL_Event& e) {
  // only handles one item, rework
  switch (e.type) {
    case SDL_EVENT_MOUSE_MOTION:
      m_hovered = true;
      break;
    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
      m_hovered = false;
      break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
      // the release of the right click that opened us may land here
      if (e.button.button == SDL_BUTTON_LEFT) {
        m_owner.requestClose();
      }
      break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
      // dismissed by clicking elsewhere
      requestClose();
      break;
    default:
      break;
  }
}

void ContextWindow::doRender() {
  if (!isOpen()) {
    return;
  }

  SDL_Renderer* r = renderer();
  SDL_SetRenderDrawColor(r, 32, 32, 36, 255);
  SDL_RenderClear(r);

  if (m_hovered) {
    const SDL_FRect item{0.0f, 0.0f, static_cast<float>(WIDTH), static_cast<float>(HEIGHT)};
    SDL_SetRenderDrawColor(r, 70, 70, 80, 255);
    SDL_RenderFillRect(r, &item);
  }

  // TODO: font
  SDL_SetRenderDrawColor(r, 230, 230, 230, 255);
  SDL_RenderDebugText(r, 12.0f, (HEIGHT - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2.0f, "Close");

  SDL_RenderPresent(r);
}
