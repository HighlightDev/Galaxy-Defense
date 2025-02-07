#pragma once

namespace Graphics {
struct ViewProjectionInfo {
    virtual glm::mat4 CreateProjectionMatrix() const = 0;
};
} // namespace Graphics