#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include "Animator.h"

static Camera s_Camera({ 0.0f, 0.0f, 5.0f });

static float s_DeltaTime = 0.0f;
static float s_LastFrameTime = 0.0f;

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

static void PrintBoneTransformations(const std::vector<glm::mat4>& boneTransforms)
{
	int boneIndex = 0;
	for (const glm::mat4& transform : boneTransforms)
	{
		std::cout << "Bone " << boneIndex << ": " << std::endl;

		std::cout << transform[0][0] << ", ";
		std::cout << transform[1][0] << ", ";
		std::cout << transform[2][0] << ", ";
		std::cout << transform[3][0] << ", " << std::endl;

		std::cout << transform[0][1] << ", ";
		std::cout << transform[1][1] << ", ";
		std::cout << transform[2][1] << ", ";
		std::cout << transform[3][1] << ", " << std::endl;

		std::cout << transform[0][2] << ", ";
		std::cout << transform[1][2] << ", ";
		std::cout << transform[2][2] << ", ";
		std::cout << transform[3][2] << ", " << std::endl;

		std::cout << transform[0][3] << ", ";
		std::cout << transform[1][3] << ", ";
		std::cout << transform[2][3] << ", ";
		std::cout << transform[3][3] << ", " << std::endl;

		boneIndex++;
	}

}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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

	/*Model backpack("assets/models/backpack/backpack.obj");*/

	Model vampireModel("assets/models/vampire/dancing_vampire.dae");
	Animation animation("assets/models/vampire/dancing_vampire.dae", &vampireModel);
	Animator animator(&animation, true);

	while (!glfwWindowShouldClose(window))
	{
		float time = glfwGetTime();
		s_DeltaTime = time - s_LastFrameTime;
		s_LastFrameTime = time;

		ProcessInput(window);
		s_Camera.UpdateInput(window, s_DeltaTime);
		animator.Update(s_DeltaTime);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

		shader.Bind();
		shader.SetMat4("u_Model", model);
		shader.SetMat4("u_View", s_Camera.GetViewMatrix());
		shader.SetMat4("u_Projection", s_Camera.GetProjectionMatrix());

		shader.SetVec3("u_DirLight.Direction", { 0.1f, -1.0f, 0.3f });
		shader.SetVec3("u_DirLight.Ambient", { 0.2f, 0.2f, 0.2f });
		shader.SetVec3("u_DirLight.Diffuse", { 0.8f, 0.8f, 0.8f });
		shader.SetVec3("u_DirLight.Specular", { 1.0f, 1.0f, 1.0f });

		auto& boneTransforms = animator.GetBoneTransforms();
		for (uint32_t i = 0; i < boneTransforms.size(); i++)
			shader.SetMat4("u_BoneTransformations[" + std::to_string(i) + "]", boneTransforms[i]);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		vampireModel.Draw(shader);
		
		//backpack.Draw(shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}