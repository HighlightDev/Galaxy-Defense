#include "ComponentCreatorFactory.h"
#include "Core/GameCore/Scene.h"

namespace Game {

   std::shared_ptr<MaterialProxy> RegisterMaterialOnScene(Scene* scene, IMaterial* materialInstance)
   {
      return scene->RegisterMaterialInstance(std::shared_ptr<IMaterial>(materialInstance));
   }

}