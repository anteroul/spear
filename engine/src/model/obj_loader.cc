#include <spear/model/obj_loader.hh>
#include <spear/spear_root.hh>

#include <fstream>
#include <iostream>
#include <sstream>

namespace spear
{

bool OBJLoader::load(const std::string& file_path, bool asset_path)
{
    std::string path = asset_path ? getAssetPath(file_path) : file_path;

    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream stream(line);
        std::string prefix;
        stream >> prefix;

        if (prefix == "v")
        {
            // Vertex
            Vertex vertex;
            stream >> vertex.x >> vertex.y >> vertex.z;
            m_vertices.push_back(vertex);
        }
        else if (prefix == "vt")
        {
            // Texture Coordinate
            TextureCoord texCoord;
            stream >> texCoord.u >> texCoord.v;
            m_textureCoords.push_back(texCoord);
        }
        else if (prefix == "vn")
        {
            // Normal
            Normal normal;
            stream >> normal.x >> normal.y >> normal.z;
            m_normals.push_back(normal);
        }
        else if (prefix == "f")
        {
            // Face
            Face face;
            std::string vertexData;
            while (stream >> vertexData)
            {
                std::istringstream vertexStream(vertexData);
                std::string index;
                std::vector<int> indices;
                while (std::getline(vertexStream, index, '/'))
                {
                    if (!index.empty())
                        indices.push_back(std::stoi(index) - 1); // OBJ is 1-based
                }
                face.vertexIndices.push_back(indices[0]);
                if (indices.size() > 1)
                    face.textureCoordIndices.push_back(indices[1]);
                if (indices.size() > 2)
                    face.normalIndices.push_back(indices[2]);
            }
            m_faces.push_back(face);
        }
    }

    std::cout << "Loaded OBJ file: " << path << " with " << m_vertices.size() << " vertices." << std::endl;
    return true;
}

} // namespace spear
