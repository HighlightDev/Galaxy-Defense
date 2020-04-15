#pragma once

#include <memory>

namespace Game
{

   class GlobalSettings
   {
   private:

      static std::unique_ptr<GlobalSettings> m_globalSettings;

      const float   mShadowOrthoProjectionHalfExtent;
      const int32_t mMaxDirLightCount;
      const int32_t mMaxDirLightShadowMapCount;
      const int32_t mMaxPointLightCount;
      const int32_t mMaxPointLightShadowMapCount;
      const float   mShadowMapBiasDirLight;
      const float   mShadowMapBiasPointLight;
      const int32_t mDirLightPCFSamplesCount;
      const int32_t mPointLightPCFSamplesCount;

      GlobalSettings();

   public:

      ~GlobalSettings();

      static inline std::unique_ptr<GlobalSettings>& GetInstance() {

         if (!m_globalSettings)
            m_globalSettings = std::unique_ptr<GlobalSettings>(new GlobalSettings());

         return m_globalSettings;
      }

      inline float GetShadowOrthoProjectionHalfExtent() const {
         return mShadowOrthoProjectionHalfExtent;
      }

      inline int32_t GetMaxDirLightCount() const {
         return
            mMaxDirLightCount;
      }

      inline int32_t GetMaxDirLightShadowMapCount() const {
         return
            mMaxDirLightShadowMapCount;
      }

      inline int32_t GetMaxPointLightCount() const {
         return mMaxPointLightCount;
      }

      inline int32_t GetMaxPointLightShadowMapCount() const {
         return mMaxPointLightShadowMapCount;
      }

      inline float GetShadowMapBiasDirLight() const {
         return mShadowMapBiasDirLight;
      }

      inline float GetShadowMapBiasPointLight() const {
         return mShadowMapBiasPointLight;
      }

      inline int32_t GetDirLightPCFSamplesCount() const {
         return mDirLightPCFSamplesCount;
      }

      inline int32_t GetPointLightPCFSamplesCount() const {
         return mPointLightPCFSamplesCount;
      }

   };

}

