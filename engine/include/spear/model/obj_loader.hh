#ifndef SPEAR_OBJ_LOADER_HH
#define SPEAR_OBJ_LOADER_HH

#include <spear/model/model_loader.hh>

namespace spear
{

class OBJLoader : public ModelLoader
{
public:
    bool load(const std::string& file_path, bool asset_path = true) override;

private:
    std::vector<ModelLoader::TextureCoord> m_textureCoords;
    std::vector<ModelLoader::Normal> m_normals;
};

} // namespace spear

#endif
