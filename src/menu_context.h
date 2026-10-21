#pragma once
#include "menu_component.h"
#include <SDL3/SDL_rect.h>
#include <functional>
#include <memory>
#include <vector>

using MenuItems = std::vector<std::unique_ptr<MenuComponent>>;
using MenuBuilder = std::function<MenuItems()>;

class MenuContext {
public:
  // Close the whole menu chain, root menu included; deferred (via requestClose), can be called in activate
  virtual void dismissAll() noexcept = 0;
  // Open a submenu next to anchor, replaces any submenu this menu already has open
  virtual void openSubmenu(MenuBuilder build, const SDL_FRect& anchor) = 0;

protected:
  MenuContext() = default;
  ~MenuContext() = default;
};
