#include "menu_submenu.h"
#include <algorithm>

namespace {
constexpr float GLYPH = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
constexpr const char* ARROW = ">";
} // namespace

MenuSubmenu::MenuSubmenu(std::string label, MenuBuilder build, bool enabled)
    : m_label(std::move(label)), m_build(std::move(build)), m_enabled(enabled) {}

SDL_FPoint MenuSubmenu::measure(const MenuStyle& style) const {
  // label, one glyph of gap, one glyph for the arrow
  const float textWidth = m_label.size() * GLYPH;
  return {std::max(style.minItemWidth, textWidth + 2.0f * GLYPH + 2.0f * style.itemInsetX), style.itemHeight};
}

void MenuSubmenu::draw(SDL_Renderer* renderer, const SDL_FRect& bounds, const MenuStyle& style,
                       MenuItemState state) const {
  if (m_enabled && state.hovered) {
    const SDL_Color& h = style.hoverBackground;
    SDL_SetRenderDrawColor(renderer, h.r, h.g, h.b, h.a);
    SDL_RenderFillRect(renderer, &bounds);
  }

  const SDL_Color& c = m_enabled ? style.text : style.textDisabled;
  SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);

  const float textY = bounds.y + (bounds.h - GLYPH) / 2.0f;
  SDL_RenderDebugText(renderer, bounds.x + style.itemInsetX, textY, m_label.c_str());
  SDL_RenderDebugText(renderer, bounds.x + bounds.w - style.itemInsetX - GLYPH, textY, ARROW);
}

void MenuSubmenu::activate(MenuContext& ctx, const SDL_FRect& bounds) {
  if (!m_enabled || !m_build) {
    return;
  }
  ctx.openSubmenu(m_build, bounds);
}
