#pragma once

#include "Core/GameCore/GlobalSettings.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/MeshData.h"

using namespace Game;

using namespace MeshLoader::Assimp;

namespace IO
{
   struct MeshResourceInfo
   {
      AnimatedMeshData* MeshAnimatedData = nullptr;
      MeshAttributes* MeshAttributes = nullptr;

      ~MeshResourceInfo() {
         delete MeshAttributes;
      }
   };
}