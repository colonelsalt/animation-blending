#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include "TextureHelper.h"

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
	glEnable(GL_STENCIL_TEST);

	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

	float cubeVertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};

	// Cube data ---
	uint32_t cubeVao;
	glGenVertexArrays(1, &cubeVao);
	glBindVertexArray(cubeVao);

	uint32_t cubeVertexBuffer;
	glGenBuffers(1, &cubeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

	
	// Plane data ---
	uint32_t planeVao;
	glGenVertexArrays(1, &planeVao);
	glBindVertexArray(planeVao);

	uint32_t planeVertexBuffer;
	glGenBuffers(1, &planeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, planeVertexBuffer);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

	uint32_t cubeTexture = TextureHelper::LoadTexture("marble.jpg", "assets/textures");
	uint32_t floorTexture = TextureHelper::LoadTexture("metal.png", "assets/textures");

	Shader shader("assets/shaders/SimpleVert.glsl", "assets/shaders/SimpleFrag.glsl");
	shader.Bind();
	shader.SetInt("u_Texture", 0);

	Shader outlineShader("assets/shaders/SimpleVert.glsl", "assets/shaders/BorderFrag.glsl");

	/*Model backpack("assets/models/backpack/backpack.obj");*/

	while (!glfwWindowShouldClose(window))
	{
		float time = glfwGetTime();
		s_DeltaTime = time - s_LastFrameTime;
		s_LastFrameTime = time;

		ProcessInput(window);
		s_Camera.UpdateInput(window, s_DeltaTime);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		shader.Bind();
		shader.SetMat4("u_View", s_Camera.GetViewMatrix());
		shader.SetMat4("u_Projection", s_Camera.GetProjectionMatrix());

		// Draw cube 2
		glBindVertexArray(cubeVao);
		glStencilMask(0);
		glm::mat4 cube2Model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
		shader.SetMat4("u_Model", cube2Model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Draw floor
		glBindVertexArray(planeVao);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		shader.SetMat4("u_Model", glm::mat4(1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Draw cube 1
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // fragment always passes stencil test if it's rendered
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // write 1 to stencil buffer if stencil & depth tests pass
		glStencilMask(0xFF);

		glBindVertexArray(cubeVao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glm::vec3 cube1Pos = glm::vec3(-1.0f, 0.0f, -1.0f);
		glm::mat4 cube1Model = glm::translate(glm::mat4(1.0f), cube1Pos);
		shader.SetMat4("u_Model", cube1Model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Draw cube outline
		glStencilMask(0); // disable writing to stencil buffer
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // stencil test passes if value != 1 (it will be 1 where the cube was rendered)
		glDisable(GL_DEPTH_TEST); // outline should not be affected by depth buffer
		outlineShader.Bind();
		outlineShader.SetMat4("u_View", s_Camera.GetViewMatrix());
		outlineShader.SetMat4("u_Projection", s_Camera.GetProjectionMatrix());
		glm::mat4 cube2Scale = glm::translate(glm::mat4(1.0f), cube1Pos)
			* glm::scale(glm::mat4(1.0f), glm::vec3(1.1f, 1.1f, 1.1f));
		outlineShader.SetMat4("u_Model", cube2Scale);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}