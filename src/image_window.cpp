#include "image_window.h"
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <format>
#include <stdexcept>

ImageWindow::ImageWindow() {
  SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS;
  // TODO: better window title
  if (!SDL_CreateWindowAndRenderer("wayimg", 100, 100, flags, &m_window, &m_renderer)) {
    throw std::runtime_error(std::format("CreateWindowAndRenderer failed: {}", SDL_GetError()));
  }
}

ImageWindow::~ImageWindow() {
  if (m_renderer)
    SDL_DestroyRenderer(m_renderer);
  if (m_window)
    SDL_DestroyWindow(m_window);
}

SDL_WindowID ImageWindow::getWindowId() {
  // return 0 if null window or error
  if (!m_window) {
    throw std::runtime_error("getWindowId failed: m_window was null");
  }
  SDL_WindowID windowId = SDL_GetWindowID(m_window);
  if (!windowId) {
    throw std::runtime_error(std::format("getWindowId failed: {}", SDL_GetError()));
  }
  return windowId;
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
  SDL_SetWindowSize(m_window, m_image->w, m_image->h);
};

void ImageWindow::handleEvent(SDL_Event e) {}

void ImageWindow::doRender() {
  SDL_RenderClear(m_renderer);

  if (m_image) {
    float texW, texH;
    int winW, winH;

    SDL_GetTextureSize(m_image, &texW, &texH);
    SDL_GetWindowSize(m_window, &winW, &winH);

    SDL_FRect dst;
    dst.x = (winW - texW) / 2.0f;
    dst.y = (winH - texH) / 2.0f;
    dst.w = texW;
    dst.h = texH;

    SDL_RenderTexture(m_renderer, m_image, nullptr, &dst);
  }

  SDL_RenderPresent(m_renderer);
}
