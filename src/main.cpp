#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <gl/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include "Md2Model.hpp"

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
GLuint crate_texture;

void render_cube()
{
    glColor3f(255, 255, 0); // Yellow

    glPushMatrix();

    cube_rotation += 0.1;

    glTranslatef(0, 0, -5);
    glRotatef(cube_rotation, 0, 1, 0);

    glBindTexture(GL_TEXTURE_2D, crate_texture);

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

    render_cube();
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
        move_distance = 0.1;
    } else if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
        move_distance = -0.1;
    }

    if (move_distance != 0) {
        camera_position.x -= glm::sin(glm::radians(camera_rotation.y)) * move_distance;
        camera_position.z += glm::cos(glm::radians(camera_rotation.y)) * move_distance;
    }
}

bool load_assets()
{
    SDL_Surface* crate_image = IMG_Load("assets/textures/crate.bmp");
    if (crate_image == NULL) {
        std::cout << "Error: " << SDL_GetError() << std::endl;
        return false;
    }
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &crate_texture);
    glBindTexture(GL_TEXTURE_2D, crate_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, crate_image->w, crate_image->h, 0, GL_BGR, GL_UNSIGNED_BYTE, crate_image->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SDL_FreeSurface(crate_image);

    auto model = GameEngine::Md2Model::load("assets/models/hueteotl/tris.md2");
    model.dump_info();

    return true;
}

int main(int ArgCount, char** Args)
{
    u32 WindowFlags = SDL_WINDOW_OPENGL;
    SDL_Window* Window = SDL_CreateWindow("OpenGL Test", 0, 0, WinWidth, WinHeight, WindowFlags);
    assert(Window);
    SDL_GLContext Context = SDL_GL_CreateContext(Window);

    SDL_Init(SDL_INIT_VIDEO);

    // VSync
    SDL_GL_SetSwapInterval(1);

    print_gl_info();

    if (!load_assets()) {
        std::cout << "Error loading assets" << std::endl;
        return 1;
    }

    bool Running = 1;
    bool FullScreen = 0;
    while (Running) {
        SDL_Event Event;
        while (SDL_PollEvent(&Event)) {
            if (Event.type == SDL_KEYDOWN) {
                switch (Event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    Running = 0;
                    break;
                case 'f':
                    FullScreen = !FullScreen;
                    if (FullScreen) {
                        SDL_SetWindowFullscreen(Window, WindowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);
                    } else {
                        SDL_SetWindowFullscreen(Window, WindowFlags);
                    }
                    break;
                default:
                    break;
                }
            } else if (Event.type == SDL_QUIT) {
                Running = 0;
            }
        }

        update();

        render();

        SDL_GL_SwapWindow(Window);
    }
    return 0;
}
