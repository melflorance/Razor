#include "rzpch.h"
#include "ShadersManager.h"

namespace Razor {

	ShadersManager::ShadersManager()
	{
	}

	ShadersManager::~ShadersManager()
	{
	}

	Shader* ShadersManager::createShader(const std::string& name)
	{
		Shader* shader = new Shader(name);
		shaders[name] = shader;
		
		return shader;
	}

	bool ShadersManager::removeShader(const std::string& name)
	{
		auto shader = shaders.find(name);

		if (shader != shaders.end())
			return shaders.erase(shader->first) == 1;

		return false;
	}

	Shader* ShadersManager::getShader(const std::string& name)
	{
		auto shader = shaders.find(name);

		if (shader != shaders.end())
			return shader->second;

		return nullptr;
	}

}
