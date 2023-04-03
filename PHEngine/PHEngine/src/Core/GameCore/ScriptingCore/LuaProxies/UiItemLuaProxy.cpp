#include "UiItemLuaProxy.h"
#include "Core/GameCore/GUI/UiElements/UiItem.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Scene.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore
{
    namespace Scripts
    {
        UiItemLuaProxy::UiItemLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiItem> &ownerUiItem)
            : UiItemBaseLuaProxy(ownerUiItem)
        {
        }

        void UiItemLuaProxy::OnLuaThreadDataUpdated(const std::string &jsonParameters)
        {
            UiItemBaseLuaProxy::OnLuaThreadDataUpdated(jsonParameters);
        }

        std::string UiItemLuaProxy::GetGameThreadData()
        {
            const auto &baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
            //auto jsonObj = nlohmann::json::parse(baseJsonStr);
            //jsonObj["test"] = 5;
            //return jsonObj.dump();
            return baseJsonStr;
        }
    }
}
