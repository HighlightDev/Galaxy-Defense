#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include <type_traits>

namespace EngineCore {
namespace GUI {
template<typename T, std::enable_if_t<std::is_same_v<T, glm::vec2> || std::is_same_v<T, glm::ivec2>, bool> = true>
class BoundingBox2D {
    T mOrigin;

    T mHalfExtent;

public:
    BoundingBox2D()
        : mOrigin()
        , mHalfExtent()
    {
    }

    BoundingBox2D(const T& origin, const T& halfExtent)
        : mOrigin(origin)
        , mHalfExtent(halfExtent)
    {
    }

    ~BoundingBox2D()
    {
    }

    T GetOrigin() const
    {
        return mOrigin;
    }

    T GetHalfExtent() const
    {
        return mHalfExtent;
    }

    T GetMax() const
    {
        return mOrigin + mHalfExtent;
    }

    T GetMin() const
    {
        return mOrigin - mHalfExtent;
    }

    void SetHalfExtentX(typename const T::value_type halfX)
    {
        mHalfExtent.x = halfX;
    }

    void SetHalfExtentY(typename const T::value_type halfY)
    {
        mHalfExtent.y = halfY;
    }
};
} // namespace GUI
} // namespace EngineCore
