#include <string>
#include <vector>
#include <tuple>
#include <glm/glm.hpp>

namespace GameEngine {

namespace Md2 {

class Vertex {
private:
    glm::vec3 m_position;
    glm::vec2 m_texture_coordinates;
    glm::vec3 m_normal;

public:
    Vertex() {};
    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texture_coordinates)
        : m_position(position)
        , m_normal(normal)
        , m_texture_coordinates(texture_coordinates)
    {
    }
    ~Vertex() = default;
    glm::vec3 const& position() const
    {
        return m_position;
    }
    glm::vec2 const& texture_coordinates() const
    {
        return m_texture_coordinates;
    }
    glm::vec3 const& normal() const
    {
        return m_normal;
    }
};

class Face {
private:
    std::array<Vertex, 3> m_vertices;

public:
    Face(std::array<Vertex, 3> vertices)
        : m_vertices(vertices) {};
    ~Face() = default;

    Vertex const& vertex(int index) const
    {
        return m_vertices[index];
    }
};

class Frame {
private:
    std::string m_name;
    std::vector<Face> m_faces;

public:
    Frame(std::string name, std::vector<Face> faces)
        : m_name(name)
        , m_faces(faces) {};
    ~Frame() = default;

    std::string const name() const
    {
        return m_name;
    }
    std::vector<Face> const& faces() const
    {
        return m_faces;
    }
};

class Model {
private:
    std::vector<Frame> m_frames;

public:
    Model(std::string const& path);
    ~Model() = default;

    void dump_info() const;
    void render() const;
};

}
}
