#pragma once
#include "ShaderType.h"
#include "ShaderPredefine.h"
#include "ShaderPredefineUtility.h"
#include "ShaderParams.h"
#include "IShader.h"

#include <vector>
#include <xutility>

namespace Graphics
{
	namespace OpenGL
	{
		class Shader
         : public IShader
		{
		public:

			Shader(const ShaderParams& params);

			virtual ~Shader();

		private:

         ShaderParams m_shaderParams;
			
			std::vector<ShaderDefineConstant> m_defineConstantParameters;
         std::vector<ShaderDefine> m_defines;

		protected:
         void LoadSubroutineIndex(ShaderType shaderType, int32_t countIndices, int32_t subroutineIndex) const;
         int32_t GetSubroutineIndex(ShaderType shaderType, const std::string& subroutineName) const;

         template <typename ValueType>
         void DefineConstant(ShaderType shaderType, const std::string& name, ValueType&& value)
         {
            std::string formatedValue = MacroConverter<ValueType>::GetValue(std::forward<ValueType>(value));
            m_defineConstantParameters.emplace_back(ShaderDefineConstant(name, formatedValue, shaderType));
         }

         void Define(ShaderType shaderType, const std::string& name);

         void Undefine(ShaderType shaderType, const std::string& name);

      private:

         bool LoadShadersSourceToGpu();

		public:

         // Init should be called in child constructor!
         virtual void ShaderInit();
         virtual void ProcessAllPredefines() override;

         ShaderParams GetShaderParams() const;

#if DEBUG
			bool RecompileShader();
#endif

		};
	}
}