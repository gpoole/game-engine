#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <glm/glm.hpp>

namespace GameEngine {

namespace Md2 {

class Vertex {
private:
    glm::vec3 m_position;
    glm::vec3 m_normal;

public:
    Vertex() {};
    Vertex(glm::vec3 position, glm::vec3 normal)
        : m_position(position)
        , m_normal(normal)
    {
    }
    ~Vertex() = default;
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
class Triangle {
private:
    std::array<T, 3> m_points;

public:
    Triangle() {};
    Triangle(std::array<T, 3> points)
        : m_points(points) {};
    ~Triangle() = default;

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

class Frame {
private:
    std::vector<Triangle<Vertex>> m_faces;

public:
    Frame(std::vector<Triangle<Vertex>> faces)
        : m_faces(faces) {};
    ~Frame() = default;

    std::vector<Triangle<Vertex>> const& faces() const
    {
        return m_faces;
    }
};

class Model {
private:
    std::map<std::string, std::vector<Frame>> m_frames;
    std::vector<Triangle<glm::vec2>> m_texture_coordinates;

public:
    Model(std::string const& path);
    ~Model() = default;

    void dump_info() const;
    void render() const;
};

}
}
