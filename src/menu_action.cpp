#include "menu_action.h"
#include "menu_context.h"

MenuAction::MenuAction(std::string label, Callback callback, bool enabled)
    : m_label(std::move(label)), m_callback(std::move(callback)), m_enabled(enabled) {}

SDL_FPoint MenuAction::measure(const MenuStyle& style) const {
  const float textWidth = m_label.size() * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
  return {std::max(style.minItemWidth, textWidth + 2.0f * style.itemInsetX), style.itemHeight};
}

void MenuAction::draw(SDL_Renderer* renderer, const SDL_FRect& bounds, const MenuStyle& style,
                      MenuItemState state) const {
  if (m_enabled && state.hovered) {
    const SDL_Color& h = style.hoverBackground;
    SDL_SetRenderDrawColor(renderer, h.r, h.g, h.b, h.a);
    SDL_RenderFillRect(renderer, &bounds);
  }

  const SDL_Color& c = m_enabled ? style.text : style.textDisabled;
  SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
  SDL_RenderDebugText(renderer, bounds.x + style.itemInsetX,
                      bounds.y + (bounds.h - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2.0f, m_label.c_str());
}

void MenuAction::activate(MenuContext& ctx, const SDL_FRect&) {
  if (!m_enabled) {
    return;
  }
  if (m_callback) {
    m_callback();
  }
  ctx.dismissAll();
}
