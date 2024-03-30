#pragma once
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>

namespace GameEngine {

class Texture {
private:
    GLuint m_texture_id;

public:
    Texture(std::string const& path);
    ~Texture();
    void bind() const;
};

}
