#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"

namespace Game
{
   class LuaWrapper;
}

using namespace Game;

namespace Labyrinth
{
   class GameSkeletalMeshComponent
      : public SkeletalMeshComponent
   {
      using Base = SkeletalMeshComponent;

   protected:

      std::unique_ptr<Game::LuaWrapper> mLuaInstance;

   public:

      GameSkeletalMeshComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const std::string& mLuaScriptRelPath,
         const SkeletalMeshRenderData& renderData);

      virtual ~GameSkeletalMeshComponent();

      virtual void Tick(const float deltaTime) override;

   };
}