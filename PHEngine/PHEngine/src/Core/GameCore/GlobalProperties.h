#pragma once

#include "InputData.h"

#include <memory>

namespace Game
{

	class GlobalProperties
	{
	private:
			
		static std::unique_ptr<GlobalProperties> m_globalProperties;

		InputData m_inputData;

		GlobalProperties();
		
	public:

		~GlobalProperties();

		static inline std::unique_ptr<GlobalProperties>& GetInstance() {

			if (!m_globalProperties)
				m_globalProperties = std::unique_ptr<GlobalProperties>(new GlobalProperties());

			return m_globalProperties;
		}

		inline InputData& GetInputData() {

			return m_inputData;
		}
	};

}

