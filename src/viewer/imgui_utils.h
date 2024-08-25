#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"

void init_imgui(SDL_Window* window, SDL_GLContext* gl_context);

void imgui_start_frame();

void imgui_end_frame();

void imgui_destroy();
