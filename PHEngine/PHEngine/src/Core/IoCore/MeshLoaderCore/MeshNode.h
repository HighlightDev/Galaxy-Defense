#pragma once
#include <glm/mat4x4.hpp>

#include <string>
#include <vector>

namespace MeshLoader {
struct MeshNode {
    std::string Name;
    std::vector<MeshNode*> Children;
    glm::mat4 NodeTransformation;

    ~MeshNode()
    {
        for (size_t i = 0; i < Children.size(); ++i) {
            delete Children[i];
        }
    }
};

} // namespace MeshLoader
