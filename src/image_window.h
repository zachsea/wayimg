#pragma once
#include "window.h"
#include <string>

class ImageWindow final : public Window {
public:
  explicit ImageWindow(WindowHost& host);
  ~ImageWindow() override;

  // Replace or initially set the image being rendered
  void setImage(const std::string& filePath);
  // Resize the window based on image zoom and offsets, do not call repeatedly to avoid fighting the WM
  void resizeToFitZoom();

  void handleEvent(const SDL_Event& e) override;
  void doRender() override;

protected:
  void onClose() noexcept override;

private:
  void zoomAtPoint(float cursorX, float cursorY, float newZoom);

  SDL_Texture* m_image{nullptr};
  double m_zoom{1.0};
  SDL_FPoint m_panOffset{0.0f, 0.0f};
  bool m_dragging{false};
};
