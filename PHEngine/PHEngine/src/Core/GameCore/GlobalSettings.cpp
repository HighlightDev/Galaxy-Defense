#include "GlobalSettings.h"

namespace Game
{

	std::unique_ptr<GlobalSettings> GlobalSettings::m_globalSettings;

   GlobalSettings::GlobalSettings()
      : mShadowOrthoProjectionHalfExtent(25)
      , mShadowTransitionAreaLength(5)
      , mMaxDirLightCount(5)
      , mMaxDirLightShadowMapCount(4)
      , mMaxPointLightCount(50)
      , mMaxPointLightShadowMapCount(4)
      , mShadowMapBiasDirLight(0.005f)
      , mShadowMapBiasPointLight(0.05f)
      , mDirLightPCFSamplesCount(2)
      , mPointLightPCFSamplesCount(4)
   {
   }

	GlobalSettings::~GlobalSettings()
	{
	}

}