#ifndef PLATFORM_SDL_HPP
#define PLATFORM_SDL_HPP
#include <memory>
#include <string>

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
class SDL_Surface;

using SDL_WindowFlags = unsigned long;

class ExceptionSDL : std::exception {
public:
  ExceptionSDL(const std::string &msg_);
  const char *what() const noexcept override { return msg.c_str(); }

private:
  std::string msg;
};

struct WindowDestroyer {
  void operator()(SDL_Window *window) const;
};

struct RendererDestroyer {
  void operator()(SDL_Renderer *renderer) const;
};

struct TextureDestroyer {
  void operator()(SDL_Texture *texture) const;
};

struct SurfaceDestroyer {
  void operator()(SDL_Surface *surface) const;
};

struct PlatformSDL {
  using Window = std::unique_ptr<SDL_Window, WindowDestroyer>;
  using Renderer = std::unique_ptr<SDL_Renderer, RendererDestroyer>;
  using Texture = std::unique_ptr<SDL_Texture, TextureDestroyer>;
  using Surface = std::unique_ptr<SDL_Surface, SurfaceDestroyer>;

  PlatformSDL();

  ~PlatformSDL();

  static Window create_window(const std::string &title, int w, int h,
                              SDL_WindowFlags flags);

  static Renderer create_renderer(SDL_Window &window);
};

#endif
