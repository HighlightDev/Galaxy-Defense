#pragma once
#include "SceneComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaScriptExecutor_MovementComponent.h"
#include "PlatformMovementComponentVisitor.h"
#include "Core/GameCore/ScriptingCore/LuaCore.inl"

using namespace EnginePhysics;

namespace Game
{

   class PlatformMovementComponent
      : public Component
   {

      std::unordered_map<std::string, std::tuple<EulerAnglesTransform, float>> mMovementPoints;

      LuaScriptExecutor_MovementComponent mScriptExecutor;

      std::unique_ptr<PlatformMovementComponentVisitorBase> mBehaviorVisitor;

      std::string mDestinationPoint;
      std::string mLastDestinationPoint;

      float mTime;

   public:

      PlatformMovementComponent(const std::string& gameObjectName, const std::string& relPathToScript);

      virtual ~PlatformMovementComponent();

      virtual ComponentType GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      virtual void PostLevelInit() override;

      const std::unordered_map<std::string, std::tuple<EulerAnglesTransform, float>>& GetMovementPoints() const;

      void AddMovementPoint(const std::string& pointName, const EulerAnglesTransform& t, const float transitionTime);

      void SetDestinationPoint(const std::string& pointName);

      std::string GetDestinationPoint() const;

   private:

      void Move(const float deltaTime);
   };

}
