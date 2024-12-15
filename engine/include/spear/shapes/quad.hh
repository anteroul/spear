#ifndef SPEAR_CUBE2_HH
#define SPEAR_CUBE2_HH

#include <spear/rendering/opengl/shader.hh>
#include <spear/shapes/shape.hh>

#include <vector>

namespace spear
{

class Quad : public Shape
{
public:
    /// Constructor.
    Quad(const glm::vec3& color);

    /// Destructor.
    ~Quad();

    void free();

    void initialize(const glm::vec3& position);

    /// Transform::translate implementation.
    void translate(const glm::vec3& position) override;

    /// Mesh::render implementation.
    void render(Camera& camera) override;

private:
    glm::vec3 m_color;
    uint32_t m_vao, m_vbo, m_ebo;
    std::vector<float> m_vertices;
};

} // namespace spear

#endif
