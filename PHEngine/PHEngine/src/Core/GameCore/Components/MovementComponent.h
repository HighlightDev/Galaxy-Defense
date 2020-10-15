#pragma once
#include "SceneComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaScriptExecutor_MovementComponent.h"
#include "MoveCompBehaviorVisitor.h"
#include "Core/GameCore/ScriptingCore/LuaCore.inl"

using namespace EnginePhysics;

namespace Game
{

   class MovementComponent
      : public Component
   {

      std::unordered_map<std::string, std::tuple<Transform, float>> mMovementPoints;

      LuaScriptExecutor_MovementComponent mScriptExecutor;

      std::unique_ptr<MoveCompBehaviorVisitorBase> mBehaviorVisitor;

      std::string mDestinationPoint;
      std::string mLastDestinationPoint;

      float mTime;

   public:

      MovementComponent(const std::string& gameObjectName, const std::string& relPathToScript);

      virtual ~MovementComponent();

      virtual ComponentType GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual void PostLevelInit() override;

      const std::unordered_map<std::string, std::tuple<Transform, float>>& GetMovementPoints() const;

      void AddMovementPoint(const std::string& pointName, const Transform& t, const float transitionTime);

      void SetDestinationPoint(const std::string& pointName);

      std::string GetDestinationPoint() const;

   private:

      void Move(const float deltaTime);
   };

}
