#include "rzpch.h"
#include "Camera.h"
#include <GLFW/glfw3.h>
#include "Razor/Core/Window.h"

#include "Editor/Components/Viewport.h"
#include "Razor/Application/Application.h"
#include "Editor/Editor.h"

namespace Razor {

	Camera::Camera(Window* window) : 

		window(window),
		position(glm::vec3(0.0f, 0.0f, 0.0f)),
		velocity(glm::vec3(0.0f, 0.0f, 0.0f)),
		direction(glm::vec3(0.0f, 0.0f, -1.0f)),
		up(glm::vec3(0.0f, 0.0f, 0.0)),
		right(glm::vec3(0.0f, 0.0f, 0.0)),
		world_up(glm::vec3(0.0f, 1.0f, 0.0)),

		yaw(90.0f),
		pitch(0.0f),
		aspect_ratio(16.0f / 9.0f),
		clip_near(0.01f),
		clip_far(1000.0f),
		fov(68.0f),

		capture(false),
		
		delta(1.0f / 60.0f),
		mode(Mode::PERSPECTIVE),
		view(glm::mat4(1.0f)),

		speed(125.0f),
		speed_factor(10.0f),
		min_speed(0.1f),
		max_speed(1000.0f)
	{
		projection = glm::perspective(fov, aspect_ratio, clip_near, clip_far);
	}

	Camera::~Camera()
	{
	}
	

}
