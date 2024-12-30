#ifndef SPEAR_MODEL_LOADER_HH
#define SPEAR_MODEL_LOADER_HH

#include <string>
#include <vector>

namespace spear
{

class ModelLoader
{
public:
    struct Vertex
    {
        float x, y, z;
    };

    struct TextureCoord
    {
        float u, v;
    };

    struct Normal
    {
        float x, y, z;
    };

    struct Face
    {
        std::vector<int> vertexIndices;
        std::vector<int> textureCoordIndices;
        std::vector<int> normalIndices;
    };

    struct Material
    {
        std::string name;
        // TODO
    };

    struct Animation
    {
        std::string name;
        // TODO
    };

    virtual ~ModelLoader() = default;

    /// Load model file.
    virtual bool load(const std::string& file_path, bool asset_path) = 0;

    // Generic accessors
    virtual const std::vector<Vertex>& getVertices() const
    {
        return m_vertices;
    }
    virtual const std::vector<Face>& getFaces() const
    {
        return m_faces;
    }
    virtual const std::vector<Material>& getMaterials() const
    {
        return m_materials;
    }
    virtual const std::vector<Animation>& getAnimations() const
    {
        return m_animations;
    }

protected:
    std::vector<Vertex> m_vertices;
    std::vector<Face> m_faces;
    std::vector<Material> m_materials;
    std::vector<Animation> m_animations;
};

} // namespace spear

#endif
