#pragma once

#include <memory>
#include <string>

namespace EngineCore
{
    class Scene;
}

namespace EngineCore
{
    namespace Scripts
    {
        class LuaScriptProcessor;

        class IReplicatorFactory
        {
        public:
            /*! \brief Creates replicator instance
             *
             *  Creates unique luaProxyId, replicator instance on game thread, registers it in Scene and returns
             *  luaProxyId back to lua thread code
             */
            virtual int32_t CreateReplicator(const std::weak_ptr<::EngineCore::Scene> &sceneWp,
                                             const std::weak_ptr<LuaScriptProcessor> &luaScriptProcessorWp,
                                             const std::string& jsonParamsStr) const = 0;
        };
    }
} // namespace EngineCore