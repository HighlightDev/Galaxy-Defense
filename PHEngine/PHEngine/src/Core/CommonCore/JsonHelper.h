#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <json/json.hpp>

struct JsonVec3
{
    float x;
    float y;
    float z;

    JsonVec3() = default;

    JsonVec3(const float _x, const float _y, const float _z);

    JsonVec3(const glm::vec3& vec);
};

void to_json(nlohmann::json &j, const JsonVec3 &vec);

void from_json(const nlohmann::json &j, JsonVec3 &resultVec);

struct JsonVec2
{
    float x;
    float y;

    JsonVec2() = default;

    JsonVec2(const float _x, const float _y);

    JsonVec2(const glm::vec2& vec);
};

void to_json(nlohmann::json &j, const JsonVec2 &vec);

void from_json(const nlohmann::json &j, JsonVec2 &resultVec);
