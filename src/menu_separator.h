#pragma once
#include "menu_component.h"

class MenuSeparator final : public MenuComponent {
public:
  [[nodiscard]] SDL_FPoint measure(const MenuStyle& style) const override;
  void draw(SDL_Renderer* renderer, const SDL_FRect& bounds, const MenuStyle& style,
            MenuItemState state) const override;
  [[nodiscard]] bool isInteractive() const noexcept override { return false; }
  void activate(MenuContext&, const SDL_FRect&) override {}
};
