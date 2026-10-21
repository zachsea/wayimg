#pragma once
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

struct MenuStyle {
  // Menu window
  SDL_Color background{32, 32, 36, 255};
  float padding{4.0f};

  // Items
  SDL_Color text{230, 230, 230, 255};
  SDL_Color textDisabled{130, 130, 138, 255};
  SDL_Color hoverBackground{70, 70, 80, 255};
  float itemHeight{28.0f};
  float itemInsetX{12.0f};
  float minItemWidth{120.0f};

  // Separators
  SDL_Color separator{70, 70, 80, 255};
  float separatorHeight{9.0f};
};

struct MenuItemState {
  bool hovered;
};

class MenuContext;

class MenuComponent {
public:
  virtual ~MenuComponent() = default;
  MenuComponent(const MenuComponent&) = delete;
  MenuComponent& operator=(const MenuComponent&) = delete;

  [[nodiscard]] virtual SDL_FPoint measure(const MenuStyle&) const = 0;
  virtual void draw(SDL_Renderer*, const SDL_FRect& bounds, const MenuStyle&, MenuItemState) const = 0;
  [[nodiscard]] virtual bool isInteractive() const noexcept { return true; }
  virtual void activate(MenuContext& ctx, const SDL_FRect& bounds) = 0;

protected:
  MenuComponent() = default;
};
