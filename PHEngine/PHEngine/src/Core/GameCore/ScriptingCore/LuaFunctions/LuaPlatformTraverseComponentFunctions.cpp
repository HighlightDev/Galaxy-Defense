#include "LuaPlatformTraverseComponentFunctions.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/PlatformTraverseComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;
using namespace EngineCore;

namespace EngineCore {
namespace Scripts {
LuaPlatformTraverseComponentFunctions::LuaPlatformTraverseComponentFunctions(
    PlatformTraverseComponent* owner, LuaScriptExecutorBase* ownerPtr)
    : mOwnerPtr(ownerPtr)
    , mOwnerComponent(owner)
{
}

void LuaPlatformTraverseComponentFunctions::Initialize()
{
}

void LuaPlatformTraverseComponentFunctions::SetScene(const std::weak_ptr<Scene>& sceneWp)
{
    mSceneWp = sceneWp;
}

void LuaPlatformTraverseComponentFunctions::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& scriptProcessor)
{
    mLuaScriptProcessor = scriptProcessor;
}

void LuaPlatformTraverseComponentFunctions::OnScriptStarted(const LuaWrapper& luaWrapper)
{
    mOwnerComponent->SetDestinationPoint(GetLuaGlobalVariable<std::string>::Value(luaWrapper, "StartRoute", -1));
}

void LuaPlatformTraverseComponentFunctions::RegisterCallbacks(const LuaWrapper& luaWrapper)
{
    LuaCallbackBindingHelper<
        Hash64_CT("LuaPlatformTraverseComponentFunctions::AddRoutePoint"),
        void(std::string, glm::vec3, glm::vec3, glm::vec3, float)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaPlatformTraverseComponentFunctions::AddRoutePoint, this, std::placeholders::_1),
            "_AddRoutePoint");
}

void LuaPlatformTraverseComponentFunctions::OnScriptStopped(const LuaWrapper& luaWrapper)
{
}

void LuaPlatformTraverseComponentFunctions::AddRoutePoint(
    const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, float>& data)
{
    const std::string& name = std::get<0>(data);
    const glm::vec3& translation = std::get<1>(data);

    const glm::vec3& eulerRotation = std::get<2>(data);
    const glm::vec3& scaling = std::get<3>(data);
    const float transitionTime = std::get<4>(data);

    mOwnerComponent->AddMovementPoint(name, EulerAnglesTransform(translation, eulerRotation, scaling), transitionTime);
}
} // namespace Scripts
} // namespace EngineCore