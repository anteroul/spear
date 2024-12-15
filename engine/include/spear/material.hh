#ifndef SPEAR_MATERIAL_HH
#define SPEAR_MATERIAL_HH

#include <spear/rendering/opengl/shader.hh>
#include <spear/rendering/opengl/texture.hh>

#include <spear/rendering/base_shader.hh>
#include <spear/rendering/base_texture.hh>

#include <glm/vec3.hpp>

namespace spear
{

class Material
{
public:
    /// Constructor.
    // Material(std::shared_ptr<rendering::BaseShader> shader, std::shared_ptr<rendering::BaseTexture> texture);

    /*
    static std::shared_ptr<Material> create(std::shared_ptr<rendering::BaseShader> shader, std::shared_ptr<rendering::BaseTexture> texture)
    {
        return std::make_shared<std::shared_ptr<Material>>(shader, texture);
    }
    int32_t getShaderId() { return m_shader.getId(); }
    */

    const int32_t& getColor() const
    {
        return m_color;
    }
    const int32_t& getMvp() const
    {
        return m_mvp;
    }
    const int32_t& getSampler() const
    {
        return m_sampler;
    }

    void use();

private:
    int32_t m_mvp;
    int32_t m_color;
    int32_t m_sampler;
};

} // namespace spear

#endif
