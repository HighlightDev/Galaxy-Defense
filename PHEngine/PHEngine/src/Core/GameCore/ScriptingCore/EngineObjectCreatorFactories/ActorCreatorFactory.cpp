#include "ActorCreatorFactory.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Actor.h"
#include "Core/CommonCore/Assertion.h"

using namespace EngineCore;

namespace EngineCore
{
    namespace Scripts
    {
        int32_t ActorCreatorFactory::CreateActor(const std::weak_ptr<::EngineCore::Scene> &sceneWp,
                                                  const std::string &actorName,
                                                  const glm::vec3 &rootTranslation,
                                                  const glm::vec3 &rootEulerRotationAngles,
                                                  const glm::vec3 &rootScale,
                                                  const std::string &jsonParamStr) const
        {
            const auto &sceneSp = sceneWp.lock();
            assert(sceneSp);
            const auto &actor = std::make_shared<Actor>(actorName, std::make_shared<SceneComponent>(actorName + "_root", rootTranslation, rootEulerRotationAngles, rootScale));
            sceneSp->AddActor(actor);
            return actor->GetObjectId();
        }
    }
}
