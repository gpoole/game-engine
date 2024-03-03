#include <iostream>
#include "Md2Model.hpp"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

// http://tfc.duke.free.fr/coding/md2-specs-en.html

struct Md2Header {
    int ident;   /* magic number: "IDP2" */
    int version; /* version: must be 8 */

    int skinwidth;  /* texture width */
    int skinheight; /* texture height */

    int framesize; /* size in bytes of a frame */

    int num_skins;    /* number of skins */
    int num_vertices; /* number of vertices per frame */
    int num_st;       /* number of texture coordinates */
    int num_tris;     /* number of triangles */
    int num_glcmds;   /* number of opengl commands */
    int num_frames;   /* number of frames */

    int offset_skins;  /* offset skin data */
    int offset_st;     /* offset texture coordinate data */
    int offset_tris;   /* offset triangle data */
    int offset_frames; /* offset frame data */
    int offset_glcmds; /* offset OpenGL command data */
    int offset_end;    /* offset end of file */
};

struct Md2Vertex {
    unsigned char compressed_position[3]; /* compressed vertex */
    unsigned char normal_index;           /* index to a normal vector for the lighting */
};

/* Model frame */
struct Md2Frame {
    glm::vec3 scale;     /* scale factor */
    glm::vec3 translate; /* translation vector */
    char name[16];       /* frame name */
    Md2Vertex* vertices; /* list of frame's vertices */
};

namespace GameEngine {

Md2Model::Md2Model()
{
}

Md2Model Md2Model::load(std::string const& path)
{
    auto file = SDL_RWFromFile(path.c_str(), "rb");
    if (file == nullptr) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    Md2Header header;
    if (SDL_RWread(file, &header, sizeof(Md2Header), 1) != 1) {
        SDL_RWclose(file);
        throw std::runtime_error("Failed to read header from file: " + path);
    }

    Md2Frame frames[header.num_frames];

    SDL_RWclose(file);
    // return Md2Model();
}

void Md2Model::dump_info()
{
    std::cout << "Hello";
}

}
