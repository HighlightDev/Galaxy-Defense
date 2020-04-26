#pragma once

#include <BulletPhys/LinearMath/btIDebugDraw.h>
#include <vector>
#include <glm/vec3.hpp>

#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "DebugPhysicsRenderData.h"

using namespace Converter;

namespace Game {

   class BulletDebugRenderer
      : public btIDebugDraw
   {
      int m_debugMode;
      DebugPhysicsRenderData mCollectDebugRenderData;

   public:

      BulletDebugRenderer()
         : btIDebugDraw()
         , m_debugMode(DebugDrawModes::DBG_DrawWireframe)
      {
      }

      virtual ~BulletDebugRenderer()
      {
      }

      virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
         mCollectDebugRenderData.AddLine(bulletToGlm(from), bulletToGlm(to));
      }

      virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {}

      virtual void reportErrorWarning(const char* warningString) {}

      virtual void draw3dText(const btVector3& location, const char* textString) {}

      virtual void setDebugMode(int debugMode) { m_debugMode = debugMode; }

      virtual int getDebugMode() const { return m_debugMode; }

      void ClearLinesBuffer()
      {
         mCollectDebugRenderData.ClearLines();
      }

      const DebugPhysicsRenderData& GetRenderData() const
      {
         return mCollectDebugRenderData;
      }

   };

}