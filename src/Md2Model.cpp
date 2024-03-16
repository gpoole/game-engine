#include <iostream>
#include "Md2Model.hpp"
#include <gl/glew.h>
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
    if (!file.read(reinterpret_cast<char*>(&header), sizeof(header))) {
        throw std::runtime_error("Failed to load: " + path);
    }

    glm::vec2 texture_coordinates[header.texture_coordinate_count];
    file.seekg(header.texture_coordinates_offset);
    file.read(reinterpret_cast<char*>(&texture_coordinates), sizeof(texture_coordinates));

    FileTriangle triangles[header.triangle_count];
    file.seekg(header.triangles_offset);
    if (!file.read(reinterpret_cast<char*>(&triangles), sizeof(triangles))) {
        throw std::runtime_error("Failed to load: " + path);
    }

    glm::vec2 texture_dimensions(header.texture_width, header.texture_height);
    file.seekg(header.frames_offset);
    for (int frame_index = 0; frame_index < header.frame_count; frame_index++) {
        glm::vec3 frame_scale;
        glm::vec3 frame_translate;
        char frame_name[16];
        FileVertex frame_vertices[header.vertex_count];

        file.read(reinterpret_cast<char*>(&frame_scale), sizeof(frame_scale));
        file.read(reinterpret_cast<char*>(&frame_translate), sizeof(frame_translate));
        file.read(reinterpret_cast<char*>(&frame_name), sizeof(frame_name));
        file.read(reinterpret_cast<char*>(&frame_vertices), sizeof(frame_vertices));

        std::vector<Face> frame_faces;
        for (int triangle_index = 0; triangle_index < header.triangle_count; triangle_index++) {
            std::array<Vertex, 3> face_vertices;
            for (int triangle_vertex = 0; triangle_vertex < 3; triangle_vertex++) {
                auto const frame_vertex_index = triangles[triangle_index].vertex[triangle_vertex];
                auto const compressed_position = frame_vertices[frame_vertex_index].compressed_position;
                auto const vertex_texture_coordinates = texture_coordinates[triangles[triangle_index].texture_coordinates[triangle_vertex]] / texture_dimensions;
                auto const vertex_position = glm::vec3(compressed_position[0], compressed_position[1], compressed_position[2]) * frame_scale + frame_translate;
                face_vertices[triangle_vertex] = Vertex(
                    vertex_position,
                    // FIXME: look up normal
                    glm::vec3(),
                    vertex_texture_coordinates);
            }

            frame_faces.push_back(Face(face_vertices));
        }

        m_frames.push_back(Frame(std::string(frame_name), frame_faces));
    }
}

void Model::dump_info() const
{
    std::cout << "Hello";
}

void Model::render() const
{
    auto const& frame = m_frames[0];
    for (auto const& face : frame.faces()) {
        glBegin(GL_TRIANGLES);
        for (int vertex_index = 0; vertex_index < 3; vertex_index++) {
            auto vertex = face.vertex(vertex_index);
            auto texture_coordinates = vertex.texture_coordinates();
            auto position = vertex.position();
            auto normal = vertex.normal();
            glTexCoord2f(texture_coordinates.s, texture_coordinates.t);
            glVertex3f(position.x, position.z, position.y);
            // FIXME: no normals yet
            // glNormal3f(normal.x, normal.y, normal.z);
        }
        glEnd();
    }
}

}
