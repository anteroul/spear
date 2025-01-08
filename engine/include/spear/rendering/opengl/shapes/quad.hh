#ifndef SPEAR_RENDERING_OPENGL_SHAPES_QUAD_HH
#define SPEAR_RENDERING_OPENGL_SHAPES_QUAD_HH

#include <spear/rendering/opengl/shader.hh>
#include <spear/rendering/shapes/shape.hh>

#include <spear/physics/bullet/object_data.hh>

#include <vector>

namespace spear::rendering::opengl
{

class Quad : public Shape
{
public:
    /// Constructor.
    Quad(physics::bullet::ObjectData&& object_data, const glm::vec4& color = glm::vec4(1.f, 1.f, 1.f, 1.f));

    /// Destructor.
    ~Quad();

    void free();

    void initialize(const glm::vec3& position);

    /// Transform::translate implementation.
    void translate(const glm::vec3& position) override;

    /// Mesh::render implementation.
    void render(Camera& camera) override;

private:
    uint32_t m_vao, m_vbo, m_ebo;
    std::vector<float> m_vertices;
};

} // namespace spear::rendering::opengl

#endif
