#ifndef SPEAR_VULKAN_RENDERER_HH
#define SPEAR_VULKAN_RENDERER_HH

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>

namespace spear::rendering::vulkan
{

class Renderer
{
public:
    /// Constructor.
    Renderer(SDL_Window *window);

    // Destructor.
    ~Renderer();

    void render();

    void setViewPort(float width, float height)
    {
        m_viewport = {0.f, 0.f, width, height, -2.f, 2.f};
    }

    void init();

    static VkInstance* getInstance()
    {
        return m_instance;
    }
private:
    static VkInstance* m_instance;
    VkBuffer m_buffer;
    VkPipeline* m_pipeline;
    VkViewport m_viewport;
    SDL_Window* m_window;
    uint32_t m_shader_program;
};

}

#endif //SPEAR_VULKAN_RENDERER_HH
