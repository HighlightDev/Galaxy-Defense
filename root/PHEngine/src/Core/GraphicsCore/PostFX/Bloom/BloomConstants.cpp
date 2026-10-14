#include "BloomConstants.h"

namespace Graphics {
const std::unordered_map<std::string, BloomQualitySettings> BloomQualitySettings::s_blurQualityMap{
    {"low", BloomQualitySettings(5, 3, 0.3f)},
    {"medium", BloomQualitySettings(7, 5, 0.3f)},
    {"high", BloomQualitySettings(10, 7, 0.5f)}};

BloomQualitySettings::BloomQualitySettings(
    const uint32_t _blurWidth, const uint32_t _blurPassCount, const float _bloomResMultiplier)
    : blurWidth(_blurWidth)
    , blurPassCount(_blurPassCount)
    , bloomResolutionMultiplier(_bloomResMultiplier)
{
}

} // namespace Graphics
