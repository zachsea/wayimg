#pragma once
#include "window_host.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <vector>

// Owns the SDL_Window + SDL_Renderer.
class Window {
public:
  virtual ~Window();

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
  Window(Window&&) = delete;
  Window& operator=(Window&&) = delete;

  // Get the SDL_Window id
  [[nodiscard]] SDL_WindowID getWindowId() const noexcept;
  // False if construction failed or close() has been called
  [[nodiscard]] bool isOpen() const noexcept;

  // Handle events
  virtual void handleEvent(const SDL_Event& e) = 0;
  // Render the window
  virtual void doRender() = 0;

  // Kill the window nicely
  void close() noexcept;
  // Ask the owner to destroy this window after the current dispatch finishes
  void requestClose() noexcept { m_closeRequested = true; }
  [[nodiscard]] bool wantsClose() const noexcept { return m_closeRequested; }

  // This window or any descendant with the given SDL id, else nullptr
  [[nodiscard]] Window* findWindow(SDL_WindowID id) noexcept;
  // Destroy descendants that asked to close. Call from the owner, never mid-callback.
  void reapClosedChildren();

protected:
  // Top-level window
  Window(WindowHost& host, const char* title, int width, int height, SDL_WindowFlags flags);
  // Popup child positioned relative to parent. flags must include SDL_WINDOW_POPUP_MENU or SDL_WINDOW_TOOLTIP.
  Window(Window& parent, int width, int height, int offsetX, int offsetY, SDL_WindowFlags flags);

  // Called once from close(), while the renderer is still alive, release renderer-owned resources (textures, etc.)
  // here.
  virtual void onClose() noexcept {}

  [[nodiscard]] SDL_Window* window() const noexcept { return m_window; }
  [[nodiscard]] SDL_Renderer* renderer() const noexcept { return m_renderer; }
  [[nodiscard]] WindowHost& host() const noexcept { return m_host; }

  struct ClampedSize {
    SDL_Point size;
    float scale;
  };
  // Clamp a desired size to the display this window is on
  [[nodiscard]] ClampedSize clampSizeToDisplay(int desiredW, int desiredH) const;

private:
  void destroyPrimitives() noexcept;

  WindowHost& m_host;
  SDL_Window* m_window{nullptr};
  SDL_Renderer* m_renderer{nullptr};
  std::vector<std::unique_ptr<Window>> m_children;
  bool m_closeRequested{false};
};
