#pragma once
#include "Core/GameCore/Tweener/Tweener.h"

#include <memory>
#include <vector>

namespace EngineCore
{

   class TweenerParser
   {
   public:
      struct TweenerParser_State
      {
         std::string Name;

         TweenerParser_State(const std::string& name)
            : Name(name)
         {
         }
      };

      struct TweenerParser_Transition
      {
         std::string Name;
         std::string From;
         std::string To;
         std::string Duration;

         TweenerParser_Transition(const std::string& name, const std::string& from, const std::string& to, const std::string& duration)
            : Name(name)
            , From(from)
            , To(to)
            , Duration(duration)
         {
         }

         TweenerParser_Transition() = default;
      };

      struct TweenerParser_Binding
      {
         std::string BindingName;
         std::string Type;

         TweenerParser_Binding(const std::string& bindingName, const std::string& type)
            : BindingName(bindingName)
            , Type(type)
         {
         }
         TweenerParser_Binding() = default;
      };

      struct TweenerParser_Property
      {
         std::string Name;
         std::string BindingName;
         std::string Type;
         std::string Value;
         std::string State;

         TweenerParser_Property(const std::string& name, const std::string& bindingName,
            const std::string& type, const std::string& value, const std::string& state)
            : Name(name)
            , BindingName(bindingName)
            , Type(type)
            , Value(value)
            , State(state)
         {
         }

         TweenerParser_Property() = default;
      };

   private:

      std::vector<TweenerParser_State> mStates;
      std::vector<TweenerParser_Transition> mTransitions;
      std::vector<TweenerParser_Binding> mBindings;
      std::vector<TweenerParser_Property> mProperties;

   public:
      std::shared_ptr<Tweener> ParseTweenerDescriptor(const std::string& relPathTweener);

   private:

      std::shared_ptr<Tweener> BuildTweener(const std::string& relPathTweener);

   };

}

