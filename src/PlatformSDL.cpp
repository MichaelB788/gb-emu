#include "PlatformSDL.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

void WindowDestroyer::operator()(SDL_Window *window) const {
  SDL_DestroyWindow(window);
}

void RendererDestroyer::operator()(SDL_Renderer *renderer) const {
  SDL_DestroyRenderer(renderer);
}

void TextureDestroyer::operator()(SDL_Texture *texture) const {
  SDL_DestroyTexture(texture);
}

void SurfaceDestroyer::operator()(SDL_Surface *surface) const {
  SDL_DestroySurface(surface);
}

ExceptionSDL::ExceptionSDL(const std::string &msg_)
    : msg(msg_ + ": " + SDL_GetError()) {}

PlatformSDL::PlatformSDL() {
  if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
    throw ExceptionSDL("Failed to initialize the SDL video subsystem");
}

PlatformSDL::~PlatformSDL() { SDL_Quit(); }

PlatformSDL::Window PlatformSDL::create_window(const std::string &title, int w,
                                               int h, SDL_WindowFlags flags) {
  SDL_Window *raw_window = SDL_CreateWindow(title.c_str(), w, h, flags);
  if (!raw_window)
    throw ExceptionSDL("Failed to initialize the SDL window");

  return Window(raw_window);
}

PlatformSDL::Renderer PlatformSDL::create_renderer(SDL_Window &window) {
  SDL_Renderer *raw_renderer = SDL_CreateRenderer(&window, NULL);
  if (!raw_renderer)
    throw ExceptionSDL("Failed to initialize the SDL renderer");

  return Renderer(raw_renderer);
}
