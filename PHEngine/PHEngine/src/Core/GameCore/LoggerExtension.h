#pragma once
#include <TinyLogger/LogInterface.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace TinyLogger {
   namespace LogHelp {

      template <>
      struct CastTypeToString<glm::vec3>
      {
         static std::string Do(const glm::vec3& value)
         {
            return "vec3:{x=" + TinyLogger::LogHelp::ToString<float>::Value(value.x) + ","
               + "y=" + TinyLogger::LogHelp::ToString<float>::Value(value.y) + ","
               + "z=" + TinyLogger::LogHelp::ToString<float>::Value(value.z) + "}";
         }
      };

      template <>
      struct CastTypeToString<glm::vec4>
      {
         static std::string Do(const glm::vec4& value)
         {
            return "vec4:{x=" + TinyLogger::LogHelp::ToString<float>::Value(value.x) + ","
               + "y=" + TinyLogger::LogHelp::ToString<float>::Value(value.y) + ","
               + "z=" + TinyLogger::LogHelp::ToString<float>::Value(value.z) + ","
               + "w=" + TinyLogger::LogHelp::ToString<float>::Value(value.w) + "}";
         }
      };

      template <>
      struct CastTypeToString<glm::mat4>
      {
         static std::string Do(const glm::mat4& value)
         {
            return
               "mat4:{[0][0]=" + TinyLogger::LogHelp::ToString<float>::Value(value[0][0]) + ","
               "[0][1]=" + TinyLogger::LogHelp::ToString<float>::Value(value[0][1]) + ","
               "[0][2]=" + TinyLogger::LogHelp::ToString<float>::Value(value[0][2]) + ","
               "[0][3]=" + TinyLogger::LogHelp::ToString<float>::Value(value[0][3]) + ","
               "[1][0]=" + TinyLogger::LogHelp::ToString<float>::Value(value[1][0]) + ","
               "[1][1]=" + TinyLogger::LogHelp::ToString<float>::Value(value[1][1]) + ","
               "[1][2]=" + TinyLogger::LogHelp::ToString<float>::Value(value[1][2]) + ","
               "[1][3]=" + TinyLogger::LogHelp::ToString<float>::Value(value[1][3]) + ","
               "[2][0]=" + TinyLogger::LogHelp::ToString<float>::Value(value[2][0]) + ","
               "[2][1]=" + TinyLogger::LogHelp::ToString<float>::Value(value[2][1]) + ","
               "[2][2]=" + TinyLogger::LogHelp::ToString<float>::Value(value[2][2]) + ","
               "[2][3]=" + TinyLogger::LogHelp::ToString<float>::Value(value[2][3]) + ","
               "[3][0]=" + TinyLogger::LogHelp::ToString<float>::Value(value[3][0]) + ","
               "[3][1]=" + TinyLogger::LogHelp::ToString<float>::Value(value[3][1]) + ","
               "[3][2]=" + TinyLogger::LogHelp::ToString<float>::Value(value[3][2]) + ","
               "[3][3]=" + TinyLogger::LogHelp::ToString<float>::Value(value[3][3]) + "}";
         }
      };
   }
}
