#include <string>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include "core/Texture.hpp"

namespace GameEngine {

Texture::Texture(std::string const& path)
{
    SDL_Surface* raw_data = IMG_Load(path.c_str());
    if (raw_data == NULL) {
        throw std::runtime_error("Failed to load texture " + path + ": " + SDL_GetError());
    }

    // Converting the raw format interpreted by IMG_Load into a standard format we can explain easily to glTexImage2D (SDL_PIXELFORMAT_RGB24 == GL_RGB).
    // Presumably would be more efficient to just detect the format that SDL has figured out and tell that to glTexImage2D rather than double-handling it
    // (as per https://stackoverflow.com/a/4340330) but I think there are more variations than what that code covers and this is simpler to write and read.
    SDL_Surface* converted_data = SDL_ConvertSurfaceFormat(raw_data, SDL_PIXELFORMAT_RGB24, 0);
    if (converted_data == NULL) {
        SDL_FreeSurface(raw_data);
        throw std::runtime_error("Failed to convert texture " + path + ": " + SDL_GetError());
    }

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &m_texture_id);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, converted_data->w, converted_data->h, 0, GL_RGB, GL_UNSIGNED_BYTE, converted_data->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(raw_data);
    SDL_FreeSurface(converted_data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_texture_id);
}

void Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
}

}
