#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <gl/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include "core/Md2Model.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"

typedef int32_t i32;
typedef uint32_t u32;
typedef int32_t b32;

#define WinWidth 1000
#define WinHeight 1000

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

glm::vec3 camera_position = glm::vec3(0, 0, 0);
glm::vec3 camera_rotation = glm::vec3(0, 0, 0);
GameEngine::Md2Model const* test_model;

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

float cube_rotation = 0;
GameEngine::Texture const* crate_texture;

void render_cube()
{
    // glColor3f(255, 255, 255); // Yellow

    glPushMatrix();

    cube_rotation += 0.1;

    glTranslatef(0, 0, -5);
    glRotatef(cube_rotation, 0, 1, 0);

    crate_texture->bind();

    glBegin(GL_QUADS);

    // Front face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1, -1, 1);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1, -1, 1);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1, 1, 1);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1, 1, 1);

    // Right face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1, -1, 1);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1, -1, -1);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1, 1, -1);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1, 1, 1);

    // Back face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1, -1, -1);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1, -1, -1);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1, 1, -1);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1, 1, -1);

    // Right face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1, -1, -1);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1, -1, 1);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1, 1, 1);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1, 1, -1);

    // Bottom face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1, -1, -1);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1, -1, -1);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1, -1, 1);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1, -1, 1);

    // Top face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1, 1, -1);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1, 1, -1);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1, 1, 1);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1, 1, 1);

    glEnd();

    glPopMatrix();
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

    // render_cube();
    test_model->render();
}

void update()
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
}

bool load_assets()
{
    test_model = new GameEngine::Md2Model("../../assets/models/hueteotl");
    // crate_texture = new GameEngine::Texture("../../assets/textures/crate.bmp");
    // crate_texture = new GameEngine::Texture("../../assets/models/hueteotl/hueteotl.pcx");

    return true;
}

void init_imgui(SDL_Window* window, SDL_GLContext* gl_context)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();
}

void imgui_start_frame()
{
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    // For testing
    // ImGui::ShowDemoWindow();
}

void imgui_end_frame()
{
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void imgui_destroy()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
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

    print_gl_info();

    if (!load_assets()) {
        std::cout << "Error loading assets" << std::endl;
        return 1;
    }

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

        imgui_start_frame();

        update();

        render();

        imgui_end_frame();

        SDL_GL_SwapWindow(window);
    }

    imgui_destroy();

    return 0;
}
