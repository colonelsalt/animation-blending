#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera();
	Camera(const glm::vec3& position, float aspect);
	Camera(float aspect);
	Camera(const glm::vec3& position);

	void UpdateInput(GLFWwindow* window, float deltaTime);

	void OnMouseMove(double xPos, double yPos);
	void OnMouseScroll(double yOffset);

	const glm::mat4& GetViewMatrix() const { return m_ViewMat; }
	const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMat; }
	const glm::vec3& GetPosition() const { return m_Pos; }

private:
	void UpdateView();
	void UpdateProjection();

private:
	glm::mat4 m_ViewMat;
	glm::mat4 m_ProjectionMat;

	float m_Fov = 45.0f;
	float m_Aspect = 800.0f / 600.0f;

	float m_Pitch = 0.0f;
	float m_Yaw = -90.0f;

	float m_Sensitivity = 0.1f;
	float m_MovementSpeed = 2.5f;

	glm::vec2 m_LastMousePos = glm::vec2(400.0f, 300.0f);

	glm::vec3 m_Pos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 m_Forward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

	bool m_IsFirstMouseInput = true;
};