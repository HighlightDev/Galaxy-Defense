#pragma once

#include "Core/UtilityCore/StringStreamWrapper.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>
#include <vector>

namespace Graphics {
namespace OpenGL {
template<typename Type>
struct TypeToString;

template<>
struct TypeToString<float> {
    static constexpr char value[] = "float";
};

template<typename Type>
struct MacroConverter;

template<>
struct MacroConverter<float> {
    static std::string GetValue(float value)
    {
        EngineUtility::StringStreamWrapper::ToString(value);
        return EngineUtility::StringStreamWrapper::FlushString();
    }

    static std::string GetArrayValue(const std::string& nameVar, const std::vector<float>& vector)
    {
        std::string unwrappedValue = "";
        for (size_t i = 0; i < vector.size(); ++i) {
            const auto isLast = i == (vector.size() - 1);
            unwrappedValue += std::to_string(vector[i]) + (isLast ? std::string("") : std::string(","));
        }
        return ("const float " + nameVar + "[" + std::to_string(vector.size()) + "] = float[] (" + unwrappedValue + ");");
    }
};

template<>
struct MacroConverter<int32_t> {
    static std::string GetValue(int32_t value)
    {
        return std::to_string(value);
    }
};

template<>
struct MacroConverter<uint32_t> {
    static std::string GetValue(uint32_t value)
    {
        return std::to_string(value);
    }
};

template<>
struct MacroConverter<glm::vec2> {
    static std::string GetValue(const glm::vec2& value)
    {
        EngineUtility::StringStreamWrapper::ToString("vec2(", value.x, ", ", value.y, ")");
        std::string result = EngineUtility::StringStreamWrapper::FlushString();
        return result;
    }
};

template<>
struct MacroConverter<glm::vec3> {
    static std::string GetValue(const glm::vec3& value)
    {
        EngineUtility::StringStreamWrapper::ToString("vec3(", value.x, ", ", value.y, value.z, ")");
        return EngineUtility::StringStreamWrapper::FlushString();
    }
};

template<>
struct MacroConverter<glm::vec4> {
    static std::string GetValue(const glm::vec4& value)
    {
        EngineUtility::StringStreamWrapper::ToString("vec4(", value.x, ", ", value.y, value.z, value.w, ")");
        return EngineUtility::StringStreamWrapper::FlushString();
    }
};
} // namespace OpenGL
} // namespace Graphics