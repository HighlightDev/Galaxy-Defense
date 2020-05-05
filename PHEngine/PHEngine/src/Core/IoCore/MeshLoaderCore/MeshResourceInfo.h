#pragma once

#include "Core/GameCore/GlobalSettings.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/MeshVertexData.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/MeshAnimationData.h"

using namespace Game;

using namespace IO::MeshLoader::Assimp;

namespace IO
{
   struct MeshResourceInfo
   {
      MeshVertexData<GlobalSettings::GetCountBonesPerVertexForAnimation()>* MeshData = nullptr;
      MeshAnimationData* AninationData = nullptr;

      ~MeshResourceInfo() {
         delete MeshData;
         delete AninationData;
      }
   };
}