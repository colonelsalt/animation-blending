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
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	uint32_t vertexBufferId;
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// Light ------
	uint32_t lightVertexArray;
	glGenVertexArrays(1, &lightVertexArray);
	glBindVertexArray(lightVertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	Shader shader("assets/shaders/SimpleVert.glsl", "assets/shaders/SimpleFrag.glsl");
	shader.Bind();

	shader.SetVec3("u_LightColor", { 1.0f, 1.0f, 1.0f });

	Shader lightShader("assets/shaders/SimpleVert.glsl", "assets/shaders/WhiteFrag.glsl");


	//
	// Textures
	//

	// Bind texture slot 0
	glActiveTexture(GL_TEXTURE0);

	uint32_t diffuseTexture;
	glGenTextures(1, &diffuseTexture);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);

	SetTextureInterpolation();
	LoadTexture("assets/textures/container2.png", GL_RGBA);

	glActiveTexture(GL_TEXTURE1);

	uint32_t specularTexture;
	glGenTextures(1, &specularTexture);
	glBindTexture(GL_TEXTURE_2D, specularTexture);

	SetTextureInterpolation();
	LoadTexture("assets/textures/container2_specular.png", GL_RGBA);

	shader.SetInt("u_Material.Diffuse", 0);
	shader.SetInt("u_Material.Specular", 1);

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	//glm::vec3 lightPos{ 1.2f, 1.5f, 2.0f };

	while (!glfwWindowShouldClose(window))
	{
		float time = glfwGetTime();
		s_DeltaTime = time - s_LastFrameTime;
		s_LastFrameTime = time;

		ProcessInput(window);
		s_Camera.UpdateInput(window, s_DeltaTime);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vertexArrayId);

		shader.Bind();
		shader.SetMat4("u_View", s_Camera.GetViewMatrix());
		shader.SetMat4("u_Projection", s_Camera.GetProjectionMatrix());
		
		shader.SetVec3("u_ViewPos", s_Camera.GetPosition());
		
		shader.SetVec3("u_Material.Ambient", { 1.0f, 0.5f, 0.31f });
		shader.SetVec3("u_Material.Diffuse", { 1.0f, 0.5f, 0.31f });
		shader.SetVec3("u_Material.Specular", { 0.5f, 0.5f, 0.5f });
		shader.SetFloat("u_Material.Shininess", 32.0f);

		shader.SetVec3("u_DirLight.Direction", { -0.2f, 1.0f, 0.3f });
		shader.SetVec3("u_DirLight.Ambient", { 0.02f, 0.02f, 0.02f });
		shader.SetVec3("u_DirLight.Diffuse", { 0.4f, 0.4f, 0.4f });
		shader.SetVec3("u_DirLight.Specular", { 1.0f, 1.0f, 1.0f });

		for (int i = 0; i < 4; i++)
		{
			const std::string uniformName = "u_PointLights[" + std::to_string(i) + "]";
			shader.SetVec3(uniformName + ".Position", pointLightPositions[i]);
			
			shader.SetVec3(uniformName + ".Ambient", { 0.05f, 0.05f, 0.05f });
			shader.SetVec3(uniformName + ".Diffuse", { 0.5f, 0.5f, 0.5f });
			shader.SetVec3(uniformName + ".Specular", { 1.0f, 1.0f, 1.0f });

			shader.SetFloat(uniformName + ".Constant", 1.0f);
			shader.SetFloat(uniformName + ".Linear", 0.09f);
			shader.SetFloat(uniformName + ".Quadratic", 0.032f);
		}

		shader.SetVec3("u_SpotLight.Position", s_Camera.GetPosition());
		shader.SetVec3("u_SpotLight.Direction", s_Camera.GetDirection());
		shader.SetFloat("u_SpotLight.InnerCutOff", glm::cos(glm::radians(12.5f)));
		shader.SetFloat("u_SpotLight.OuterCutOff", glm::cos(glm::radians(17.5f)));
		shader.SetVec3("u_SpotLight.Ambient", { 0.0f, 0.0f, 0.0f });
		shader.SetVec3("u_SpotLight.Diffuse", { 0.5f, 0.5f, 0.5f });
		shader.SetVec3("u_SpotLight.Specular", { 1.0f, 1.0f, 1.0f });

		for (uint32_t i = 0; i < 10; i++)
		{
			float angle = 20.0f * i;
			glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePositions[i])
				* glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

			shader.SetMat4("u_Model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(lightVertexArray);
		lightShader.Bind();
		lightShader.SetMat4("u_View", s_Camera.GetViewMatrix());
		lightShader.SetMat4("u_Projection", s_Camera.GetProjectionMatrix());

		for (uint32_t i = 0; i < 4; i++)
		{
			glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), pointLightPositions[i])
				* glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
			lightShader.SetMat4("u_Model", lightModel);
			
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}