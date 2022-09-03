#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

static Camera s_Camera;

static float s_DeltaTime = 0.0f;
static float s_LastFrameTime = 0.0f;

/// Assuming the texture to load to is already bound
void LoadTexture(const char* path, GLenum imageFormat)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, numChannels;
	unsigned char* data = stbi_load(path, &width, &height, &numChannels, 0);
	if (!data)
	{
		std::cout << "Failed to load texture." << std::endl;
		exit(1);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

void SetTextureInterpolation()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

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

	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

	// Cube -----

	// Vertex array
	uint32_t vertexArrayId;
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	// Vertex buffer
	float vertices[] = {
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
	uint32_t vertexBufferId;
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Light ------
	uint32_t lightVertexArray;
	glGenVertexArrays(1, &lightVertexArray);
	glBindVertexArray(lightVertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	// Index buffer
	/*uint32_t indices[] = {
		0, 1, 3,
		1, 2, 3,
	};
	uint32_t indexBufferId;
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

	Shader shader("assets/shaders/SimpleVert.glsl", "assets/shaders/SimpleFrag.glsl");
	shader.Bind();

	shader.SetVec3("u_ObjectColor", { 1.0f, 0.5f, 0.31f });
	shader.SetVec3("u_LightColor", { 1.0f, 1.0f, 1.0f });

	Shader lightShader("assets/shaders/SimpleVert.glsl", "assets/shaders/WhiteFrag.glsl");


	//
	// Textures
	//

	// Bind texture slot 0
	glActiveTexture(GL_TEXTURE0);

	uint32_t texture1Id;
	glGenTextures(1, &texture1Id);
	glBindTexture(GL_TEXTURE_2D, texture1Id);

	SetTextureInterpolation();
	LoadTexture("assets/textures/container.jpg", GL_RGB);

	// Bind texture slot 1
	glActiveTexture(GL_TEXTURE1);

	uint32_t texture2Id;
	glGenTextures(1, &texture2Id);
	glBindTexture(GL_TEXTURE_2D, texture2Id);

	SetTextureInterpolation();
	LoadTexture("assets/textures/awesomeface.png", GL_RGBA);

	shader.SetInt("u_Texture1", 0);
	shader.SetInt("u_Texture2", 1);


	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), lightPos) *
		glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
	

	while (!glfwWindowShouldClose(window))
	{
		float time = glfwGetTime();
		s_DeltaTime = time - s_LastFrameTime;
		s_LastFrameTime = time;

		ProcessInput(window);
		s_Camera.UpdateInput(window, s_DeltaTime);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glBindVertexArray(lightVertexArray);
		lightShader.Bind();
		lightShader.SetMat4("u_View", s_Camera.GetViewMatrix());
		lightShader.SetMat4("u_Model", lightModel);
		lightShader.SetMat4("u_Projection", s_Camera.GetProjectionMatrix());

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(vertexArrayId);

		shader.Bind();
		shader.SetMat4("u_View", s_Camera.GetViewMatrix());
		shader.SetMat4("u_Model", glm::mat4(1.0f));
		shader.SetMat4("u_Projection", s_Camera.GetProjectionMatrix());
		
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}