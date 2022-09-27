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

	GLFWwindow* window = glfwCreateWindow(800, 600, "AnimationSystem", nullptr, nullptr);
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

	// Clips --------------
	AnimationClip idleClip("assets/models/boss/idle (2).fbx", jointDirectory, false, true);
	AnimationClip walkClip("assets/models/boss/walking.fbx", jointDirectory, true);
	AnimationClip runClip("assets/models/boss/running.fbx", jointDirectory, true);
	AnimationClip haltClip("assets/models/boss/run to stop.fbx", jointDirectory, true, true);
	AnimationClip jumpClip("assets/models/boss/jumping up.fbx", jointDirectory, false, true);
	AnimationClip fallClip("assets/models/boss/falling idle.fbx", jointDirectory, false, true);
	AnimationClip landClip("assets/models/boss/hard landing.fbx", jointDirectory, false, true);
	AnimationClip rollClip("assets/models/boss/falling to roll.fbx", jointDirectory, true, true);

	BlendNode locomotionNode(&walkClip, &runClip);

	// States --------------
	AnimationState idleState("Idle", &idleClip, true);

	AnimationState locomotionState("Locomotion", &locomotionNode, true, false);
	locomotionState.AddVar<float>("MoveSpeed", { 0.2f, 0.0f, 1.0f });

	AnimationState haltState("Halting", &haltClip);
	AnimationState jumpState("Jumping", &jumpClip);
	AnimationState fallState("Falling", &fallClip);
	AnimationState landState("Landing", &landClip);
	AnimationState rollState("Rolling", &rollClip);
	
	fallState.SetCompletionTime(0.3f);
	rollState.SetCompletionTime(0.7f);

	// Transitions ----------
	Transition idleToMove(&idleState, &locomotionState, 0.3f);
	idleState.AddTriggerTransition("MoveTrigger", &idleToMove);

	Transition moveToIdle(&locomotionState, &idleState, 0.3f);
	locomotionState.AddTriggerTransition("IdleTrigger", &moveToIdle);

	Transition runToHalt(&locomotionState, &haltState, 0.1f);
	locomotionState.AddTriggerTransition("HaltTrigger", &runToHalt);
	
	Transition haltToIdle(&haltState, &idleState, 0.1f);
	haltState.SetOnCompleteTransition(&haltToIdle);

	Transition idleToJump(&idleState, &jumpState, 0.2f);
	idleState.AddTriggerTransition("JumpTrigger", &idleToJump);

	Transition runToJump(&locomotionState, &jumpState, 0.2f);
	locomotionState.AddTriggerTransition("JumpTrigger", &runToJump);

	Transition jumpToFall(&jumpState, &fallState, 0.2f);
	jumpState.SetOnCompleteTransition(&jumpToFall);

	Transition fallToLand(&fallState, &landState, 0.1f);
	fallState.SetOnCompleteTransition(&fallToLand);

	Transition fallToRoll(&fallState, &rollState, 0.3f);
	fallState.AddTriggerTransition("MoveTrigger", &fallToRoll);

	Transition landToIdle(&landState, &idleState, 0.3f);
	landState.SetOnCompleteTransition(&landToIdle);

	Transition rollToMove(&rollState, &locomotionState, 0.3f);
	rollState.SetOnCompleteTransition(&rollToMove);

	Animator* animator = Animator::GetInstance();
	animator->SetDirectory(jointDirectory);
	animator->SetState(&idleState);

	while (!glfwWindowShouldClose(window))
	{
		float time = glfwGetTime();
		s_DeltaTime = (time - s_LastFrameTime);
		s_LastFrameTime = time;

		ProcessInput(window);
		s_Camera.UpdateInput(window, s_DeltaTime);

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			s_MoveSpeed += 0.3f * s_DeltaTime;
			//std::cout << "Move speed: " << s_MoveSpeed << std::endl;
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			s_MoveSpeed -= 0.3f * s_DeltaTime;
			//std::cout << "Move speed: " << s_MoveSpeed << std::endl;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			if (s_MoveSpeed > 0.1f)
			{
				s_MoveSpeed = 0.0f;
				animator->SetFloat("MoveSpeed", s_MoveSpeed);
				animator->SetTrigger("HaltTrigger");
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			/*s_MoveSpeed = 0.0f;
			animator->SetFloat("MoveSpeed", s_MoveSpeed);*/
			animator->SetTrigger("JumpTrigger");
		}
		s_MoveSpeed = glm::clamp(s_MoveSpeed, 0.0f, 1.0f);
		if (s_MoveSpeed > 0)
			animator->SetTrigger("MoveTrigger");
		else if (s_MoveSpeed <= 0)
			animator->SetTrigger("IdleTrigger");

		animator->SetFloat("MoveSpeed", s_MoveSpeed);

		animator->Update(s_DeltaTime);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.04f));

		shader.Bind();
		shader.SetMat4("u_Model", model);
		shader.SetMat4("u_View", s_Camera.GetViewMatrix());
		shader.SetMat4("u_Projection", s_Camera.GetProjectionMatrix());

		shader.SetVec3("u_DirLight.Direction", { -0.2f, -1.0f, -0.3f });
		shader.SetVec3("u_DirLight.Ambient", { 0.2f, 0.2f, 0.2f });
		shader.SetVec3("u_DirLight.Diffuse", { 0.8f, 0.8f, 0.8f });
		shader.SetVec3("u_DirLight.Specular", { 0.3f, 0.3f, 0.3f });

		auto& skinningMatrices = animator->GetSkinningMatrices();
		for (uint32_t i = 0; i < skinningMatrices.size(); i++)
			shader.SetMat4("u_SkinningMatrices[" + std::to_string(i) + "]", skinningMatrices[i]);

		bossModel.Draw(shader);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}