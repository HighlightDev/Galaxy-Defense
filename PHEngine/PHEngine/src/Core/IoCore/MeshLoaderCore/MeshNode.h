#pragma once
#include <glm/mat4x4.hpp>

#include <memory>
#include <string>
#include <vector>

namespace MeshLoader {
struct MeshNode {
    std::string Name;
    std::vector<std::shared_ptr<MeshNode>> Children;
    glm::mat4 NodeTransformation;
};

} // namespace MeshLoader
