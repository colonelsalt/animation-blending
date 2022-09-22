#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include "Animation/Animator.h"
#include "Animation/BlendNode.h"

static Camera s_Camera({ 0.0f, 4.0f, 13.0f });

static float s_DeltaTime = 0.0f;
static float s_LastFrameTime = 0.0f;

static float s_MoveSpeed = 0.0f;



void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
	s_Camera.UpdateInput(window, s_DeltaTime);
}

void OnMouseMove(GLFWwindow* window, double xPos, double yPos)
{
	s_Camera.OnMouseMove(xPos, yPos);
}

void OnMouseScroll(GLFWwindow* window, double xOffset, double yOffset)
{
	s_Camera.OnMouseScroll(yOffset);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGLStuff", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialise GLAD." << std::endl;
		return 1;
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, OnMouseMove);
	glfwSetScrollCallback(window, OnMouseScroll);

	glViewport(0, 0, 800, 600);
	glEnable(GL_DEPTH_TEST);

	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

	Shader shader("assets/shaders/AnimVert.glsl", "assets/shaders/MeshFrag.glsl");
	shader.Bind();

	std::shared_ptr<JointDirectory> jointDirectory = std::make_shared<JointDirectory>();

	Model bossModel("assets/models/boss/The Boss.fbx", jointDirectory);

	AnimationClip walkClip("assets/models/boss/walking.fbx", jointDirectory, true);
	AnimationClip runClip("assets/models/boss/running.fbx", jointDirectory, true);
	
	BlendNode locomotionNode(&walkClip, &runClip);
	AnimationState locomotionState("Locomotion", &locomotionNode, true);
	locomotionState.AddVar<float>("MoveSpeed", { 0.2f, 0.0f, 1.0f });

	Animator* animator = Animator::GetInstance();
	animator->SetDirectory(jointDirectory);
	animator->SetState(&locomotionState);

	while (!glfwWindowShouldClose(window))
	{
		float time = glfwGetTime();
		s_DeltaTime = time - s_LastFrameTime;
		s_LastFrameTime = time;

		ProcessInput(window);
		s_Camera.UpdateInput(window, s_DeltaTime);

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			s_MoveSpeed += 0.3f * s_DeltaTime;
			std::cout << "Move speed: " << s_MoveSpeed << std::endl;
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			s_MoveSpeed -= 0.3f * s_DeltaTime;
			std::cout << "Move speed: " << s_MoveSpeed << std::endl;
		}
		s_MoveSpeed = glm::clamp(s_MoveSpeed, 0.0f, 1.0f);
		animator->SetFloat("MoveSpeed", s_MoveSpeed);

		animator->Update(s_DeltaTime);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.04f));

		shader.Bind();
		shader.SetMat4("u_Model", model);
		shader.SetMat4("u_View", s_Camera.GetViewMatrix());
		shader.SetMat4("u_Projection", s_Camera.GetProjectionMatrix());

		shader.SetVec3("u_DirLight.Direction", { 0.1f, -1.0f, 0.3f });
		shader.SetVec3("u_DirLight.Ambient", { 0.2f, 0.2f, 0.2f });
		shader.SetVec3("u_DirLight.Diffuse", { 0.8f, 0.8f, 0.8f });
		shader.SetVec3("u_DirLight.Specular", { 1.0f, 1.0f, 1.0f });

		auto& skinningMatrices = animator->GetSkinningMatrices();
		for (uint32_t i = 0; i < skinningMatrices.size(); i++)
			shader.SetMat4("u_SkinningMatrices[" + std::to_string(i) + "]", skinningMatrices[i]);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		bossModel.Draw(shader);
		
		//backpack.Draw(shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}