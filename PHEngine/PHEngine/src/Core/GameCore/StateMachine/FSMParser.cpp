#include "FSMParser.h"
#include "Core/IoCore/FileFacade.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/CommonCore/ParsingXmlStructure.h"

using namespace Common;

namespace Game
{
#define FSM_START_NODE_NAME                  "<FSM>"
#define FSM_END_NODE_NAME                    "</FSM>"
#define STATES_START_NODE_NAME               "<states>"
#define STATES_END_NODE_NAME                 "</states>"
#define STATE_START_NODE_NAME                "<state>"
#define STATE_END_NODE_NAME                  "</state>"
#define TRANSITIONS_START_NODE_NAME          "<transitions>"
#define TRANSITIONS_END_NODE_NAME            "</transitions>"
#define TRANSITION_START_NODE_NAME           "<transition>"
#define TRANSITION_END_NODE_NAME             "</transition>"
#define BINDINGS_START_NODE_NAME             "<bindings>"
#define BINDINGS_END_NODE_NAME               "</bindings>"
#define BINDING_START_NODE_NAME              "<binding>"
#define BINDING_END_NODE_NAME                "</binding>"
#define PROPERTIES_START_NODE_NAME           "<properties>"
#define PROPERTIES_END_NODE_NAME             "</properties>"
#define PROPERTY_START_NODE_NAME             "<property>"
#define PROPERTY_END_NODE_NAME               "</property>"

   std::shared_ptr<StateMachine> FSMParser::ParseFSMDescriptor(const std::string& relPathToFSM)
   {
      const std::string& absolutePath = EngineUtility::ConvertFromRelativeToAbsolutePath(relPathToFSM);
      FileFacade fileWorker(absolutePath);

      const size_t sizeOfSrc = fileWorker.GetFileSourceSize();
      assert(sizeOfSrc > 0);

      std::list<std::string> fileSource = fileWorker.GetFileSrc();

      std::string materialName;
      std::string materialShaderName;

      auto fsmStartNode = GetItByNodeName(fileSource, FSM_START_NODE_NAME);
      auto fsmEndNode = GetItByNodeName(fileSource, FSM_END_NODE_NAME);
      ++fsmStartNode;

      auto statesStartNode = GetItByNodeName(fsmStartNode, fsmEndNode, STATES_START_NODE_NAME);
      auto statesEndNode = GetItByNodeName(fsmStartNode, fsmEndNode, STATES_END_NODE_NAME);
      ++statesStartNode;

      for (auto it = statesStartNode; it != statesEndNode; ++it)
      {
         const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

         if (EngineUtility::StartsWith(currentNodeStr, "name"))
         {
            const std::string& name = GetPropertyNodeByName(currentNodeStr, "name");
            mStates.emplace_back(FSMP_State(name));
         }
      }

      auto transitionsStartNode = GetItByNodeName(statesEndNode, fsmEndNode, TRANSITIONS_START_NODE_NAME);
      auto transitionsEndNode = GetItByNodeName(transitionsStartNode, fsmEndNode, TRANSITIONS_END_NODE_NAME);
      ++transitionsStartNode;

      for (auto it = transitionsStartNode; it != transitionsEndNode; ++it)
      {
         const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

         std::string name, from, to, duration;

         if (EngineUtility::StartsWith(currentNodeStr, "name"))
         {
            name = GetPropertyNodeByName(currentNodeStr, "name");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "from"))
         {
            from = GetPropertyNodeByName(currentNodeStr, "from");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "to"))
         {
            to = GetPropertyNodeByName(currentNodeStr, "to");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "duration"))
         {
            duration = GetPropertyNodeByName(currentNodeStr, "duration");
         }
      }

      State* stub;
      return std::make_shared<StateMachine>(stub);
   }

#undef FSM_START_NODE_NAME             
#undef FSM_END_NODE_NAME               
#undef STATES_START_NODE_NAME          
#undef STATES_END_NODE_NAME            
#undef STATE_START_NODE_NAME           
#undef STATE_END_NODE_NAME             
#undef TRANSITIONS_START_NODE_NAME     
#undef TRANSITIONS_END_NODE_NAME       
#undef TRANSITION_START_NODE_NAME      
#undef TRANSITION_END_NODE_NAME        
#undef BINDINGS_START_NODE_NAME        
#undef BINDINGS_END_NODE_NAME          
#undef BINDING_START_NODE_NAME         
#undef BINDING_END_NODE_NAME           
#undef PROPERTIES_START_NODE_NAME      
#undef PROPERTIES_END_NODE_NAME        
#undef PROPERTY_START_NODE_NAME        
#undef PROPERTY_END_NODE_NAME          
}
