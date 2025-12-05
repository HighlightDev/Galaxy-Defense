#include "ActorControllerCreatorFactory.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/HumanoidPlayerController.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

using namespace EngineCore;

namespace EngineCore {
namespace Scripts {
void ActorControllerCreatorFactory::CreateActorController(
    const std::weak_ptr<::EngineCore::Scene>& sceneWp,
    const std::string& actorName,
    const std::string& actorControllerTypeName,
    const std::string& jsonParamStr) const
{
    const nlohmann::json jsonRoot = nlohmann::json::parse(jsonParamStr);
    const auto& sceneSp = sceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in CreateActorController");
    if ("HumanoidPlayerController" == actorControllerTypeName) {
        const auto& actorSp = sceneSp->GetActorByName(actorName);
        const auto& cameraSp = sceneSp->GetCamera(nlohmann_utilities::GetStringFromJson(jsonRoot, "cameraName"));
        sceneSp->AddActorController(std::make_shared<HumanoidPlayerController>(cameraSp, actorSp));
    } else {
        ext_assert(false, "ActorController type not implemented: " + actorControllerTypeName);
    }
}
} // namespace Scripts
} // namespace EngineCore
