#pragma once

namespace Graphics {

enum class eProjectionType { PERSPECTIVE, ORTHOGRAPHIC };

struct ViewProjectionInfo {
    virtual glm::mat4 CreateProjectionMatrix() const = 0;

    virtual eProjectionType GetProjectionType() const = 0;
};
} // namespace Graphics