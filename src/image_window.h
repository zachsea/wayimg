#pragma once
#include <SDL3/SDL_render.h>
#include <string>

class ImageWindow {
public:
  ImageWindow();
  ~ImageWindow();
  ImageWindow(const ImageWindow&) = delete;
  ImageWindow& operator=(const ImageWindow&) = delete;
  ImageWindow(ImageWindow&&) = delete;
  ImageWindow& operator=(ImageWindow&&) = delete;

  /* LIFECYCLE */

  // Get the SDL_Window id
  SDL_WindowID getWindowId();
  // Replace or initially set the image being rendered
  void setImage(std::string filePath);
  // Handle events
  void handleEvent(SDL_Event e);
  // Render the window
  void doRender();

private:
  SDL_Window* m_window{nullptr};
  SDL_Renderer* m_renderer{nullptr};
  SDL_Texture* m_image{nullptr};
};
