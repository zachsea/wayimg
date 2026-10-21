#include "context_window.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <algorithm>
#include <cmath>
#include <exception>

ContextWindow::ContextWindow(Window& parent, int x, int y, MenuItems items, MenuStyle style)
    : ContextWindow(parent, x, y, prepare(std::move(items), style), style) {}

ContextWindow::ContextWindow(Window& parent, int x, int y, Prepared prepared, const MenuStyle& style)
    : Window(parent, prepared.size.x, prepared.size.y, x, y, SDL_WINDOW_POPUP_MENU), m_style(style),
      m_items(std::move(prepared.items)), m_rects(std::move(prepared.rects)),
      m_parentMenu(dynamic_cast<ContextWindow*>(&parent)) {}

ContextWindow::Prepared ContextWindow::prepare(MenuItems items, const MenuStyle& style) {
  std::vector<SDL_FPoint> sizes;
  sizes.reserve(items.size());

  float width = 0.0f;
  for (const auto& item : items) {
    const SDL_FPoint s = item->measure(style);
    sizes.push_back(s);
    width = std::max(width, s.x);
  }

  Prepared p;
  p.rects.reserve(items.size());
  float y = style.padding;
  for (const SDL_FPoint& s : sizes) {
    p.rects.push_back(SDL_FRect{style.padding, y, width, s.y});
    y += s.y;
  }

  p.size = {std::max(1, static_cast<int>(std::ceil(width + 2.0f * style.padding))),
            std::max(1, static_cast<int>(std::ceil(y + style.padding)))};
  p.items = std::move(items);
  return p;
}

void ContextWindow::handleEvent(const SDL_Event& e) {
  switch (e.type) {
    case SDL_EVENT_MOUSE_MOTION:
      m_hovered = interactiveItemAt(e.motion.x, e.motion.y);
      break;
    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
      m_hovered.reset();
      break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
      if (e.button.button == SDL_BUTTON_LEFT) {
        if (const auto i = interactiveItemAt(e.button.x, e.button.y)) {
          m_items[*i]->activate(*this, m_rects[*i]);
        }
      }
      break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
      if (!hasOpenSubmenu()) {
        dismissAll();
      }
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
  const SDL_Color& bg = m_style.background;
  SDL_SetRenderDrawColor(r, bg.r, bg.g, bg.b, bg.a);
  SDL_RenderClear(r);

  for (std::size_t i = 0; i < m_items.size(); ++i) {
    m_items[i]->draw(r, m_rects[i], m_style, MenuItemState{m_hovered == i});
  }

  SDL_RenderPresent(r);
}

void ContextWindow::dismissAll() noexcept {
  // Closing the root reaps every submenu below it
  ContextWindow* root = this;
  while (root->m_parentMenu) {
    root = root->m_parentMenu;
  }
  root->requestClose();
}

void ContextWindow::openSubmenu(MenuBuilder build, const SDL_FRect& anchor) {
  MenuItems items = build ? build() : MenuItems{};
  if (items.empty()) {
    return;
  }

  clearChildren();

  try {
    auto& submenu = emplaceChild<ContextWindow>(anchor.x + anchor.w, anchor.y, std::move(items), m_style);
    m_submenuId = submenu.getWindowId();
  } catch (const std::exception& ex) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "submenu failed: %s", ex.what());
    m_submenuId = 0;
  }
}

std::optional<std::size_t> ContextWindow::interactiveItemAt(float x, float y) const {
  const SDL_FPoint p{x, y};
  for (std::size_t i = 0; i < m_rects.size(); ++i) {
    if (SDL_PointInRectFloat(&p, &m_rects[i])) {
      if (m_items[i]->isInteractive()) {
        return i;
      }
      return std::nullopt;
    }
  }
  return std::nullopt;
}

bool ContextWindow::hasOpenSubmenu() noexcept { return m_submenuId != 0 && findWindow(m_submenuId) != nullptr; }
