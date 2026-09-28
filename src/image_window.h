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

  // Get the SDL_Window id
  SDL_WindowID getWindowId();
  // Replace or initially set the image being rendered
  void setImage(std::string filePath);
  // Resize the window based on image zoom and offsets, do not call repeatedly to avoid fighting the WM
  void resizeToFitZoom();
  // Handle events
  void handleEvent(SDL_Event e);
  // Render the window
  void doRender();
  // Kill the window nicely
  void close();

private:
  SDL_Window* m_window{nullptr};
  SDL_Renderer* m_renderer{nullptr};
  SDL_Texture* m_image{nullptr};

  double m_zoom{1.0};
  SDL_FPoint m_panOffset{0.0f, 0.0f};
  bool m_dragging = false;

  struct ClampedSize {
    SDL_Point size;
    float scale;
  };

  ClampedSize clampSizeToDisplay(int desiredW, int desiredH);
  void zoomAtPoint(float cursorX, float cursorY, float newZoom);
};
