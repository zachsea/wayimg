#include "image_window.h"
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <algorithm>
#include <cmath>
#include <format>
#include <stdexcept>

ImageWindow::ImageWindow() {
  SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS;
  // TODO: better window title
  if (!SDL_CreateWindowAndRenderer("wayimg", 100, 100, flags, &m_window, &m_renderer)) {
    throw std::runtime_error(std::format("CreateWindowAndRenderer failed: {}", SDL_GetError()));
  }
}

ImageWindow::~ImageWindow() { close(); }

SDL_WindowID ImageWindow::getWindowId() {
  if (!m_window) {
    throw std::runtime_error("getWindowId failed: m_window was null");
  }
  SDL_WindowID windowId = SDL_GetWindowID(m_window);
  if (!windowId) {
    throw std::runtime_error(std::format("getWindowId failed: {}", SDL_GetError()));
  }
  return windowId;
}

ImageWindow::ClampedSize ImageWindow::clampSizeToDisplay(int desiredW, int desiredH) {
  float scale = 1.0f;
  SDL_DisplayID display = SDL_GetDisplayForWindow(m_window);
  if (display) {
    SDL_Rect bounds;
    if (SDL_GetDisplayUsableBounds(display, &bounds)) {
      if (desiredW > bounds.w || desiredH > bounds.h) {
        scale = std::min(bounds.w / static_cast<float>(desiredW), bounds.h / static_cast<float>(desiredH));
        desiredW = static_cast<int>(std::lround(desiredW * scale));
        desiredH = static_cast<int>(std::lround(desiredH * scale));
      }
    }
  }
  return {{desiredW, desiredH}, scale};
}

void ImageWindow::setImage(std::string filePath) {
  SDL_Texture* newImage = IMG_LoadTexture(m_renderer, filePath.c_str());
  if (!newImage) {
    throw std::runtime_error(std::format("IMG_LoadTexture failed: {}", SDL_GetError()));
  }
  if (m_image) {
    SDL_DestroyTexture(m_image);
  }
  m_image = newImage;
  m_panOffset = {0.0f, 0.0f};

  float texW, texH;
  SDL_GetTextureSize(m_image, &texW, &texH);

  ClampedSize clamped = clampSizeToDisplay(static_cast<int>(std::ceil(texW)), static_cast<int>(std::ceil(texH)));

  // TODO: make this toggleable ("fit to window on open" vs. always 1:1)
  m_zoom = clamped.scale;

  SDL_SetWindowSize(m_window, clamped.size.x, clamped.size.y);
}

void ImageWindow::resizeToFitZoom() {
  if (!m_image || !m_window) {
    return;
  }

  float texW, texH;
  SDL_GetTextureSize(m_image, &texW, &texH);
  texW *= m_zoom;
  texH *= m_zoom;

  int winW, winH;
  SDL_GetWindowSize(m_window, &winW, &winH);

  int desiredW = std::max(winW, static_cast<int>(std::ceil(texW)));
  int desiredH = std::max(winH, static_cast<int>(std::ceil(texH)));

  if (desiredW == winW && desiredH == winH) {
    return; // still fits, nothing to do
  }

  ClampedSize clamped = clampSizeToDisplay(desiredW, desiredH);
  SDL_SetWindowSize(m_window, clamped.size.x, clamped.size.y);
}

void ImageWindow::zoomAtPoint(float cursorX, float cursorY, float newZoom) {
  if (!m_window || !m_image) {
    m_zoom = newZoom;
    return;
  }

  int winW, winH;
  SDL_GetWindowSize(m_window, &winW, &winH);

  float oldZoom = m_zoom;
  float zoomRatio = newZoom / oldZoom;

  // cursor's offset from the image's current on-screen center
  float dx = cursorX - winW / 2.0f - m_panOffset.x;
  float dy = cursorY - winH / 2.0f - m_panOffset.y;

  m_zoom = newZoom;
  resizeToFitZoom();

  int newWinW, newWinH;
  SDL_GetWindowSize(m_window, &newWinW, &newWinH);

  m_panOffset.x = (cursorX - newWinW / 2.0f) - dx * zoomRatio;
  m_panOffset.y = (cursorY - newWinH / 2.0f) - dy * zoomRatio;
}

void ImageWindow::handleEvent(SDL_Event e) {
  // TODO: make keys configurable, abstract responsibility out of image_window
  switch (e.type) {
    case SDL_EVENT_MOUSE_WHEEL: {
      const float ZOOM_FACTOR = 1.15f;

      float newZoom = m_zoom;
      if (e.wheel.y > 0) {
        newZoom *= ZOOM_FACTOR;
      } else if (e.wheel.y < 0) {
        newZoom /= ZOOM_FACTOR;
      }
      newZoom = std::fmax(0.001f, newZoom);

      zoomAtPoint(e.wheel.mouse_x, e.wheel.mouse_y, newZoom);
      break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
      if (e.button.button == SDL_BUTTON_LEFT) {
        m_dragging = true;
      }
      break;
    }
    case SDL_EVENT_MOUSE_BUTTON_UP: {
      if (e.button.button == SDL_BUTTON_LEFT) {
        m_dragging = false;
      }
      break;
    }
    case SDL_EVENT_MOUSE_MOTION: {
      if (m_dragging) {
        m_panOffset.x += e.motion.xrel;
        m_panOffset.y += e.motion.yrel;
      }
      break;
    }
    default:
      break;
  }
}

void ImageWindow::doRender() {
  SDL_RenderClear(m_renderer);

  if (m_image) {
    float texW, texH;
    int winW, winH;

    SDL_GetTextureSize(m_image, &texW, &texH);
    SDL_GetWindowSize(m_window, &winW, &winH);

    texW *= m_zoom;
    texH *= m_zoom;

    SDL_FRect dst;
    dst.x = (winW - texW) / 2.0f + m_panOffset.x;
    dst.y = (winH - texH) / 2.0f + m_panOffset.y;
    dst.w = texW;
    dst.h = texH;

    // TODO: make configurable
    SDL_SetTextureScaleMode(m_image, SDL_SCALEMODE_PIXELART);

    SDL_RenderTexture(m_renderer, m_image, nullptr, &dst);
  }

  SDL_RenderPresent(m_renderer);
}

void ImageWindow::close() {
  if (m_renderer) {
    SDL_DestroyRenderer(m_renderer);
    m_renderer = nullptr;
  }
  if (m_window) {
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
  }
}
