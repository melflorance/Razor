#include "rzpch.h"
#include "Shader.h"
#include "Razor/Filesystem/File.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Razor {

	Shader::Shader(const std::string& name) : 
		name(name),
		dirty(true)
	{
		shadersDir = "F:/Razor/Razor/Razor/src/Shaders/";
		load();
	}

	Shader::~Shader()
	{
	}

	void Shader::bind()
	{
		glUseProgram(program);
	}

	void Shader::unbind()
	{
		glUseProgram(0);
	}

	bool Shader::load()
	{
		status &= ~(Status::Loaded);

		auto frag_data = File::read(shadersDir + name + ".frag");
		auto vert_data = File::read(shadersDir + name + ".vert");

		if (frag_data.empty() || vert_data.empty())
			return false;

		sources[Type::VERTEX] = vert_data;
		sources[Type::FRAGMENT] = frag_data;

		int size = 0;
		size += Utils::getFileSize(shadersDir + name + ".frag");
		size += Utils::getFileSize(shadersDir + name + ".vert");

		Log::info("Loaded shader: %s %s", name.c_str(), Utils::bytesToSize(size).c_str());

		parseIncludes();

		return status & Status::Loaded;
	}

	void Shader::defineConstant(Type type, const std::string& name, int value)
	{
		constants[std::make_pair(type, name)] = value;
	}

	void Shader::replaceConstants()
	{
		ConstantsMap::iterator it;
		
		for (it = constants.begin(); it != constants.end(); ++it)
		{
			switch (it->first.first)
			{
			case Type::FRAGMENT:
				std::string out;
				auto data = sources[Type::FRAGMENT];
				auto lines = Utils::splitString(data, "\n");

				for (auto& line : lines)
				{
					if (line.rfind("#define", 0) == 0)
					{
						auto parts = Utils::splitString(line, " ");

						if (parts.at(1) == it->first.second)
							line = "#define " + it->first.second + " " + std::to_string(it->second);
					}

					out += line + "\n";
				}

				sources[Type::FRAGMENT] = out;

				break;
			}
		}
	}

	void Shader::parseIncludes()
	{
		std::string out;
		auto data = sources[Type::FRAGMENT];
		auto lines = Utils::splitString(data, "\n");

		for (auto& line : lines)
		{
			if (line.rfind("#include", 0) == 0)
			{
				auto parts = Utils::splitString(line, " ");

				std::string ln;
				std::string str;
				std::ifstream stream;
				stream.open(shadersDir + parts[1]);

				if (stream.is_open())
				{
					while (getline(stream, ln))
						str += ln + "\n";

					stream.close();
				}

				line = str;
			}

			out += line + "\n";
		}

		sources[Type::FRAGMENT] = out;
	}

	bool Shader::compile()
	{
		status &= ~(Status::Compiled);

		SourcesMap::iterator it = sources.begin();

		for (; it != sources.end(); it++)
		{
			if (!it->second.empty())
			{
				int shader = glCreateShader((GLenum)it->first);
				shaders[it->first] = shader;

				const char* str = it->second.c_str();
				glShaderSource(shader, 1, &str, NULL);

				glCompileShader(shader);	

				int status;
				char error[512];
				glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

				if (!status) {
					glGetShaderInfoLog(shader, 512, NULL, error);
					Log::error("Shader compilation failed: %s.", error);
				}
				else {
					std::string type;
					switch (it->first) {
						case Type::VERTEX: type = "Vertex"; break;
						case Type::FRAGMENT: type = "Fragment";  break;
					}
					Log::info("Compiled %s shader: %s.", type.c_str(), name.c_str());
					status |= Status::Compiled;
				}
			}
		}

		return status & Status::Compiled;
	}

	bool Shader::link()
	{
		status &= ~(Status::Linked);

		program = glCreateProgram();
		ShadersMap::iterator it = shaders.begin();

		for (; it != shaders.end(); it++)
			glAttachShader(program, it->second);

		glLinkProgram(program);

		int status;
		char error[512];
		glGetProgramiv(program, GL_LINK_STATUS, &status);

		if (!status) {
			glGetProgramInfoLog(program, 512, NULL, error);
			Log::error("Shader program linking failed: %s.", error);
		}
		else {
			Log::info("Linked shader: %s.", name.c_str());
			status |= Status::Linked;
		}

		for (; it != shaders.end(); it++)
			glDeleteShader(it->second);

		return status & Status::Linked;
	}



	void Shader::setUniform1i(const std::string & name, int value){
		glUniform1i(getUniformLocation(name), value);
	}
	void Shader::setUniform1f(const std::string& name, float value) {
		glUniform1f(getUniformLocation(name), value);
	}
	void Shader::setUniform2f(const std::string& name, float x, float y) {
		glUniform2f(getUniformLocation(name), x, y);
	}
	void Shader::setUniform2f(const std::string& name, const glm::vec2 & value) {
		glUniform2f(getUniformLocation(name), value.x, value.y);
	}
	void Shader::setUniform3f(const std::string& name, const glm::vec3& value) {
		glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
	}
	void Shader::setUniform3f(const std::string& name, float x, float y, float z) {
		glUniform3f(getUniformLocation(name), x, y, z);
	}
	void Shader::setUniform4f(const std::string& name, const glm::vec4& value) {
		glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
	}
	void Shader::setUniform4f(const std::string& name, float x, float y, float z, float w) {
		glUniform4f(getUniformLocation(name), x, y, z, w);
	}
	void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix) {
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	unsigned int Shader::getUniformLocation(const std::string& uniform_name)
	{

		int location = glGetUniformLocation(program, uniform_name.c_str());

		if (location == -1)
			Log::warn("Shader \"%s\" Invalid uniform location: %s", name.c_str(), uniform_name.c_str());

		return location;
	}

}
