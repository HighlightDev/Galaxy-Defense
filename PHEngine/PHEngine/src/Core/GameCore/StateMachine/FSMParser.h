#pragma once
#include "Core/GameCore/StateMachine/StateMachine.h"

#include <memory>
#include <vector>

namespace Game
{

   class FSMParser
   {
      struct FSMP_State
      {
         std::string Name;

         FSMP_State(const std::string& name)
            : Name(name)
         {
         }
      };

      struct FSMP_Transition
      {
         std::string Name;
         std::string From;
         std::string To;
         std::string Duration;

         FSMP_Transition(const std::string& name, const std::string& from, const std::string& to, const std::string& duration)
            : Name(name)
            , From(from)
            , To(to)
            , Duration(duration)
         {
         }
      };

      struct FSMP_Binding
      {
         std::string BindingName;
         std::string Type;

         FSMP_Binding(const std::string& bindingName, const std::string& type)
            : BindingName(bindingName)
            , Type(type)
         {
         }
      };

      struct FSMP_Property
      {
         std::string Name;
         std::string MutualName;
         std::string BindingName;
         std::string Type;
         std::string Value;
         std::string State;

         FSMP_Property(const std::string& name, const std::string& mutualName, const std::string& bindingName,
            const std::string& type, const std::string& value, const std::string& state)
            : Name(name)
            , MutualName(mutualName)
            , BindingName(bindingName)
            , Type(type)
            , Value(value)
            , State(state)
         {
         }
      };

      std::vector<FSMP_State> mStates;
      std::vector<FSMP_Transition> mTransitions;
      std::vector<FSMP_Binding> mBindings;
      std::vector<FSMP_Property> mProperties;

   public:
      std::shared_ptr<StateMachine> ParseFSMDescriptor(const std::string& relPathToFSM);
   };

}

