#include "rzpch.h"
#include "FPSCamera.h"
#include <GLFW/glfw3.h>
#include "Razor/Core/Window.h"
#include "Editor/Components/Viewport.h"
#include "Razor/Application/Application.h"
#include "Editor/Editor.h"
#include "Editor/Components/Viewport.h"
#include "glm/gtx/string_cast.hpp"

namespace Razor {

	FPSCamera::FPSCamera(Window* window) :
		Camera(window),
		viewport(nullptr),
		sensitivity(0.3f),
		view_friction(0.0f),
		move_friction(10.0f),
		mouse(glm::vec2()),
		mouse_offset(glm::vec2()),
		constrain_pitch(true)
	{
		projection = glm::perspective(fov, 16.0f / 9.0f, clip_near, clip_far);
		updateVectors();
		view = glm::lookAt(position, position + direction, up);
	}

	FPSCamera::~FPSCamera()
	{
	}

	void FPSCamera::updateVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction = glm::normalize(front);

		right = glm::normalize(glm::cross(direction, world_up));
		up = glm::normalize(glm::cross(right, direction));
	}

	// #.0 "%"; [YELLOW] [>= 50] #.0 "%";

	void FPSCamera::update(double dt)
	{
		delta = (float)dt;

		if (viewport == nullptr)
			viewport = (Viewport*)Application::Get().getEditor()->getComponents()["Viewport"];
	}

	void FPSCamera::onEvent(Window * window)
	{
		GLFWwindow* native = (GLFWwindow*)window->GetNativeWindow();

		if (viewport != nullptr) 
			aspect_ratio = viewport->getSize().x / viewport->getSize().y;

		updateVectors();

		if (glfwGetKey(native, GLFW_KEY_KP_5) == GLFW_PRESS && viewport->isHovered())
			mode = mode == Camera::Mode::ORTHOGRAPHIC ? Camera::Mode::PERSPECTIVE : Camera::Mode::ORTHOGRAPHIC;

		if (capture)
		{
			if (glfwGetKey(native, GLFW_KEY_W) == GLFW_PRESS)
				onKeyPressed(Direction::FORWARD);
			if (glfwGetKey(native, GLFW_KEY_S) == GLFW_PRESS)
				onKeyPressed(Direction::BACKWARD);
			if (glfwGetKey(native, GLFW_KEY_A) == GLFW_PRESS)
				onKeyPressed(Direction::LEFT);
			if (glfwGetKey(native, GLFW_KEY_D) == GLFW_PRESS)
				onKeyPressed(Direction::RIGHT);
			if (glfwGetKey(native, GLFW_KEY_Q) == GLFW_PRESS)
				onKeyPressed(Direction::UP);
			if (glfwGetKey(native, GLFW_KEY_E) == GLFW_PRESS)
				onKeyPressed(Direction::DOWN);
		}
		
		switch (mode) {
			case Mode::ORTHOGRAPHIC:
				projection = glm::ortho(1.5f * aspect_ratio, -1.5f * aspect_ratio, 1.5f, -1.5f, -100.0f, 100.0f);
				break;

			case Mode::PERSPECTIVE:
				projection = glm::perspectiveFov(fov, viewport->getSize().x, viewport->getSize().y, clip_near, clip_far);
				break;
		}

		velocity *= 1.0f / (1.0f + delta * move_friction);
		position += velocity * delta;
		
		view = glm::lookAt(position, position + direction, up);
	}

	void FPSCamera::onKeyPressed(Direction dir)
	{
		if (dir == Direction::FORWARD)
			velocity += direction * delta * speed;
		if (dir == Direction::BACKWARD)
			velocity -= direction * delta * speed;
		if (dir == Direction::LEFT)
			velocity += right * delta * speed;
		if (dir == Direction::RIGHT)
			velocity -= right * delta * speed;
		if (dir == Direction::DOWN)
			velocity += up * delta * speed;
		if (dir == Direction::UP)
			velocity -= up * delta * speed;
	}

	void FPSCamera::onMouseMoved(glm::vec2& pos, bool constrain)
	{
		if (first)
		{
			last_pos = pos;
			first = false;
		}

		mouse_offset = pos - last_pos;
		last_pos = pos;

		if (capture)
		{
			yaw -= mouse_offset.x * sensitivity;
			pitch -= mouse_offset.y * sensitivity;

			if (constrain)
			{
				if (pitch > 89.0f)  pitch = 89.0f;
				if (pitch < -89.0f) pitch = -89.0f;
			}

			updateVectors();
		}
	}

	void FPSCamera::onMouseScrolled(glm::vec2& offset)
	{
		if (capture)
		{
			speed += offset.y / 10.0f * speed_factor;

			if (speed < min_speed)
				speed = min_speed;

			if (speed > max_speed)
				speed = max_speed;
		}
	}

	void FPSCamera::onMouseDown(int button)
	{
		if (button == 1 && viewport->isHovered())
		{
			capture = true;
			glfwSetInputMode((GLFWwindow*)window->GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	void FPSCamera::onMouseUp(int button)
	{
		if (button == 1)
		{
			capture = false;
			glfwSetInputMode((GLFWwindow*)window->GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	void FPSCamera::onWindowResized(const glm::vec2& size)
	{
		Viewport* vp = (Viewport*)Application::Get().getEditor()->getComponents()["Viewport"];

		if (vp != nullptr) {
			aspect_ratio = (float)vp->getSize().x / (float)vp->getSize().y;
		}

	}

}
