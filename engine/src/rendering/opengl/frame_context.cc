#include <spear/rendering/opengl/frame_context.hh>

namespace spear::rendering::opengl
{

FrameContext g_frameContext;

bool beginShadowDraw(const glm::mat4& model)
{
    if (!g_frameContext.shadowPass || !g_frameContext.shadowShader)
        return false;
    g_frameContext.shadowShader->use();
    g_frameContext.shadowShader->setMat4("mvp", g_frameContext.lightViewProjection * model);
    return true;
}

} // namespace spear::rendering::opengl
