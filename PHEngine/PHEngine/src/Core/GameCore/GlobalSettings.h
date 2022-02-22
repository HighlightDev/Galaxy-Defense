#pragma once

#include <memory>

namespace EngineCore
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
      const int32_t mMaxSpotlightCount;
      const int32_t mMaxSpotlightShadowMapCount;
      const float   mShadowMapBiasDirLight;
      const float   mShadowMapBiasPointLight;
      const float   mShadowMapBiasSpotlight;
      const int32_t mDirLightPCFSamplesCount;
      const int32_t mPointLightPCFSamplesCount;
      const int32_t mSpotlightPCFSamplesCount;
      static constexpr int32_t CountBonesPerVertexForAnimation = 4;

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

      inline int32_t GetMaxSpotlightCount() const {
         return mMaxSpotlightCount;
      }

      inline int32_t GetMaxSpotlightShadowMapCount() const {
         return mMaxSpotlightShadowMapCount;
      }

      inline float GetShadowMapBiasDirLight() const {
         return mShadowMapBiasDirLight;
      }

      inline float GetShadowMapBiasPointLight() const {
         return mShadowMapBiasPointLight;
      }

      inline float GetShadowMapBiasSpotlight() const {
         return mShadowMapBiasSpotlight;
      }

      inline int32_t GetDirLightPCFSamplesCount() const {
         return mDirLightPCFSamplesCount;
      }

      inline int32_t GetPointLightPCFSamplesCount() const {
         return mPointLightPCFSamplesCount;
      }

      inline int32_t GetSpotlightPCFSamplesCount() const {
         return mSpotlightPCFSamplesCount;
      }

      static constexpr int32_t GetCountBonesPerVertexForAnimation() {
         return GlobalSettings::CountBonesPerVertexForAnimation;
      }

   };

}

