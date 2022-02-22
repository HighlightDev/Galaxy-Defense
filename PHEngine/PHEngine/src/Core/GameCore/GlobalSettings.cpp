#include "GlobalSettings.h"

namespace EngineCore
{

	std::unique_ptr<GlobalSettings> GlobalSettings::m_globalSettings;

   GlobalSettings::GlobalSettings()
      : mShadowOrthoProjectionHalfExtent(25)
      , mMaxDirLightCount(5)
      , mMaxDirLightShadowMapCount(4)
      , mMaxPointLightCount(50)
      // todo: IMPORTANT!! some vendors don't support array of cubemap samplers,
      // so currently engine supports only one cubemap sampler
      , mMaxPointLightShadowMapCount(1)
      , mMaxSpotlightCount(50)
      , mMaxSpotlightShadowMapCount(4)
      , mShadowMapBiasDirLight(0.005f)
      , mShadowMapBiasPointLight(0.55f)
      , mShadowMapBiasSpotlight(0.25f)
      , mDirLightPCFSamplesCount(2)
      , mPointLightPCFSamplesCount(3)
      , mSpotlightPCFSamplesCount(3)
   {
   }

	GlobalSettings::~GlobalSettings()
	{
	}

}