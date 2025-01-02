#include <spear/model/obj_loader.hh>
#include <spear/spear_root.hh>

#include <fstream>
#include <iostream>
#include <sstream>

namespace spear
{

bool OBJLoader::load(const std::string& filePath, bool asset_path)
{
    std::string path = asset_path ? getAssetPath(filePath) : filePath;
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open OBJ file: " << filePath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream lineStream(line);
        std::string prefix;
        lineStream >> prefix;

        if (prefix == "v")
        {
            Vertex vertex;
            lineStream >> vertex.x >> vertex.y >> vertex.z;
            m_vertices.push_back(vertex);
        }
        else if (prefix == "vt")
        {
            TextureCoord texCoord;
            lineStream >> texCoord.u >> texCoord.v;
            m_textureCoords.push_back(texCoord);
        }
        else if (prefix == "vn")
        {
            Normal normal;
            lineStream >> normal.x >> normal.y >> normal.z;
            m_normals.push_back(normal);
        }
        else if (prefix == "f")
        {
            Face face;
            std::string vertexData;
            while (lineStream >> vertexData)
            {
                std::istringstream vertexStream(vertexData);
                std::string vIndex, tIndex, nIndex;

                std::getline(vertexStream, vIndex, '/');
                std::getline(vertexStream, tIndex, '/');
                std::getline(vertexStream, nIndex, '/');

                if (!vIndex.empty())
                    face.vertexIndices.push_back(std::stoi(vIndex) - 1);
                if (!tIndex.empty())
                    face.textureCoordIndices.push_back(std::stoi(tIndex) - 1);
                if (!nIndex.empty())
                    face.normalIndices.push_back(std::stoi(nIndex) - 1);
            }

            // If the face is a quad (4 vertices), split it into two triangles.
            if (face.vertexIndices.size() == 4)
            {
                Face tri1, tri2;

                // First triangle (vertices 0, 1, 2)
                tri1.vertexIndices = {face.vertexIndices[0], face.vertexIndices[1], face.vertexIndices[2]};
                if (!face.textureCoordIndices.empty())
                {
                    tri1.textureCoordIndices = {face.textureCoordIndices[0], face.textureCoordIndices[1], face.textureCoordIndices[2]};
                }
                if (!face.normalIndices.empty())
                {
                    tri1.normalIndices = {face.normalIndices[0], face.normalIndices[1], face.normalIndices[2]};
                }

                // Second triangle (vertices 0, 2, 3)
                tri2.vertexIndices = {face.vertexIndices[0], face.vertexIndices[2], face.vertexIndices[3]};
                if (!face.textureCoordIndices.empty())
                {
                    tri2.textureCoordIndices = {face.textureCoordIndices[0], face.textureCoordIndices[2], face.textureCoordIndices[3]};
                }
                if (!face.normalIndices.empty())
                {
                    tri2.normalIndices = {face.normalIndices[0], face.normalIndices[2], face.normalIndices[3]};
                }

                // Replace quad with two triangles
                m_faces.push_back(tri1);
                m_faces.push_back(tri2);
            }
            else
            {
                m_faces.push_back(face);
            }
        }
    }

    file.close();
    return true;
}

} // namespace spear
