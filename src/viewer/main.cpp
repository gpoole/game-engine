#include <iostream>
#include <format>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <gl/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include "core/Md2Model.hpp"
#include "imgui_utils.h"

typedef int32_t i32;
typedef uint32_t u32;
typedef int32_t b32;

#define WinWidth 1000
#define WinHeight 1000

// Globals

glm::vec3 camera_position = glm::vec3(0, 0, 0);
glm::vec3 camera_rotation = glm::vec3(0, 0, 0);
std::unique_ptr<GameEngine::Md2Model> active_model = nullptr;
uint engine_last_tick;

void print_gl_info()
{
    char const* gl_vendor = (char const*)glGetString(GL_VENDOR);
    char const* gl_renderer = (char const*)glGetString(GL_RENDERER);
    char const* gl_version = (char const*)glGetString(GL_VERSION);
    char const* gl_extensions = (char const*)glGetString(GL_EXTENSIONS);
    std::cout << "GL_VENDOR: " << (gl_vendor ? gl_vendor : "(null)") << std::endl;
    std::cout << "GL_RENDERER: " << (gl_renderer ? gl_renderer : "(null)") << std::endl;
    std::cout << "GL_VERSION: " << (gl_version ? gl_version : "(null)") << std::endl;
    std::cout << "GL_EXTENSIONS: " << (gl_extensions ? gl_extensions : "(null)") << std::endl;
    return;
}

void init()
{
    // FIXME: just for testing
    active_model = std::make_unique<GameEngine::Md2Model>("../../assets/models/hueteotl");
}

// FIXME: move this to a specialised Camera class
void render_camera()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45, (float)WinWidth / (float)WinHeight, 1, 1000);
    glRotatef(camera_rotation.x, 1, 0, 0);
    glRotatef(camera_rotation.y, 0, 1, 0);
    glRotatef(camera_rotation.z, 0, 0, 1);

    glTranslatef(camera_position.x, camera_position.y, camera_position.z);
}

void render()
{
    glViewport(0, 0, WinWidth, WinHeight);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_camera();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Needed or there's no depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // Not sure what this does
    // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    if (active_model) {
        active_model->render();
    }
}

void render_gui()
{
    ImGui::SetNextWindowSize(ImVec2(150, 400));
    ImGui::Begin("Properties", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    if (active_model) {
        ImGui::Text("Model: %s", active_model->name().c_str());

        ImGui::SeparatorText("Animations");
        static int item_selected_idx = 0;
        auto const& animations = active_model->animations();
        for (int n = 0; n < animations.size(); n++) {
            bool const is_selected = (item_selected_idx == n);
            if (ImGui::Selectable(animations[n].c_str(), is_selected))
                item_selected_idx = n;

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        active_model->set_animation_name(animations[item_selected_idx]);
    } else {
        ImGui::Text("No model loaded.");
    }

    ImGui::End();
}

void update(float delta_time)
{
    Uint8 const* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_RIGHT]) {
        camera_rotation.y += 1;
    }

    if (keys[SDL_SCANCODE_LEFT]) {
        camera_rotation.y -= 1;
    }

    float move_distance = 0;
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
        move_distance = 0.5;
    } else if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
        move_distance = -0.5;
    }

    if (move_distance != 0) {
        camera_position.x -= glm::sin(glm::radians(camera_rotation.y)) * move_distance;
        camera_position.z += glm::cos(glm::radians(camera_rotation.y)) * move_distance;
    }

    if (active_model) {
        active_model->update(delta_time);
    }
}

int main(int ArgCount, char** Args)
{
    u32 window_flags = SDL_WINDOW_OPENGL;
    SDL_Window* window = SDL_CreateWindow("OpenGL Test", 0, 0, WinWidth, WinHeight, window_flags);
    assert(window);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    SDL_Init(SDL_INIT_VIDEO);

    init_imgui(window, &context);

    // VSync
    SDL_GL_SetSwapInterval(1);

    // print_gl_info();

    engine_last_tick = SDL_GetTicks();

    init();

    bool running = 1;
    bool full_screen = 0;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = 0;
                    break;
                case 'f':
                    full_screen = !full_screen;
                    if (full_screen) {
                        SDL_SetWindowFullscreen(window, window_flags | SDL_WINDOW_FULLSCREEN_DESKTOP);
                    } else {
                        SDL_SetWindowFullscreen(window, window_flags);
                    }
                    break;
                default:
                    break;
                }
            } else if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        auto delta_time = (SDL_GetTicks() - engine_last_tick) / 1000.0f;

        engine_last_tick = SDL_GetTicks();

        imgui_start_frame();

        update(delta_time);

        render();

        render_gui();

        imgui_end_frame();

        SDL_GL_SwapWindow(window);
    }

    imgui_destroy();

    return 0;
}
