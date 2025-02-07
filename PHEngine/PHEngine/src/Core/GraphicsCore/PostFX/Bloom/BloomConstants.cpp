#include "BloomConstants.h"

namespace Graphics {
const std::unordered_map<std::string, BloomQualitySettings> BloomQualitySettings::s_blurQualityMap{
    {"low", BloomQualitySettings(2, 3, 0.3f)},
    {"medium", BloomQualitySettings(5, 5, 0.7f)},
    {"high", BloomQualitySettings(10, 10, 1.0f)}};

BloomQualitySettings::BloomQualitySettings(
    const uint32_t _blurWidth, const uint32_t _blurPassCount, const float _bloomResMultiplier)
    : blurWidth(_blurWidth)
    , blurPassCount(_blurPassCount)
    , bloomResolutionMultiplier(_bloomResMultiplier)
{
}

} // namespace Graphics
