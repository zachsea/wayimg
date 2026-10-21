#pragma once
#include "menu_component.h"
#include <functional>
#include <string>

class MenuAction final : public MenuComponent {
public:
  using Callback = std::function<void()>;

  MenuAction(std::string label, Callback callback, bool enabled = true);

  [[nodiscard]] SDL_FPoint measure(const MenuStyle& style) const override;
  void draw(SDL_Renderer* renderer, const SDL_FRect& bounds, const MenuStyle& style,
            MenuItemState state) const override;
  [[nodiscard]] bool isInteractive() const noexcept override { return m_enabled; }
  void activate(MenuContext& ctx, const SDL_FRect&) override;

private:
  std::string m_label;
  Callback m_callback;
  bool m_enabled;
};
