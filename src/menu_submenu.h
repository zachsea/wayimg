#pragma once
#include "menu_component.h"
#include "menu_context.h"
#include <string>

class MenuSubmenu final : public MenuComponent {
public:
  MenuSubmenu(std::string label, MenuBuilder build, bool enabled = true);

  [[nodiscard]] SDL_FPoint measure(const MenuStyle& style) const override;
  void draw(SDL_Renderer* renderer, const SDL_FRect& bounds, const MenuStyle& style,
            MenuItemState state) const override;
  [[nodiscard]] bool isInteractive() const noexcept override { return m_enabled; }
  void activate(MenuContext& ctx, const SDL_FRect& bounds) override;

private:
  std::string m_label;
  MenuBuilder m_build;
  bool m_enabled;
};
