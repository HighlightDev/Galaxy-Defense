#pragma once
#include "Component.h"
#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaPlatformTraverseComponentFunctions.h"
#include "PlatformTraverseComponentVisitor.h"

using namespace EnginePhysics;
using namespace EngineCore::Scripts;

namespace EngineCore
{
   struct PlatformTraverseComponentData;

   class PlatformTraverseComponent
      : public Component
   {

      std::unordered_map<std::string, std::tuple<EulerAnglesTransform, float>> mMovementPoints;

      LuaPlatformTraverseComponentFunctions mScriptExecutor;

      std::unique_ptr<PlatformTraverseComponentVisitorBase> mBehaviorVisitor;

      std::string mDestinationPoint;
      std::string mLastDestinationPoint;

      float mTime;

   public:

      PlatformTraverseComponent(const PlatformTraverseComponentData& data);

      ~PlatformTraverseComponent() override;

      eComponentType GetComponentType() const override;

      void Tick(const float deltaTime) override;

      void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      void PostLevelInit() override;

      const std::unordered_map<std::string, std::tuple<EulerAnglesTransform, float>>& GetMovementPoints() const;

      void AddMovementPoint(const std::string& pointName, const EulerAnglesTransform& t, const float transitionTime);

      void SetDestinationPoint(const std::string& pointName);

      std::string GetDestinationPoint() const;

   private:

      void Move(const float deltaTime);
   };

}
