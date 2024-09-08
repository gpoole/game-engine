#include <iostream>
#include <filesystem>
#include "Md2Model.hpp"
#include <SDL2/SDL_opengl.h>
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

    int frame_size;

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

struct FileSkin {
    char name[64];
};

// Original Quake 2 precomputed vertex normals as per
// https://github.com/id-Software/Quake-2/blob/372afde46e7defc9dd2d719a1732b8ace1fa096e/ref_gl/anorms.h
#define Q2_MD2_NUM_VERTEX_NORMALS 162
float Q2VertexNormals[Q2_MD2_NUM_VERTEX_NORMALS][3] = {
    { -0.525731f, 0.000000f, 0.850651f },
    { -0.442863f, 0.238856f, 0.864188f },
    { -0.295242f, 0.000000f, 0.955423f },
    { -0.309017f, 0.500000f, 0.809017f },
    { -0.162460f, 0.262866f, 0.951056f },
    { 0.000000f, 0.000000f, 1.000000f },
    { 0.000000f, 0.850651f, 0.525731f },
    { -0.147621f, 0.716567f, 0.681718f },
    { 0.147621f, 0.716567f, 0.681718f },
    { 0.000000f, 0.525731f, 0.850651f },
    { 0.309017f, 0.500000f, 0.809017f },
    { 0.525731f, 0.000000f, 0.850651f },
    { 0.295242f, 0.000000f, 0.955423f },
    { 0.442863f, 0.238856f, 0.864188f },
    { 0.162460f, 0.262866f, 0.951056f },
    { -0.681718f, 0.147621f, 0.716567f },
    { -0.809017f, 0.309017f, 0.500000f },
    { -0.587785f, 0.425325f, 0.688191f },
    { -0.850651f, 0.525731f, 0.000000f },
    { -0.864188f, 0.442863f, 0.238856f },
    { -0.716567f, 0.681718f, 0.147621f },
    { -0.688191f, 0.587785f, 0.425325f },
    { -0.500000f, 0.809017f, 0.309017f },
    { -0.238856f, 0.864188f, 0.442863f },
    { -0.425325f, 0.688191f, 0.587785f },
    { -0.716567f, 0.681718f, -0.147621f },
    { -0.500000f, 0.809017f, -0.309017f },
    { -0.525731f, 0.850651f, 0.000000f },
    { 0.000000f, 0.850651f, -0.525731f },
    { -0.238856f, 0.864188f, -0.442863f },
    { 0.000000f, 0.955423f, -0.295242f },
    { -0.262866f, 0.951056f, -0.162460f },
    { 0.000000f, 1.000000f, 0.000000f },
    { 0.000000f, 0.955423f, 0.295242f },
    { -0.262866f, 0.951056f, 0.162460f },
    { 0.238856f, 0.864188f, 0.442863f },
    { 0.262866f, 0.951056f, 0.162460f },
    { 0.500000f, 0.809017f, 0.309017f },
    { 0.238856f, 0.864188f, -0.442863f },
    { 0.262866f, 0.951056f, -0.162460f },
    { 0.500000f, 0.809017f, -0.309017f },
    { 0.850651f, 0.525731f, 0.000000f },
    { 0.716567f, 0.681718f, 0.147621f },
    { 0.716567f, 0.681718f, -0.147621f },
    { 0.525731f, 0.850651f, 0.000000f },
    { 0.425325f, 0.688191f, 0.587785f },
    { 0.864188f, 0.442863f, 0.238856f },
    { 0.688191f, 0.587785f, 0.425325f },
    { 0.809017f, 0.309017f, 0.500000f },
    { 0.681718f, 0.147621f, 0.716567f },
    { 0.587785f, 0.425325f, 0.688191f },
    { 0.955423f, 0.295242f, 0.000000f },
    { 1.000000f, 0.000000f, 0.000000f },
    { 0.951056f, 0.162460f, 0.262866f },
    { 0.850651f, -0.525731f, 0.000000f },
    { 0.955423f, -0.295242f, 0.000000f },
    { 0.864188f, -0.442863f, 0.238856f },
    { 0.951056f, -0.162460f, 0.262866f },
    { 0.809017f, -0.309017f, 0.500000f },
    { 0.681718f, -0.147621f, 0.716567f },
    { 0.850651f, 0.000000f, 0.525731f },
    { 0.864188f, 0.442863f, -0.238856f },
    { 0.809017f, 0.309017f, -0.500000f },
    { 0.951056f, 0.162460f, -0.262866f },
    { 0.525731f, 0.000000f, -0.850651f },
    { 0.681718f, 0.147621f, -0.716567f },
    { 0.681718f, -0.147621f, -0.716567f },
    { 0.850651f, 0.000000f, -0.525731f },
    { 0.809017f, -0.309017f, -0.500000f },
    { 0.864188f, -0.442863f, -0.238856f },
    { 0.951056f, -0.162460f, -0.262866f },
    { 0.147621f, 0.716567f, -0.681718f },
    { 0.309017f, 0.500000f, -0.809017f },
    { 0.425325f, 0.688191f, -0.587785f },
    { 0.442863f, 0.238856f, -0.864188f },
    { 0.587785f, 0.425325f, -0.688191f },
    { 0.688191f, 0.587785f, -0.425325f },
    { -0.147621f, 0.716567f, -0.681718f },
    { -0.309017f, 0.500000f, -0.809017f },
    { 0.000000f, 0.525731f, -0.850651f },
    { -0.525731f, 0.000000f, -0.850651f },
    { -0.442863f, 0.238856f, -0.864188f },
    { -0.295242f, 0.000000f, -0.955423f },
    { -0.162460f, 0.262866f, -0.951056f },
    { 0.000000f, 0.000000f, -1.000000f },
    { 0.295242f, 0.000000f, -0.955423f },
    { 0.162460f, 0.262866f, -0.951056f },
    { -0.442863f, -0.238856f, -0.864188f },
    { -0.309017f, -0.500000f, -0.809017f },
    { -0.162460f, -0.262866f, -0.951056f },
    { 0.000000f, -0.850651f, -0.525731f },
    { -0.147621f, -0.716567f, -0.681718f },
    { 0.147621f, -0.716567f, -0.681718f },
    { 0.000000f, -0.525731f, -0.850651f },
    { 0.309017f, -0.500000f, -0.809017f },
    { 0.442863f, -0.238856f, -0.864188f },
    { 0.162460f, -0.262866f, -0.951056f },
    { 0.238856f, -0.864188f, -0.442863f },
    { 0.500000f, -0.809017f, -0.309017f },
    { 0.425325f, -0.688191f, -0.587785f },
    { 0.716567f, -0.681718f, -0.147621f },
    { 0.688191f, -0.587785f, -0.425325f },
    { 0.587785f, -0.425325f, -0.688191f },
    { 0.000000f, -0.955423f, -0.295242f },
    { 0.000000f, -1.000000f, 0.000000f },
    { 0.262866f, -0.951056f, -0.162460f },
    { 0.000000f, -0.850651f, 0.525731f },
    { 0.000000f, -0.955423f, 0.295242f },
    { 0.238856f, -0.864188f, 0.442863f },
    { 0.262866f, -0.951056f, 0.162460f },
    { 0.500000f, -0.809017f, 0.309017f },
    { 0.716567f, -0.681718f, 0.147621f },
    { 0.525731f, -0.850651f, 0.000000f },
    { -0.238856f, -0.864188f, -0.442863f },
    { -0.500000f, -0.809017f, -0.309017f },
    { -0.262866f, -0.951056f, -0.162460f },
    { -0.850651f, -0.525731f, 0.000000f },
    { -0.716567f, -0.681718f, -0.147621f },
    { -0.716567f, -0.681718f, 0.147621f },
    { -0.525731f, -0.850651f, 0.000000f },
    { -0.500000f, -0.809017f, 0.309017f },
    { -0.238856f, -0.864188f, 0.442863f },
    { -0.262866f, -0.951056f, 0.162460f },
    { -0.864188f, -0.442863f, 0.238856f },
    { -0.809017f, -0.309017f, 0.500000f },
    { -0.688191f, -0.587785f, 0.425325f },
    { -0.681718f, -0.147621f, 0.716567f },
    { -0.442863f, -0.238856f, 0.864188f },
    { -0.587785f, -0.425325f, 0.688191f },
    { -0.309017f, -0.500000f, 0.809017f },
    { -0.147621f, -0.716567f, 0.681718f },
    { -0.425325f, -0.688191f, 0.587785f },
    { -0.162460f, -0.262866f, 0.951056f },
    { 0.442863f, -0.238856f, 0.864188f },
    { 0.162460f, -0.262866f, 0.951056f },
    { 0.309017f, -0.500000f, 0.809017f },
    { 0.147621f, -0.716567f, 0.681718f },
    { 0.000000f, -0.525731f, 0.850651f },
    { 0.425325f, -0.688191f, 0.587785f },
    { 0.587785f, -0.425325f, 0.688191f },
    { 0.688191f, -0.587785f, 0.425325f },
    { -0.955423f, 0.295242f, 0.000000f },
    { -0.951056f, 0.162460f, 0.262866f },
    { -1.000000f, 0.000000f, 0.000000f },
    { -0.850651f, 0.000000f, 0.525731f },
    { -0.955423f, -0.295242f, 0.000000f },
    { -0.951056f, -0.162460f, 0.262866f },
    { -0.864188f, 0.442863f, -0.238856f },
    { -0.951056f, 0.162460f, -0.262866f },
    { -0.809017f, 0.309017f, -0.500000f },
    { -0.864188f, -0.442863f, -0.238856f },
    { -0.951056f, -0.162460f, -0.262866f },
    { -0.809017f, -0.309017f, -0.500000f },
    { -0.681718f, 0.147621f, -0.716567f },
    { -0.681718f, -0.147621f, -0.716567f },
    { -0.850651f, 0.000000f, -0.525731f },
    { -0.688191f, 0.587785f, -0.425325f },
    { -0.587785f, 0.425325f, -0.688191f },
    { -0.425325f, 0.688191f, -0.587785f },
    { -0.425325f, -0.688191f, -0.587785f },
    { -0.587785f, -0.425325f, -0.688191f },
    { -0.688191f, -0.587785f, -0.425325f }
};

