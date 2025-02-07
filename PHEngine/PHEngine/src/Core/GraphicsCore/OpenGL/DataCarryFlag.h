#pragma once

namespace Graphics {
namespace OpenGL {
enum class eDataCarryFlag {
    // buffer data becomes unavailable after it is sent on a gpu
    INVALIDATE = 0,
    // buffer data is available
    STORE = 1
};
}
} // namespace Graphics