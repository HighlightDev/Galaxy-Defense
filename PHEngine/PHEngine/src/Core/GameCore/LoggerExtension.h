#pragma once
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"

#include <TinyLogger/LogInterface.h>
#include <glm/ext/quaternion_float.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <utility>

using namespace EngineCore::GUI;

namespace EngineCore {
template<typename... LogArgs>
void LogInfo(LogArgs&&... args)
{
    TinyLogger::Logger::Out(
        ThreadHelper::GetInstance()->GetCurrentThreadNameFromRegisteredThreads(), std::forward<LogArgs>(args)...);
}
} // namespace EngineCore

namespace TinyLogger {
namespace LogHelp {

template<>
struct CastTypeToString<glm::vec2> {
    static std::string Do(const glm::vec2& value)
    {
        return "vec2:{x=" + TinyLogger::LogHelp::ToString<float>::Value(value.x)
            + " y=" + TinyLogger::LogHelp::ToString<float>::Value(value.y) + "}";
    }
};

template<>
struct CastTypeToString<glm::ivec2> {
    static std::string Do(const glm::ivec2& value)
    {
        return "ivec2:{x=" + TinyLogger::LogHelp::ToString<int32_t>::Value(value.x)
            + " y=" + TinyLogger::LogHelp::ToString<int32_t>::Value(value.y) + "}";
    }
};

template<>
struct CastTypeToString<glm::vec3> {
    static std::string Do(const glm::vec3& value)
    {
        return "vec3:{x=" + TinyLogger::LogHelp::ToString<float>::Value(value.x)
            + " y=" + TinyLogger::LogHelp::ToString<float>::Value(value.y)
            + " z=" + TinyLogger::LogHelp::ToString<float>::Value(value.z) + "}";
    }
};

template<>
struct CastTypeToString<glm::vec4> {
    static std::string Do(const glm::vec4& value)
    {
        return "vec4:{x=" + TinyLogger::LogHelp::ToString<float>::Value(value.x) + " y="
            + TinyLogger::LogHelp::ToString<float>::Value(value.y) + "z=" + TinyLogger::LogHelp::ToString<float>::Value(value.z)
            + "w=" + TinyLogger::LogHelp::ToString<float>::Value(value.w) + "}";
    }
};

template<>
struct CastTypeToString<glm::quat> {
    static std::string Do(const glm::quat& value)
    {
        return "quat:{x=" + TinyLogger::LogHelp::ToString<float>::Value(value.x)
            + " y=" + TinyLogger::LogHelp::ToString<float>::Value(value.y) + ","
            + "z=" + TinyLogger::LogHelp::ToString<float>::Value(value.z)
            + "w=" + TinyLogger::LogHelp::ToString<float>::Value(value.w) + "}";
    }
};

template<>
struct CastTypeToString<glm::mat4> {
    static std::string Do(const glm::mat4& value)
    {
        return "mat4:{[0][0]=" + TinyLogger::LogHelp::ToString<float>::Value(value[0][0])
            + ","
              "[0][1]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[0][1])
            + ","
              "[0][2]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[0][2])
            + ","
              "[0][3]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[0][3])
            + ","
              "[1][0]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[1][0])
            + ","
              "[1][1]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[1][1])
            + ","
              "[1][2]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[1][2])
            + ","
              "[1][3]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[1][3])
            + ","
              "[2][0]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[2][0])
            + ","
              "[2][1]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[2][1])
            + ","
              "[2][2]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[2][2])
            + ","
              "[2][3]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[2][3])
            + ","
              "[3][0]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[3][0])
            + ","
              "[3][1]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[3][1])
            + ","
              "[3][2]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[3][2])
            + ","
              "[3][3]="
            + TinyLogger::LogHelp::ToString<float>::Value(value[3][3]) + "}";
    }
};

template<>
struct CastTypeToString<BoundingBox2D<glm::vec2>> {
    static std::string Do(const BoundingBox2D<glm::vec2>& value)
    {
        return "origin = " + CastTypeToString<glm::vec2>::Do(value.GetOrigin())
            + "; half extent = " + CastTypeToString<glm::vec2>::Do(value.GetHalfExtent());
    }
};

template<>
struct CastTypeToString<BoundingBox2D<glm::ivec2>> {
    static std::string Do(const BoundingBox2D<glm::ivec2>& value)
    {
        return "origin = " + CastTypeToString<glm::ivec2>::Do(value.GetOrigin())
            + "; half extent = " + CastTypeToString<glm::ivec2>::Do(value.GetHalfExtent());
    }
};
} // namespace LogHelp
} // namespace TinyLogger
