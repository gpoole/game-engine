#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <glm/glm.hpp>
#include "core/Texture.hpp"

namespace GameEngine {

class Md2Vertex {
private:
    glm::vec3 m_position;
    glm::vec3 m_normal;

public:
    Md2Vertex() {};
    Md2Vertex(glm::vec3 position, glm::vec3 normal)
        : m_position(position)
        , m_normal(normal)
    {
    }
    ~Md2Vertex() = default;
    glm::vec3 const& position() const
    {
        return m_position;
    }
    glm::vec3 const& normal() const
    {
        return m_normal;
    }
};

template<typename T>
class Md2Triangle {
private:
    std::array<T, 3> m_points;

public:
    Md2Triangle() {};
    Md2Triangle(std::array<T, 3> points)
        : m_points(points) {};
    ~Md2Triangle() = default;

    T const& point(int index) const
    {
        if (index < 0 || index >= 3) {
            throw std::invalid_argument("invalid point index");
        }
        return m_points[index];
    }

    void set_point(int index, T point)
    {
        m_points[index] = point;
    }
};

class Md2Frame {
private:
    std::vector<Md2Triangle<Md2Vertex>> m_faces;

public:
    Md2Frame(std::vector<Md2Triangle<Md2Vertex>> faces)
        : m_faces(faces) {};
    ~Md2Frame() = default;

    std::vector<Md2Triangle<Md2Vertex>> const& faces() const
    {
        return m_faces;
    }
};

class Md2Model {
private:
    std::map<std::string, std::vector<Md2Frame>> m_frames;
    std::vector<std::string const> m_animations;
    std::vector<Md2Triangle<glm::vec2>> m_texture_coordinates;
    std::unique_ptr<Texture> m_texture;
    std::string m_name;
    float m_animation_frame = 0;
    std::string m_animation_name;

public:
    Md2Model(std::string const& path);
    ~Md2Model() = default;

    void render() const;
    void update(float delta_time);
    std::string const& name() const;
    std::vector<std::string const> const& animations() const;
    // int triangle_count() const;
    // int frame_count() const;
    std::string const& animation_name() const;
    void set_animation_name(std::string const&);
    float animation_frame() const;
};

}