namespace GameEngine {

Md2Model::Md2Model(std::string const& path_str)
{
    // There is a convention for the way md2 model file sets are arranged
    // http://wiki.polycount.com/wiki/Quake_2
    std::filesystem::path path(path_str);

    std::filesystem::path tris_path = path / "tris.md2";

    m_name = path.filename();

    std::filesystem::path texture_path = path / (m_name + ".pcx");

    std::ifstream file(tris_path, std::ios::binary);

    FileHeader header;
    if (!file.read((char*)&header, sizeof(header))) {
        throw std::runtime_error("Failed to load: " + path_str);
    }

    // TODO: add any kind of integrity checking at all

    // uses short type for the s and t fields in the header
    glm::i16vec2 texture_coordinates[header.texture_coordinate_count];
    file.seekg(header.texture_coordinates_offset);
    file.read((char*)&texture_coordinates, sizeof(texture_coordinates));

    FileTriangle triangles[header.triangle_count];
    file.seekg(header.triangles_offset);
    if (!file.read((char*)&triangles, sizeof(triangles))) {
        throw std::runtime_error("Failed to load: " + path_str);
    }

    // FIXME: test model didn't have skins, not supporting this for now
    // FileSkin skins[header.skin_count];
    // file.seekg(header.skins_offset);
    // if (!file.read((char*)&skins, sizeof(skins))) {
    //     throw std::runtime_error("Failed to load: " + path);
    // }

    m_texture_coordinates.resize(header.triangle_count);
    for (int triangle_index = 0; triangle_index < header.triangle_count; triangle_index++) {
        for (int triangle_vertex = 0; triangle_vertex < 3; triangle_vertex++) {
            auto const vertex_coordinates = texture_coordinates[triangles[triangle_index].texture_coordinates[triangle_vertex]];
            m_texture_coordinates[triangle_index].set_point(
                triangle_vertex,
                glm::vec2(
                    // Note dimensions are normalised here to fit with GL's 0-1 mapping
                    (float)vertex_coordinates.s / (float)header.texture_width,
                    (float)vertex_coordinates.t / (float)header.texture_height));
        }
    }

    file.seekg(header.frames_offset);
    for (int frame_index = 0; frame_index < header.frame_count; frame_index++) {
        glm::vec3 frame_scale;
        glm::vec3 frame_translate;
        std::string frame_name(16, '\0');
        FileVertex frame_vertices[header.vertex_count];

        file.read((char*)&frame_scale, sizeof(frame_scale));
        file.read((char*)&frame_translate, sizeof(frame_translate));
        file.read(&frame_name[0], sizeof(char) * 16);
        file.read((char*)&frame_vertices, sizeof(frame_vertices));

        std::vector<Md2Triangle<Md2Vertex>> frame_faces(header.triangle_count);
        for (int triangle_index = 0; triangle_index < header.triangle_count; triangle_index++) {
            auto face_vertices = frame_faces[triangle_index];
            for (int triangle_vertex = 0; triangle_vertex < 3; triangle_vertex++) {
                auto const frame_vertex_index = triangles[triangle_index].vertex[triangle_vertex];
                auto const compressed_position = frame_vertices[frame_vertex_index].compressed_position;
                auto const vertex_position = glm::vec3(compressed_position[0], compressed_position[1], compressed_position[2]) * frame_scale + frame_translate;
                // FIXME: bounds check this lookup with Q2_MD2_NUM_VERTEX_NORMALS
                auto const vertex_normals = Q2VertexNormals[frame_vertices[frame_vertex_index].normal_index];
                face_vertices.set_point(triangle_vertex, Md2Vertex(vertex_position, glm::vec3(vertex_normals[0], vertex_normals[1], vertex_normals[2])));
            }

            frame_faces[triangle_index] = face_vertices;
        }

        // assuming name format is animation000
        // FIXME: handle bad name?
        int animation_name_end_pos = frame_name.find_first_of('\0') - 3;
        std::string animation_name = frame_name.substr(0, animation_name_end_pos);

        m_frames[animation_name].push_back(Md2Frame(frame_faces));
    }

    // Make a list of all the animation names
    for (auto const& [key, _] : m_frames) {
        m_animations.push_back(key);
    }

    // FIXME: just loading the default skin but obviously would be nice to allow loading any available
    // skins as requested by the user
    m_texture = std::make_unique<Texture>(texture_path);

    // Just default to whatever the first animation is
    m_animation_name = m_animations[0];
}

std::string const& Md2Model::name() const
{
    return m_name;
}

std::vector<std::string const> const& Md2Model::animations() const
{
    return m_animations;
}

std::string const& Md2Model::animation_name() const
{
    return m_animation_name;
}

void Md2Model::set_animation_name(std::string const& name)
{
    m_animation_name = name;
}

float Md2Model::animation_frame() const
{
    return m_animation_frame;
}

void Md2Model::update(float delta_time)
{
    auto animation_frames = m_frames.at(m_animation_name);
    m_animation_frame = std::fmod(m_animation_frame + (15.0f * delta_time), animation_frames.size());
}

void Md2Model::render() const
{
    m_texture->bind();

    auto animation_frames = m_frames.at(m_animation_name);
    int current_frame_index = floor(m_animation_frame);
    auto const& current_frame = animation_frames[current_frame_index];
    auto const& next_frame = animation_frames[(current_frame_index + 1) % animation_frames.size()];

    for (int triangle_index = 0; triangle_index < current_frame.faces().size(); triangle_index++) {
        auto const& current_face = current_frame.faces()[triangle_index];
        auto const& next_face = next_frame.faces()[triangle_index];
        auto const& face_texture_coordinates = m_texture_coordinates[triangle_index];

        glBegin(GL_TRIANGLES);
        for (int triangle_vertex = 0; triangle_vertex < 3; triangle_vertex++) {
            auto const& current_vertex = current_face.point(triangle_vertex);
            auto const& next_vertex = next_face.point(triangle_vertex);
            auto const& vertex_texture_coordinates = face_texture_coordinates.point(triangle_vertex);
            // animation_progress is a fractional frame counter, so we interpolate between this frame and the next frame using
            // the fractional component of it
            auto interpolation_amount = std::fmod(m_animation_frame, 1);

            glTexCoord2f(vertex_texture_coordinates.s, vertex_texture_coordinates.t);

            auto interpolated_position = glm::mix(
                current_vertex.position(),
                next_vertex.position(),
                interpolation_amount);
            auto interpolated_normal = glm::mix(
                current_vertex.normal(),
                next_vertex.normal(),
                interpolation_amount);

            glVertex3f(interpolated_position.x, interpolated_position.z, interpolated_position.y);
            glNormal3f(interpolated_normal.x, interpolated_normal.y, interpolated_normal.z);
        }
        glEnd();
    }
}
}
