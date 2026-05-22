#include "EngineMath.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/Random.h"

#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>

#include <limits>

namespace EngineMath {
float G = 9.8f;
float PI = 3.14159f;
float PI_HALF = 1.570795F;
float ENGINE_FLOAT_EPSILON = 1e-5f;
glm::vec3 AXIS_RIGHT = glm::vec3(1, 0, 0);
glm::vec3 AXIS_UP = glm::vec3(0, 1, 0);
glm::vec3 AXIS_FORWARD = glm::vec3(0, 0, 1);

bool FloatsNearEqual(const float X, const float Y)
{
    return std::abs(X - Y) <= ENGINE_FLOAT_EPSILON;
}

float LerpNormalizedFloat(const float src, const float dst, const float factor)
{
    ext_assert(factor <= 1 && factor >= 0, "EngineMath::LerpNormalizedFloat: factor out of range [0,1]");
    return (dst - src) * factor + src;
}

float LerpFloat(const float x, const float x1, const float x2, const float y1, const float y2)
{
    return ((y2 - y1) / (x2 - x1)) * (x - x1) + y1;
}

float CosineInterpolation(const float x, const float y1, const float y2)
{
    const float angle = x * PI;
    const float mu2 = (1.0f - std::cos(angle)) * 0.5f;
    return LerpNormalizedFloat(y1, y2, mu2);
}

int32_t LerpInt(const float x, const float x1, const float x2, const int32_t y1, const int32_t y2)
{
    const int32_t y_diff = (y2 - y1);
    return static_cast<int32_t>(std::round(((static_cast<float>(y_diff) / (x2 - x1)) * (x - x1)))) + y1;
}

glm::vec3 LerpVec3(const float t, const float t1, const float t2, const glm::vec3& position1, const glm::vec3& position2)
{
    glm::vec3 resultPosition;

    float x_delta = t2 - t1;
    float x_zero_offset = t - t1;

    resultPosition.x = ((position2.x - position1.x) / x_delta) * x_zero_offset + position1.x;
    resultPosition.y = ((position2.y - position1.y) / x_delta) * x_zero_offset + position1.y;
    resultPosition.z = ((position2.z - position1.z) / x_delta) * x_zero_offset + position1.z;

    return resultPosition;
}

glm::vec2 LerpVec2(const float t, const float t1, const float t2, const glm::vec2& position1, const glm::vec2& position2)
{
    glm::vec2 resultPosition;

    float x_delta = t2 - t1;
    float x_zero_offset = t - t1;

    resultPosition.x = ((position2.x - position1.x) / x_delta) * x_zero_offset + position1.x;
    resultPosition.y = ((position2.y - position1.y) / x_delta) * x_zero_offset + position1.y;

    return resultPosition;
}

glm::vec4 LerpVec4(const float t, const float t1, const float t2, const glm::vec4& position1, const glm::vec4& position2)
{
    glm::vec4 resultPosition;

    float x_delta = t2 - t1;
    float x_zero_offset = t - t1;

    resultPosition.x = ((position2.x - position1.x) / x_delta) * x_zero_offset + position1.x;
    resultPosition.y = ((position2.y - position1.y) / x_delta) * x_zero_offset + position1.y;
    resultPosition.z = ((position2.z - position1.z) / x_delta) * x_zero_offset + position1.z;
    resultPosition.w = ((position2.w - position1.w) / x_delta) * x_zero_offset + position1.w;

    return resultPosition;
}

glm::quat SLerpQuat(float t, const glm::quat& src, const glm::quat& dst)
{
    return glm::lerp(src, dst, t);
}

float GetDistancePlaneToPointVec3(const glm::vec3& point, const glm::vec4& plane, const float w)
{
    const glm::vec3 normal(plane);
    const float distance = glm::dot(point, normal) + w;
    return distance;
}

void TestAABBPlane(
    const glm::vec3& origin,
    const glm::vec3& extent,
    const glm::vec4& plane,
    float& outDistanceOriginToPlane,
    float& outAbsExtentOnNormalProjected)
{
    const glm::vec3& planeNormal = glm::vec3(plane);
    const glm::vec3& absNormal = glm::abs(planeNormal);
    outAbsExtentOnNormalProjected = glm::abs(glm::dot(extent, absNormal));
    outDistanceOriginToPlane = glm::dot(planeNormal, origin) + plane.w;
}

bool TestPointInAABB(const glm::vec3& aabbMin, const glm::vec3& aabbMax, const glm::vec3& point)
{
    if ((point.x < aabbMin.x || point.y < aabbMin.y || point.z < aabbMin.z)
        || (point.x > aabbMax.x || point.y > aabbMax.y || point.z > aabbMax.z)) {
        return false;
    }
    return true;
}

bool TestPointInAABB(const glm::ivec2& aabbMin, const glm::ivec2& aabbMax, const glm::ivec2& point)
{
    if ((point.x < aabbMin.x || point.y < aabbMin.y) || (point.x > aabbMax.x || point.y > aabbMax.y)) {
        return false;
    }
    return true;
}

bool TestPointInAABB(const glm::vec2& aabbMin, const glm::vec2& aabbMax, const glm::vec2& point)
{
    if ((point.x < aabbMin.x || point.y < aabbMin.y) || (point.x > aabbMax.x || point.y > aabbMax.y)) {
        return false;
    }
    return true;
}

glm::quat EulerAnglesToQuat(const glm::vec3& eulerAngles)
{
    return glm::quat(glm::vec3(DEG_TO_RAD(eulerAngles.x), DEG_TO_RAD(eulerAngles.y), DEG_TO_RAD(eulerAngles.z)));
}

glm::mat4 BuildMirrorMatrix(const glm::vec4& mirrorPlane)
{
    const glm::mat4 mirrorMatrix(
        glm::vec4(
            -2.f * mirrorPlane.x * mirrorPlane.x + 1.f,
            -2.f * mirrorPlane.y * mirrorPlane.x,
            -2.f * mirrorPlane.z * mirrorPlane.x,
            0.f),
        glm::vec4(
            -2.f * mirrorPlane.x * mirrorPlane.y,
            -2.f * mirrorPlane.y * mirrorPlane.y + 1.f,
            -2.f * mirrorPlane.z * mirrorPlane.y,
            0.f),
        glm::vec4(
            -2.f * mirrorPlane.x * mirrorPlane.z,
            -2.f * mirrorPlane.y * mirrorPlane.z,
            -2.f * mirrorPlane.z * mirrorPlane.z + 1.f,
            0.f),
        glm::vec4(
            2.f * mirrorPlane.x * mirrorPlane.w, 2.f * mirrorPlane.y * mirrorPlane.w, 2.f * mirrorPlane.z * mirrorPlane.w, 1.f));

    return mirrorMatrix;
}

glm::vec3 QuatToEulerAngles(const glm::quat& rotationQuat)
{
    static constexpr float radToDeg = 180.f / 3.14159f;
    return glm::eulerAngles(rotationQuat) * radToDeg;
}

std::optional<glm::vec3> TestPlaneToPlaneToPlane(const glm::vec4& plane1, const glm::vec4& plane2, const glm::vec4& plane3)
{
    std::optional<glm::vec3> result = std::nullopt;
    glm::vec3 N1(plane1);
    glm::vec3 N2(plane2);
    glm::vec3 N3(plane3);

    glm::vec3 n2n3 = glm::cross(N2, N3);
    glm::vec3 n3n1 = glm::cross(N3, N1);
    glm::vec3 n1n2 = glm::cross(N1, N2);

    float quotient = glm::dot(N1, n2n3);

    if (glm::abs(quotient) > ENGINE_FLOAT_EPSILON) {
        quotient = -1.0f / quotient;
        n2n3 *= plane1.w;
        n3n1 *= plane2.w;
        n1n2 *= plane3.w;
        glm::vec3 potentialVertex = n2n3;
        potentialVertex += n3n1;
        potentialVertex += n1n2;
        potentialVertex *= quotient;
        result = potentialVertex;
    }

    return result;
}

float GaussFunction(const float x, const float sigma)
{
    const float power = -((x * x) / (2.0f * (sigma * sigma)));
    return (1.0f / (std::sqrt(2.0f * PI * (sigma * sigma)))) * std::pow(glm::e<float>(), power);
}

std::vector<float> CalculateGaussNormalizedWeights(const uint32_t blurWidth)
{
    std::vector<float> weights;
    static constexpr auto sigma2 = 2.0f;

    float sum = GaussFunction(0, sigma2); // The 1-D Gaussian function
    weights.emplace_back(sum);
    for (size_t i = 1; i < blurWidth; ++i) {
        const float weight = GaussFunction(static_cast<float>(i), sigma2);
        sum += 2.0f * weight;
        weights.emplace_back(weight);
    }

    const float invSum = 1.0f / sum;

    // Normalize the weights
    for (auto& weight : weights) {
        weight = weight * invSum;
    }
    return weights;
}

bool CheckSimilarityVec2(const glm::vec2& left, const glm::vec2& right)
{
    const auto absoluteDiff = glm::abs(left - right);
    return absoluteDiff.x <= ENGINE_FLOAT_EPSILON && absoluteDiff.y <= ENGINE_FLOAT_EPSILON;
}

bool CheckSimilarityIVec2(const glm::ivec2& left, const glm::ivec2& right)
{
    return left.x == right.x && left.y == right.y;
}

bool CheckSimilarityVec3(const glm::vec3& left, const glm::vec3& right)
{
    const auto absoluteDiff = glm::abs(left - right);
    return absoluteDiff.x <= ENGINE_FLOAT_EPSILON && absoluteDiff.y <= ENGINE_FLOAT_EPSILON
        && absoluteDiff.z <= ENGINE_FLOAT_EPSILON;
}

bool CheckSimilarityVec4(const glm::vec4& left, const glm::vec4& right)
{
    const auto absoluteDiff = glm::abs(left - right);
    return absoluteDiff.x <= ENGINE_FLOAT_EPSILON && absoluteDiff.y <= ENGINE_FLOAT_EPSILON
        && absoluteDiff.z <= ENGINE_FLOAT_EPSILON && absoluteDiff.w <= ENGINE_FLOAT_EPSILON;
}

glm::vec3 FromHexColorToVec3Color(const uint32_t hexValue)
{
    static constexpr auto mask_b = 0xFF;
    static constexpr auto mask_g = 0xFF << 0x8;
    static constexpr auto mask_r = 0xFF << 0x10;

    const uint8_t r = (mask_r & hexValue) >> 0x10;
    const uint8_t g = (mask_g & hexValue) >> 0x8;
    const uint8_t b = mask_b & hexValue;

    static constexpr float INV_COLOR_MAX_BYTE_VALUE = 1.0f / 255.0f;
    glm::vec3 color = glm::vec3(
        static_cast<float>(r) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(g) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(b) * INV_COLOR_MAX_BYTE_VALUE);
    return color;
}

glm::vec4 FromHexColorToVec4Color(const uint32_t hexValue)
{
    static constexpr auto mask_a = 0xFF;
    static constexpr auto mask_b = 0xFF << 0x8;
    static constexpr auto mask_g = 0xFF << 0x10;
    static constexpr auto mask_r = 0xFF << 0x18;

    const uint8_t r = (mask_r & hexValue) >> 0x18;
    const uint8_t g = (mask_g & hexValue) >> 0x10;
    const uint8_t b = (mask_b & hexValue) >> 0x8;
    const uint8_t a = mask_a & hexValue;

    static constexpr float INV_COLOR_MAX_BYTE_VALUE = 1.0f / 255.0f;
    glm::vec4 color = glm::vec4(
        static_cast<float>(r) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(g) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(b) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(a) * INV_COLOR_MAX_BYTE_VALUE);
    return color;
}

glm::vec3 CreateRandomColor()
{
    return glm::vec3(Random::Float(), Random::Float(), Random::Float());
}

//     P*
//    / |          
//   /  |
// A ---X---->B
// Project PA on BA
float ProjectVectors(const glm::vec3& pa, const glm::vec3& ba)
{
    return glm::dot(pa, ba) / glm::dot(ba, ba);
}

float RaycastPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec4& plane)
{
    const auto& planeNormal = glm::vec3(plane.x, plane.y, plane.z);
    const float nd = glm::dot(rayDirection, planeNormal);
    const float pn = glm::dot(rayOrigin, planeNormal);

    if (nd >= 0.0f) {
        return -1.0f;
    }

    const float t = (plane.w - pn) / nd;
    return t > 0.0f ? t : -1.0f;
}

