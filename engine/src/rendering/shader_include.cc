#include <spear/rendering/shader_include.hh>
#include <spear/spear_root.hh>

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace spear::rendering
{

namespace
{

std::string resolve(const std::string& source, int depth)
{
    if (depth > 8)
        throw std::runtime_error("Shader includes nested too deeply");

    std::istringstream input(source);
    std::ostringstream output;
    std::string line;
    while (std::getline(input, line))
    {
        const auto directive = line.find("#include");
        const auto open = line.find('"');
        const auto close = line.rfind('"');
        if (directive == std::string::npos || open == std::string::npos || close <= open)
        {
            output << line << '\n';
            continue;
        }

        const std::string path = getShaderPath(line.substr(open + 1, close - open - 1));
        std::ifstream file(path);
        if (!file.is_open())
            throw std::runtime_error("Failed to open shader include: " + path);
        std::stringstream contents;
        contents << file.rdbuf();
        output << resolve(contents.str(), depth + 1) << '\n';
    }
    return output.str();
}

} // namespace

std::string resolveShaderIncludes(const std::string& source)
{
    return resolve(source, 0);
}

} // namespace spear::rendering
