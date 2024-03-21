#include "JsonHelper.h"

JsonVec3::JsonVec3(const float _x, const float _y, const float _z)
    : x(_x),
      y(_y),
      z(_z)
{
}

JsonVec3::JsonVec3(const glm::vec3 &vec)
    : JsonVec3(vec.x, vec.y, vec.z)
{
}

void to_json(nlohmann::json &j, const JsonVec3 &vec)
{
    j["x"] = vec.x;
    j["y"] = vec.y;
    j["z"] = vec.z;
}

void from_json(const nlohmann::json &j, JsonVec3 &resultVec)
{
    resultVec.x = j.at("x").get<float>();
    resultVec.y = j.at("y").get<float>();
    resultVec.z = j.at("z").get<float>();
}
