#include <iostream>
#include "Md2Model.hpp"
#include <gl/glew.h>
#include <SDL2/SDL.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>

// http://tfc.duke.free.fr/coding/md2-specs-en.html

struct FileHeader {
    int ident;   /* magic number: "IDP2" */
    int version; /* version: must be 8 */

    int texture_width;
    int texture_height;

    int frame_size; /* size in bytes of a frame */

    int skin_count;
    int vertex_count;
    int texture_coordinate_count;
    int triangle_count;
    int gl_command_count;
    int frame_count;

    int skins_offset;
    int texture_coordinates_offset;
    int triangles_offset;
    int frames_offset;
    int gl_command_offset;
    int end_offset;
};

struct FileVertex {
    unsigned char compressed_position[3]; /* compressed vertex */
    unsigned char normal_index;           /* index to a normal vector for the lighting */
};

struct FileTriangle {
    unsigned short vertex[3];
    unsigned short texture_coordinates[3];
};

namespace GameEngine::Md2 {
Model::Model(std::string const& path)
{
    std::ifstream file(path, std::ios::binary);

    FileHeader header;
    if (!file.read((char*)&header, sizeof(header))) {
        throw std::runtime_error("Failed to load: " + path);
    }

    glm::vec2 texture_coordinates[header.texture_coordinate_count];
    file.seekg(header.texture_coordinates_offset);
    file.read((char*)&texture_coordinates, sizeof(texture_coordinates));

    FileTriangle triangles[header.triangle_count];
    file.seekg(header.triangles_offset);
    if (!file.read((char*)&triangles, sizeof(triangles))) {
        throw std::runtime_error("Failed to load: " + path);
    }

    glm::vec2 texture_dimensions(header.texture_width, header.texture_height);
    m_texture_coordinates.resize(header.triangle_count);
    for (int triangle_index = 0; triangle_index < header.triangle_count; triangle_index++) {
        for (int triangle_vertex = 0; triangle_vertex < 3; triangle_vertex++) {
            // Note dimensions are normalised here to fit with GL's 0-1 mapping
            auto const vertex_coordinates = texture_coordinates[triangles[triangle_index].texture_coordinates[triangle_vertex]] / texture_dimensions;
            m_texture_coordinates[triangle_index].set_point(triangle_vertex, vertex_coordinates);
        }
    }

    file.seekg(header.frames_offset);
    std::string* current_frame_name = NULL;
    for (int frame_index = 0; frame_index < header.frame_count; frame_index++) {
        glm::vec3 frame_scale;
        glm::vec3 frame_translate;
        std::string frame_name(16, '\0');
        FileVertex frame_vertices[header.vertex_count];

        file.read((char*)&frame_scale, sizeof(frame_scale));
        file.read((char*)&frame_translate, sizeof(frame_translate));
        file.read(&frame_name[0], sizeof(char) * 16);
        file.read((char*)&frame_vertices, sizeof(frame_vertices));

        std::vector<Triangle<Vertex>> frame_faces(header.triangle_count);
        for (int triangle_index = 0; triangle_index < header.triangle_count; triangle_index++) {
            auto face_vertices = frame_faces[triangle_index];
            for (int triangle_vertex = 0; triangle_vertex < 3; triangle_vertex++) {
                auto const frame_vertex_index = triangles[triangle_index].vertex[triangle_vertex];
                auto const compressed_position = frame_vertices[frame_vertex_index].compressed_position;
                auto const vertex_position = glm::vec3(compressed_position[0], compressed_position[1], compressed_position[2]) * frame_scale + frame_translate;
                // FIXME: look up normal
                face_vertices.set_point(triangle_vertex, Vertex(vertex_position, glm::vec3()));
            }

            frame_faces.push_back(face_vertices);
        }

        // assuming name format is animation000
        // FIXME: handle bad name?
        int animation_name_end_pos = frame_name.find_first_of('\0') - 3;
        std::string animation_name = frame_name.substr(0, animation_name_end_pos);

        m_frames[animation_name].push_back(Frame(frame_faces));
    }
}

void Model::dump_info() const
{
    std::cout << "Hello";
}

// FIXME: move this info to the model and also think about consoldiating time tracking,
// maybe we want to pass a deltaTime to render?
uint32_t last_tick = -1;
float animation_progress = 0;
std::string animation_name = "stand";

void Model::render() const
{
    auto animation_frames = m_frames.at(animation_name);

    if (last_tick == -1) {
        last_tick = SDL_GetTicks();
    }
    auto seconds_elapsed = (SDL_GetTicks() - last_tick) / 1000.0f;
    animation_progress = std::fmod(animation_progress + (15.0f * seconds_elapsed), animation_frames.size());
    last_tick = SDL_GetTicks();

    int current_frame_index = floor(animation_progress);
    auto const& current_frame = animation_frames[current_frame_index];
    auto const& next_frame = animation_frames[(current_frame_index + 1) % animation_frames.size()];
    for (int triangle_index = 0; triangle_index < current_frame.faces().size(); triangle_index++) {
        auto const& current_face = current_frame.faces()[triangle_index];
        auto const& next_face = next_frame.faces()[triangle_index];
        auto const& face_texture_coordinates = m_texture_coordinates[triangle_index];

        glBegin(GL_TRIANGLES);
        for (int vertex_index = 0; vertex_index < 3; vertex_index++) {
            auto current_vertex = current_face.point(vertex_index);
            auto next_vertex = next_face.point(vertex_index);
            auto vertex_texture_coordinates = face_texture_coordinates.point(vertex_index);

            // FIXME: the texture coordinates don't change between frames, I've just superimposed them
            // on to the frames for "easy" access. Really indicating that this is a bit of an awkward structure and we should go
            // back to a looking them up from a central list by triangle / vertex index instead of putting them on each
            // frame of animation.
            glTexCoord2f(vertex_texture_coordinates.s, vertex_texture_coordinates.t);
            auto interpolated_position = glm::mix(
                current_vertex.position(),
                next_vertex.position(),
                std::fmod(animation_progress, 1));

            glVertex3f(interpolated_position.x, interpolated_position.z, interpolated_position.y);
            // FIXME: no normals yet, also need to interpolate normals
            // glNormal3f(vertex.normal().x, vertex.normal().y, vertex.normal().z);
        }
        glEnd();
    }
}

}
