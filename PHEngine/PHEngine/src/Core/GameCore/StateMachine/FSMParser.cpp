#include "FSMParser.h"
#include "Core/IoCore/FileFacade.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/CommonCore/XMLParserHelper.h"

#include <unordered_map>

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

   FSMParser::FSMP_Property GetPropertyAndAdvanceIt(XMLParserHelper::iterator_t& beginIt, const XMLParserHelper::iterator_t& endIt)
   {
      auto propertyStartNode = XMLParserHelper::GetItByNodeName(beginIt, endIt, PROPERTY_START_NODE_NAME);
      auto propertyEndNode = XMLParserHelper::GetItByNodeName(propertyStartNode, endIt, PROPERTY_END_NODE_NAME);
      ++propertyStartNode;

      FSMParser::FSMP_Property property;

      for (auto it = propertyStartNode; it != propertyEndNode; ++it)
      {
         const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

         std::string name, from, to, duration;

         if (EngineUtility::StartsWith(currentNodeStr, "name"))
         {
            property.Name = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "name");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "binding_name"))
         {
            property.BindingName = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "binding_name");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "type"))
         {
            property.Type = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "type");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "value"))
         {
            property.Value = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "value");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "state"))
         {
            property.State = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "state");
         }
         else
            assert((false, "unknown xml node."));
      }

      beginIt = propertyEndNode;
      return property;
   }

   FSMParser::FSMP_Binding GetBindingAndAdvanceIt(XMLParserHelper::iterator_t& beginIt, const XMLParserHelper::iterator_t& endIt)
   {
      auto bindingStartNode = XMLParserHelper::GetItByNodeName(beginIt, endIt, BINDING_START_NODE_NAME);
      auto bindingEndNode = XMLParserHelper::GetItByNodeName(bindingStartNode, endIt, BINDING_END_NODE_NAME);
      ++bindingStartNode;

      FSMParser::FSMP_Binding binding;

      for (auto it = bindingStartNode; it != bindingEndNode; ++it)
      {
         const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

         std::string name, from, to, duration;

         if (EngineUtility::StartsWith(currentNodeStr, "binding_name"))
         {
            binding.BindingName = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "binding_name");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "type"))
         {
            binding.Type = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "type");
         }
         else
            assert((false, "unknown xml node."));
      }

      beginIt = bindingEndNode;
      return binding;
   }

   FSMParser::FSMP_Transition GetTransitionAndAdvanceIt(XMLParserHelper::iterator_t& beginIt, const XMLParserHelper::iterator_t& endIt)
   {
      auto transitionStartNode = XMLParserHelper::GetItByNodeName(beginIt, endIt, TRANSITION_START_NODE_NAME);
      auto transitionEndNode = XMLParserHelper::GetItByNodeName(transitionStartNode, endIt, TRANSITION_END_NODE_NAME);
      ++transitionStartNode;

      FSMParser::FSMP_Transition transition;

      for (auto it = transitionStartNode; it != transitionEndNode; ++it)
      {
         const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

         std::string name, from, to, duration;

         if (EngineUtility::StartsWith(currentNodeStr, "name"))
         {
            transition.Name = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "name");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "from_state"))
         {
            transition.From = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "from_state");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "to_state"))
         {
            transition.To = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "to_state");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "duration"))
         {
            transition.Duration = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "duration");
         }
         else
            assert((false, "unknown xml node."));
      }

      beginIt = transitionEndNode;
      return transition;
   }

   std::shared_ptr<StateMachine> FSMParser::ParseFSMDescriptor(const std::string& relPathToFSM)
   {
      const std::string& absolutePath = EngineUtility::ConvertFromRelativeToAbsolutePath(relPathToFSM);
      FileFacade fileWorker(absolutePath);

      const size_t sizeOfSrc = fileWorker.GetFileSourceSize();
      assert(sizeOfSrc > 0);

      std::list<std::string> fileSource = fileWorker.GetFileSrc();

      // Collect FSM data
      {

         auto fsmStartNode = XMLParserHelper::GetItByNodeName(fileSource, FSM_START_NODE_NAME);
         auto fsmEndNode = XMLParserHelper::GetItByNodeName(fileSource, FSM_END_NODE_NAME);
         ++fsmStartNode;

         auto statesStartNode = XMLParserHelper::GetItByNodeName(fsmStartNode, fsmEndNode, STATES_START_NODE_NAME);
         auto statesEndNode = XMLParserHelper::GetItByNodeName(fsmStartNode, fsmEndNode, STATES_END_NODE_NAME);
         ++statesStartNode;

         for (auto it = statesStartNode; it != statesEndNode; ++it)
         {
            const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

            if (EngineUtility::StartsWith(currentNodeStr, "name"))
            {
               const std::string& name = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "name");
               mStates.emplace_back(FSMP_State(name));
            }
         }

         auto transitionsStartNode = XMLParserHelper::GetItByNodeName(statesEndNode, fsmEndNode, TRANSITIONS_START_NODE_NAME);
         auto transitionsEndNode = XMLParserHelper::GetItByNodeName(transitionsStartNode, fsmEndNode, TRANSITIONS_END_NODE_NAME);
         ++transitionsStartNode;

         for (auto it = transitionsStartNode; it != transitionsEndNode; ++it)
         {
            mTransitions.emplace_back(GetTransitionAndAdvanceIt(it, transitionsEndNode));
         }

         auto bindingsStartNode = XMLParserHelper::GetItByNodeName(transitionsEndNode, fsmEndNode, BINDINGS_START_NODE_NAME);
         auto bindingsEndNode = XMLParserHelper::GetItByNodeName(bindingsStartNode, fsmEndNode, BINDINGS_END_NODE_NAME);
         ++bindingsStartNode;

         for (auto it = bindingsStartNode; it != bindingsEndNode; ++it)
         {
            mBindings.emplace_back(GetBindingAndAdvanceIt(it, bindingsEndNode));
         }

         auto propertiesStartNode = XMLParserHelper::GetItByNodeName(bindingsEndNode, fsmEndNode, PROPERTIES_START_NODE_NAME);
         auto propertiesEndNode = XMLParserHelper::GetItByNodeName(propertiesStartNode, fsmEndNode, PROPERTIES_END_NODE_NAME);
         ++propertiesStartNode;

         for (auto it = propertiesStartNode; it != propertiesEndNode; ++it)
         {
            mProperties.emplace_back(GetPropertyAndAdvanceIt(it, propertiesEndNode));
         }
      }

      return BuildFSM();
   }

   std::shared_ptr<StatePropertyBinding> CreatePropertyBinding(const FSMParser::FSMP_Binding& binding)
   {
      std::shared_ptr<StatePropertyBinding> result;

      if ("animation" == binding.Type) {
         result = std::make_shared<AnimationPropertyBinding>(binding.BindingName);
      }
      else if ("float" == binding.Type) {
         result = std::make_shared<FloatPropertyBinding>(binding.BindingName);
      }
      else 
      {
         assert((false, "unknown binding type."));
      }

      return result;
   }

   BaseStateProperty* CreateProperty(const FSMParser::FSMP_Property& property, const std::unordered_map<std::string, std::shared_ptr<StatePropertyBinding>>& bindings)
   {
      BaseStateProperty* result = nullptr;

      assert(bindings.count(property.BindingName));

      if ("animation" == property.Type) 
      {
         result = new StateProperty<StatePropertyType::Animation>(property.Value, std::static_pointer_cast<AnimationPropertyBinding>(bindings.at(property.BindingName)));
      }
      else if ("float" == property.Type)
      {
         const float value = std::stof(property.Value);
         result = new StateProperty<StatePropertyType::Float>(value, std::static_pointer_cast<FloatPropertyBinding>(bindings.at(property.BindingName)));
      }
      else 
      {
         assert((false, "unknown binding type."));
      }

      return result;
   }

   std::shared_ptr<StateMachine> FSMParser::BuildFSM()
   {
      std::unordered_map<std::string, State*> states;
      std::unordered_map<std::string, std::shared_ptr<StatePropertyBinding>> bindings;

      for (const auto& item : mStates)
      {
         states[item.Name] = new State(item.Name);
      }

      auto fsm = std::make_shared<StateMachine>(states[mStates[0].Name]);

      for (const auto& item : mTransitions)
      {
         assert(states.count(item.From));
         auto transition = StateTransition(states.at(item.From), states.at(item.To), std::stof(item.Duration));
         states.at(item.From)->AddStateTransition(transition);
      }

      for (const auto& item : mBindings)
      {
         bindings[item.BindingName] = CreatePropertyBinding(item);
         fsm->AddPropertyBinding(item.BindingName, bindings.at(item.BindingName));
      }

      for (const auto& item : mProperties)
      {
         BaseStateProperty* property = CreateProperty(item, bindings);
         states[item.State]->AddStateProperty(property);
      }

      return fsm;
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
