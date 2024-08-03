#pragma once

#include <list>
#include <string>
#include <utility>
#include <vector>

namespace EngineUtility
{
    struct EngineConfig
    {
        size_t MaxSkeletBones{0};
        size_t MaxStaticMeshInstancesPerBatch{0};

        size_t MaxDirLightCount{0};
        size_t MaxDirLightShadowMapCount{0};
        size_t MaxPointLightCount{0};
        size_t MaxPointLightShadowMapCount{0};
        size_t MaxSpotlightCount{0};
        size_t MaxSpotlightShadowMapCount{0};

        size_t DirLightPCFSamplesCount{0};
        size_t PointLightPCFSamplesCount{0};
        size_t SpotlightPCFSamplesCount{0};

        float ShadowOrthoProjectionHalfExtent{0.0f};
        float ShadowMapBiasDirLight{0.0f};
        float ShadowMapBiasPointLight{0.0f};
        float ShadowMapBiasSpotlight{0.0f};

        bool IsHdrEnabled{false};
        float HdrExposureValue{1.0f};

        bool IsBloomEnabled{false};
        std::string BloomQualityName{""};

        size_t MaxFontCharactersCount{0};

        std::vector<std::string> FontsVector;

#ifdef DEBUG
        bool EnableAmbientMusic{true};
        bool RenderDebugPhysicsData{false};
#endif
    };

    class EngineConfigHolder
    {
        bool bSettingsLoaded;

        EngineConfig mEngineConfig;

    private:
        EngineConfigHolder();

        void FillEngineConfig(const std::list<std::string> &configLines);

        std::pair<std::string, std::string> GetKeyValueConfigFromLine(const std::string &line) const;

    public:
        static EngineConfigHolder *GetInstance()
        {
            static EngineConfigHolder sHolder;
            return &sHolder;
        }

        void LoadSettings(const std::string &pathToSettings);

        const EngineConfig &GetEngineConfig() const;
    };
}