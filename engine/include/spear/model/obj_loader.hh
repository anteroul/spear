#ifndef SPEAR_OBJ_LOADER_HH
#define SPEAR_OBJ_LOADER_HH

#include <spear/model/model_loader.hh>

namespace spear
{

class OBJLoader : public ModelLoader
{
public:
    bool load(const std::string& obj_file_path, const std::string& material_file_path = "", bool asset_path = true) override;

    const std::vector<ModelLoader::TextureCoord>& getUvs()
    {
        return m_textureCoords;
    }
    const std::vector<ModelLoader::Normal>& getNormals()
    {
        return m_normals;
    }
    const ModelLoader::Material& getMaterial() const
    {
        return m_material;
    }

private:
    bool loadMaterial(const std::string& mtlFilePath);

private:
    std::vector<ModelLoader::TextureCoord> m_textureCoords;
    std::vector<ModelLoader::Normal> m_normals;
    ModelLoader::Material m_material;
};

} // namespace spear

#endif
