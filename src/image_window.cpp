#include "image_window.h"
#include "context_window.h"
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <algorithm>
#include <cmath>
#include <format>
#include <stdexcept>

// TODO: better window title
ImageWindow::ImageWindow(WindowHost& host)
    : Window(host, "wayimg", 100, 100, SDL_WINDOW_RESIZABLE | SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS) {}

ImageWindow::~ImageWindow() { close(); }

void ImageWindow::onClose() noexcept {
  if (m_image) {
    SDL_DestroyTexture(m_image);
    m_image = nullptr;
  }
}

void ImageWindow::setImage(const std::string& filePath) {
  SDL_Texture* newImage = IMG_LoadTexture(renderer(), filePath.c_str());
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

  SDL_SetWindowSize(window(), clamped.size.x, clamped.size.y);
}

void ImageWindow::resizeToFitZoom() {
  if (!m_image || !isOpen()) {
    return;
  }

  float texW, texH;
  SDL_GetTextureSize(m_image, &texW, &texH);
  texW *= m_zoom;
  texH *= m_zoom;

  int winW, winH;
  SDL_GetWindowSize(window(), &winW, &winH);

  int desiredW = std::max(winW, static_cast<int>(std::ceil(texW)));
  int desiredH = std::max(winH, static_cast<int>(std::ceil(texH)));

  if (desiredW == winW && desiredH == winH) {
    return; // still fits, nothing to do
  }

  ClampedSize clamped = clampSizeToDisplay(desiredW, desiredH);
  SDL_SetWindowSize(window(), clamped.size.x, clamped.size.y);
}

void ImageWindow::zoomAtPoint(float cursorX, float cursorY, float newZoom) {
  if (!isOpen() || !m_image) {
    m_zoom = newZoom;
    return;
  }

  int winW, winH;
  SDL_GetWindowSize(window(), &winW, &winH);

  float oldZoom = m_zoom;
  float zoomRatio = newZoom / oldZoom;

  // cursor's offset from the image's current on-screen center
  float dx = cursorX - winW / 2.0f - m_panOffset.x;
  float dy = cursorY - winH / 2.0f - m_panOffset.y;

  m_zoom = newZoom;
  resizeToFitZoom();

  int newWinW, newWinH;
  SDL_GetWindowSize(window(), &newWinW, &newWinH);

  m_panOffset.x = (cursorX - newWinW / 2.0f) - dx * zoomRatio;
  m_panOffset.y = (cursorY - newWinH / 2.0f) - dy * zoomRatio;
}

void ImageWindow::handleEvent(const SDL_Event& e) {
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
      } else if (e.button.button == SDL_BUTTON_RIGHT) {
        try {
          emplaceChild<ContextWindow>(static_cast<int>(e.button.x), static_cast<int>(e.button.y));
        } catch (const std::exception& ex) {
          SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "context menu failed: %s", ex.what());
        }
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
  if (!isOpen()) {
    return;
  }

  SDL_RenderClear(renderer());

  if (m_image) {
    float texW, texH;
    int winW, winH;

    SDL_GetTextureSize(m_image, &texW, &texH);
    SDL_GetWindowSize(window(), &winW, &winH);

    texW *= m_zoom;
    texH *= m_zoom;

    SDL_FRect dst;
    dst.x = (winW - texW) / 2.0f + m_panOffset.x;
    dst.y = (winH - texH) / 2.0f + m_panOffset.y;
    dst.w = texW;
    dst.h = texH;

    // TODO: make configurable
    SDL_SetTextureScaleMode(m_image, SDL_SCALEMODE_PIXELART);

    SDL_RenderTexture(renderer(), m_image, nullptr, &dst);
  }

  SDL_RenderPresent(renderer());
}
