#pragma once
#include "menu_context.h"
#include "window.h"
#include <optional>
#include <vector>

class ContextWindow final : public Window, private MenuContext {
public:
  ContextWindow(Window& parent, int x, int y, MenuItems items, MenuStyle style = {});

  void handleEvent(const SDL_Event& e) override;
  void doRender() override;

private:
  // MenuContext
  void dismissAll() noexcept override;
  void openSubmenu(MenuBuilder build, const SDL_FRect& anchor) override;

  struct Prepared {
    MenuItems items;
    std::vector<SDL_FRect> rects;
    SDL_Point size{};
  };
  ContextWindow(Window& parent, int x, int y, Prepared prepared, const MenuStyle& style);
  [[nodiscard]] static Prepared prepare(MenuItems items, const MenuStyle& style);

  [[nodiscard]] std::optional<std::size_t> interactiveItemAt(float x, float y) const;
  [[nodiscard]] bool hasOpenSubmenu() noexcept;

  MenuStyle m_style;
  MenuItems m_items;
  std::vector<SDL_FRect> m_rects;
  std::optional<std::size_t> m_hovered;
  ContextWindow* m_parentMenu{nullptr};
  SDL_WindowID m_submenuId{0};
};
