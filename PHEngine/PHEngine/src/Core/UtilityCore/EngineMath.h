#pragma once

#include <glm/ext/quaternion_float.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <algorithm>
#include <optional>
#include <vector>

namespace EngineMath {
/************************************************************************/
/*                             CONSTANTS                                */
/************************************************************************/
extern float G;
extern float PI;
extern float PI_HALF;
extern float ENGINE_FLOAT_EPSILON;
extern glm::vec3 AXIS_RIGHT;
extern glm::vec3 AXIS_UP;
extern glm::vec3 AXIS_FORWARD;

/************************************************************************/
/*                       Convert from degrees to radians                */
/************************************************************************/
#define DEG_TO_RAD(X) (glm::radians<float>(X))

/************************************************************************/
/*                       Convert from radians to degrees                */
/************************************************************************/
#define RAD_TO_DEG(X) (glm::degrees<float>(X))

/************************************************************************/
/*                           Math Helper Functions                      */
/************************************************************************/
bool FloatsNearEqual(const float X, const float Y);

float LerpNormalizedFloat(const float src, const float dst, const float factor);

float LerpFloat(const float x, const float x1, const float x2, const float y1, const float y2);

float CosineInterpolation(const float x, const float y1, const float y2);

int32_t LerpInt(const float x, const float x1, const float x2, const int32_t y1, const int32_t y2);

glm::vec3 LerpVec3(const float t, const float t1, const float t2, const glm::vec3& position1, const glm::vec3& position2);

glm::vec2 LerpVec2(const float t, const float t1, const float t2, const glm::vec2& position1, const glm::vec2& position2);

glm::vec4 LerpVec4(const float t, const float t1, const float t2, const glm::vec4& position1, const glm::vec4& position2);

glm::quat SLerpQuat(float t, const glm::quat& src, const glm::quat& dst);

float GetDistancePlaneToPointVec3(const glm::vec3& point, const glm::vec4& plane, float);

void TestAABBPlane(
    const glm::vec3& origin,
    const glm::vec3& extent,
    const glm::vec4& plane,
    float& outDistanceOriginToPlane,
    float& outAbsExtentOnNormalProjected);

bool TestPointInAABB(const glm::vec3& aabbMin, const glm::vec3& aabbMax, const glm::vec3& point);

bool TestPointInAABB(const glm::ivec2& aabbMin, const glm::ivec2& aabbMax, const glm::ivec2& point);

bool TestPointInAABB(const glm::vec2& aabbMin, const glm::vec2& aabbMax, const glm::vec2& point);

std::optional<glm::vec3> TestPlaneToPlaneToPlane(const glm::vec4& plane1, const glm::vec4& plane2, const glm::vec4& plane3);

float RaycastPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec4& plane);

glm::quat EulerAnglesToQuat(const glm::vec3& eulerAngles);

glm::mat4 BuildMirrorMatrix(const glm::vec4& mirrorPlane);

glm::vec3 QuatToEulerAngles(const glm::quat& rotationQuat);

float GaussFunction(const float x, const float sigma);

std::vector<float> CalculateGaussNormalizedWeights(const uint32_t blurWidth);

bool CheckSimilarityVec2(const glm::vec2& left, const glm::vec2& right);
bool CheckSimilarityIVec2(const glm::ivec2& left, const glm::ivec2& right);

bool CheckSimilarityVec3(const glm::vec3& left, const glm::vec3& right);
bool CheckSimilarityVec4(const glm::vec4& left, const glm::vec4& right);

glm::vec3 FromHexColorToVec3Color(const uint32_t hexValue);
glm::vec4 FromHexColorToVec4Color(const uint32_t hexValue);
glm::vec3 CreateRandomColor();

float ProjectVector3OnVector(const glm::vec3& projectedNonUnitVec, const glm::vec3& unitDirection);

glm::vec3 QuadraticBezier(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const float t);

} // namespace EngineMath