#include "GlobalProperties.h"

namespace Game
{

	std::unique_ptr<GlobalProperties> GlobalProperties::m_globalProperties;

	GlobalProperties::GlobalProperties()
		: m_inputData()
	{
	}

	GlobalProperties::~GlobalProperties()
	{
	}

}