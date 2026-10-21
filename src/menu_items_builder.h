#pragma once
#include "menu_action.h"
#include "menu_context.h"
#include "menu_separator.h"
#include "menu_submenu.h"
#include <memory>
#include <string>

class MenuItemsBuilder {
public:
  MenuItemsBuilder& action(std::string label, MenuAction::Callback cb, bool enabled = true) {
    m_items.push_back(std::make_unique<MenuAction>(std::move(label), std::move(cb), enabled));
    return *this;
  }
  MenuItemsBuilder& separator() {
    m_items.push_back(std::make_unique<MenuSeparator>());
    return *this;
  }
  MenuItemsBuilder& submenu(std::string label, MenuBuilder build) {
    m_items.push_back(std::make_unique<MenuSubmenu>(std::move(label), std::move(build)));
    return *this;
  }
  [[nodiscard]] MenuItems build() { return std::move(m_items); }

private:
  MenuItems m_items;
};
