#include "menu_separator.h"

SDL_FPoint MenuSeparator::measure(const MenuStyle& style) const { return {style.minItemWidth, style.separatorHeight}; }

void MenuSeparator::draw(SDL_Renderer* renderer, const SDL_FRect& bounds, const MenuStyle& style, MenuItemState) const {
  const SDL_Color& c = style.separator;
  const float y = bounds.y + bounds.h / 2.0f;
  SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
  SDL_RenderLine(renderer, bounds.x, y, bounds.x + bounds.w, y);
}
