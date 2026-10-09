#include <SDL3/SDL_hints.h>
#define SDL_MAIN_USE_CALLBACKS 1

#include "application.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_video.h>
#include <memory>

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
  if (argc < 2) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Usage: %s [file ...]", argv[0]);
    return SDL_APP_FAILURE;
  }

  SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "wayland");
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
  SDL_SetHint(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE, "1");

  SDL_SetAppMetadata("wayimg", "0.1.0", "cafe.zach.wayimg");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // initialize the application class
  try {
    auto app = std::make_unique<Application>();
    // open requested files
    for (int i = 1; i < argc; i++) {
      app->createImageWindow(argv[i]);
    }
    *appstate = app.release();
  } catch (std::exception e) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize application: %s", e.what());
    return SDL_APP_FAILURE;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  // maybe applicaiton should handle this
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  static_cast<Application*>(appstate)->handleEvent(*event);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
  if (appstate) {
    // draw
    static_cast<Application*>(appstate)->doRender();
  }

  return static_cast<Application*>(appstate)->hasWindows() ? SDL_APP_CONTINUE : SDL_APP_SUCCESS;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  if (appstate) {
    std::unique_ptr<Application> app(static_cast<Application*>(appstate));
  }
  SDL_Quit();
}
