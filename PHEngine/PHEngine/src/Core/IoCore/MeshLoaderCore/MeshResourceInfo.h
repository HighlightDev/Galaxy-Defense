#pragma once

#include "Core/GameCore/GlobalSettings.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/MeshData.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/AnimationData.h"

using namespace Game;

using namespace IO::MeshLoader::Assimp;

namespace IO
{
   struct MeshResourceInfo
   {
      MeshData<GlobalSettings::GetCountBonesPerVertexForAnimation()>* MeshData = nullptr;
      AnimationData* AninationData = nullptr;

      ~MeshResourceInfo() {
         delete MeshData;
         delete AninationData;
      }
   };
}