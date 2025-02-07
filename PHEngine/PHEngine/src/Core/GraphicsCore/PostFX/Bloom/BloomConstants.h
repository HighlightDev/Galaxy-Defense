#pragma once

#include <stdint.h>

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Graphics {
struct BloomQualitySettings {
    uint32_t blurWidth;
    uint32_t blurPassCount;
    float bloomResolutionMultiplier;

    BloomQualitySettings(const uint32_t _blurWidth, const uint32_t _blurPassCount, const float _bloomResMultiplier);

    static const std::unordered_map<std::string, BloomQualitySettings> s_blurQualityMap;
};

} // namespace Graphics
