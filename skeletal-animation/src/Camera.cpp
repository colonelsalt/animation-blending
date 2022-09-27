#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
	UpdateView();
	UpdateProjection();
}

Camera::Camera(const glm::vec3& position, float aspect)
	: m_Pos(position), m_Aspect(aspect)
{
	UpdateView();
	UpdateProjection();
}

Camera::Camera(float aspect)
	: m_Aspect(aspect)
{
	UpdateView();
	UpdateProjection();
}

Camera::Camera(const glm::vec3& position)
	: m_Pos(position)
{
	UpdateView();
	UpdateProjection();
}

void Camera::UpdateInput(GLFWwindow* window, float deltaTime)
{
	float speed = m_MovementSpeed * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_Pos += speed * m_Forward;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_Pos -= speed * m_Forward;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_Pos -= glm::normalize(glm::cross(m_Forward, m_Up)) * speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_Pos += glm::normalize(glm::cross(m_Forward, m_Up)) * speed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		m_Pos -= m_Up * speed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		m_Pos += m_Up * speed;

	UpdateView();
}

void Camera::OnMouseMove(double xPos, double yPos)
{
	if (m_IsFirstMouseInput)
	{
		m_LastMousePos = glm::vec2(xPos, yPos);
		m_IsFirstMouseInput = false;
	}

	float xOffset = (xPos - m_LastMousePos.x) * m_Sensitivity;
	float yOffset = (m_LastMousePos.y - yPos) * m_Sensitivity;
	
	m_LastMousePos = glm::vec2(xPos, yPos);

	m_Yaw += xOffset;
	m_Pitch += yOffset;
	
	m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);

	UpdateView();
}

void Camera::OnMouseScroll(double yOffset)
{
	m_Fov -= (float)yOffset;
	m_Fov = glm::clamp(m_Fov, 1.0f, 45.0f);

	UpdateProjection();
}

void Camera::UpdateView()
{
	glm::vec3 cameraDir;
	cameraDir.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	cameraDir.y = sin(glm::radians(m_Pitch));
	cameraDir.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Forward = glm::normalize(cameraDir);

	m_ViewMat = glm::lookAt(m_Pos, m_Pos + m_Forward, m_Up);
}

void Camera::UpdateProjection()
{
	m_ProjectionMat = glm::perspective(glm::radians(m_Fov), m_Aspect, 0.1f, 100.0f);
}
