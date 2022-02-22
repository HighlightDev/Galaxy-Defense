#include "ComponentCreatorFactory.h"
#include "Core/GameCore/Scene.h"

namespace EngineCore {

   std::shared_ptr<MaterialProxy> RegisterMaterialOnScene(Scene* const scene, IMaterial* materialInstance)
   {
      return scene->RegisterMaterialInstance(std::shared_ptr<IMaterial>(materialInstance));
   }

}