glm::vec3 QuadraticBezier(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const float t)
{
    const float restTime = 1.0f - t;
    const float x = (restTime * restTime) * p1.x + 2.0f * restTime * t * p2.x + (t * t) * p3.x;
    const float y = (restTime * restTime) * p1.y + 2.0f * restTime * t * p2.y + (t * t) * p3.y;
    const float z = (restTime * restTime) * p1.z + 2.0f * restTime * t * p2.z + (t * t) * p3.z;
    return glm::vec3(x, y, z);
}

float NormalizeAngleDegrees(float angleDegrees)
{
    while (angleDegrees > 180.0f) {
        angleDegrees -= 360.0f;
    }
    while (angleDegrees < -180.0f) {
        angleDegrees += 360.0f;
    }
    return angleDegrees;
}

float NormalizeAngleRadians(float angleRadians)
{
    while (angleRadians > PI) {
        angleRadians -= 2.0f * PI;
    }
    while (angleRadians < -PI) {
        angleRadians += 2.0f * PI;
    }
    return angleRadians;
}

float GetRollFromQuaternion(const glm::quat& quat)
{
    // Roll (x-axis rotation)
    float sinr_cosp = 2.0f * (quat.w * quat.x + quat.y * quat.z);
    float cosr_cosp = 1.0f - 2.0f * (quat.x * quat.x + quat.y * quat.y);
    float roll = std::atan2(sinr_cosp, cosr_cosp);
    return RAD_TO_DEG(roll);
}

glm::mat4 CreateRotationMatrixFromDirection(const glm::vec3& direction)
{
    // Find a vector, ref, not parallel to v
    glm::vec3 vmag = glm::abs(direction);
    glm::vec3 ref;
    if (vmag.x <= vmag.y && vmag.x <= vmag.z) {
        ref = glm::vec3(1.0, 0.0, 0.0);
    } else if (vmag.y <= vmag.z) {
        ref = glm::vec3(0.0, 1.0, 0.0);
    } else {
        ref = glm::vec3(0.0, 0.0, 1.0);
    }
    // Use ref to create two unit vectors, u1, u2, so {direction, u1, u2} are orthogonal
    glm::vec3 utemp = glm::cross(direction, ref);
    glm::vec3 u1 = glm::normalize(glm::cross(direction, utemp));
    glm::vec3 u2 = glm::normalize(glm::cross(direction, u1));
    return glm::mat4(glm::vec4(u1, 0.0f), glm::vec4(u2, 0.0f), glm::vec4(direction, 0.0f), glm::vec4(0, 0, 0, 1));
}

} // namespace EngineMath