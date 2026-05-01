#include "EngineConfigHolder.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/IoCore/FileFacade.h"
#include "StringExtendedFunctions.h"

#include <algorithm>
#include <functional>
#include <sstream>

namespace EngineUtility {
EngineConfigHolder::EngineConfigHolder()
    : bSettingsLoaded(false)
    , mEngineConfig()
{
}

void EngineConfigHolder::LoadSettings(const std::string& pathToSettings)
{
    ext_assert(!bSettingsLoaded, "EngineConfigHolder::LoadSettings: Settings already loaded");
    FileFacade fileLoader;
    fileLoader.OpenAndReadFile(pathToSettings);
    const auto& configFileLines = fileLoader.GetFileSrc();
    FillEngineConfig(configFileLines);
    bSettingsLoaded = true;
}

void EngineConfigHolder::FillEngineConfig(const std::list<std::string>& configLines)
{
    const auto parseUInt = [](const std::string& valueStr) -> size_t {
        std::istringstream iss(valueStr);
        size_t value;
        iss >> value;
        return value;
    };

    const auto parseFloat = [](const std::string& valueStr) -> float { return stof(valueStr); };

    const auto parseBool = [](const std::string& valueStr) -> float { return valueStr == "true"; };

    const auto parseStringVector = [](const std::string& valueStr) -> std::vector<std::string> {
        auto vectorItems = Split(valueStr, ';');
        ext_assert(vectorItems.size(), "EngineConfigHolder::FillEngineConfig: Empty string vector");
        std::transform(vectorItems.begin(), vectorItems.end(), vectorItems.begin(), [](const auto& item) -> std::string {
            return Trim(item);
        });
        return vectorItems;
    };

    std::unordered_map<std::string, std::function<void(const std::string& valueStr)>> config_values_map
        = {std::make_pair(
               "window_mode", std::function([=, this](const std::string& valueStr) { mEngineConfig.WindowMode = valueStr; })),
           std::make_pair("active_monitor", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.ActiveMonitor = valueStr;
                          })),
           std::make_pair("max_skeletal_mesh_bones", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.MaxSkeletBones = parseUInt(valueStr);
                          })),
           std::make_pair("max_static_mesh_instances_per_batch", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.MaxStaticMeshInstancesPerBatch = parseUInt(valueStr);
                          })),
           std::make_pair("enable_point_lights", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.EnablePointLights = parseBool(valueStr);
                          })),
           std::make_pair("enable_shadows", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.EnableShadows = parseBool(valueStr);
                          })),
           std::make_pair("shadowmap_size", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.ShadowMapSize = parseUInt(valueStr);
                          })),
           std::make_pair("max_dir_light_shadow_map_count", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.MaxDirLightShadowMapCount = parseUInt(valueStr);
                          })),
           std::make_pair("max_point_light_shadow_map_count", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.MaxPointLightShadowMapCount = parseUInt(valueStr);
                          })),
           std::make_pair("max_spotlight_shadow_map_count", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.MaxSpotlightShadowMapCount = parseUInt(valueStr);
                          })),
           std::make_pair("dir_light_PCF_samples_count", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.DirLightPCFSamplesCount = parseUInt(valueStr);
                          })),
           std::make_pair("point_light_PCF_samples_count", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.PointLightPCFSamplesCount = parseUInt(valueStr);
                          })),
           std::make_pair("spotlight_PCF_samples_count", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.SpotlightPCFSamplesCount = parseUInt(valueStr);
                          })),
           std::make_pair("shadow_ortho_projection_half_extent", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.ShadowOrthoProjectionHalfExtent = parseFloat(valueStr);
                          })),
           std::make_pair("shadow_map_bias_dir_light", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.ShadowMapBiasDirLight = parseFloat(valueStr);
                          })),
           std::make_pair("shadow_map_bias_point_light", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.ShadowMapBiasPointLight = parseFloat(valueStr);
                          })),
           std::make_pair("shadow_map_bias_spot_light", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.ShadowMapBiasSpotlight = parseFloat(valueStr);
                          })),
           std::make_pair("max_font_characters_count", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.MaxFontCharactersCount = parseUInt(valueStr);
                          })),
           std::make_pair("enable_hdr", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.IsHdrEnabled = parseBool(valueStr);
                          })),
           std::make_pair("hdr_exposure_value", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.HdrExposureValue = parseFloat(valueStr);
                          })),
           std::make_pair("enable_bloom", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.IsBloomEnabled = parseBool(valueStr);
                          })),
           std::make_pair("bloom_quality", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.BloomQualityName = valueStr;
                          })),
           std::make_pair("bloom_threshold", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.BloomThreshold = parseFloat(valueStr);
                          })),
#ifdef DEBUG
           std::make_pair("debug_enable_ambient_music", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.EnableAmbientMusic = parseBool(valueStr);
                          })),
           std::make_pair("debug_enable_sounds", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.EnableSounds = parseBool(valueStr);
                          })),
           std::make_pair("render_debug_physics_data", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.RenderDebugPhysicsData = parseBool(valueStr);
                          })),
           std::make_pair("enable_debug_ui_widget_naming", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.EnableDebugUiWidgetNaming = parseBool(valueStr);
                          })),
           std::make_pair("enable_debug_shader_source_print", std::function([=, this](const std::string& valueStr) {
                              mEngineConfig.EnableDebugShaderSourcePrint = parseBool(valueStr);
                          }))
#endif
        };

    for (const auto& line : configLines) {
        if ("" != Trim(line) && !StartsWith(line, "#")) {
            const auto [key, value] = GetKeyValueConfigFromLine(line);
            if (config_values_map.count(key)) {
                const auto fn = config_values_map[key];
                fn(value);
                EngineCore::LogInfo("EngineConfigHolder::FillEngineConfig: Loaded config: ", key, " with value: ", value);
            } else {
                EngineCore::LogInfo("EngineConfigHolder::FillEngineConfig: unknown key: ", key, ". Skip.");
            }
        }
    }
}

std::pair<std::string, std::string> EngineConfigHolder::GetKeyValueConfigFromLine(const std::string& line) const
{
    const auto& splitKeyValue = Split(line, ':');
    ext_assert(splitKeyValue.size() == 2, "EngineConfigHolder::GetKeyValueConfigFromLine: Invalid config line: " + line);
    return std::make_pair(Trim(splitKeyValue[0]), Trim(splitKeyValue[1]));
}

const EngineConfig& EngineConfigHolder::GetEngineConfig() const
{
    ext_assert(bSettingsLoaded, "EngineConfigHolder::GetEngineConfig: Settings not loaded");
    return mEngineConfig;
}
} // namespace EngineUtility
