#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>

std::string ParseShader(const char* fileName)
{
	std::stringstream buffer;
	std::ifstream fileStream(fileName);
	buffer << fileStream.rdbuf();
	return buffer.str();
}

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);
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

	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

	// Vertex array
	uint32_t vertexArrayId;
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	// Vertex buffer
	float vertices[] = {
		 0.5f,  0.5f, 0.0f, // 0
		 0.5f, -0.5f, 0.0f, // 1
		-0.5f, -0.5f, 0.0f, // 2
		-0.5f,  0.5f, 0.0f, // 3
	};
	uint32_t vertexBufferId;
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	// Index buffer
	uint32_t indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	uint32_t indexBufferId;
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//
	// Shaders
	//

	// Vertex shader
	std::string vertexShaderSource = ParseShader("assets/shaders/SimpleVert.glsl");

	uint32_t vertShaderId = glCreateShader(GL_VERTEX_SHADER);
	const char* rawVertSrc = vertexShaderSource.c_str();

	glShaderSource(vertShaderId, 1, &rawVertSrc, nullptr);
	glCompileShader(vertShaderId);

	int vertShaderCompileSuccess;
	glGetShaderiv(vertShaderId, GL_COMPILE_STATUS, &vertShaderCompileSuccess);
	if (!vertShaderCompileSuccess)
	{
		char infoLog[512];
		glGetShaderInfoLog(vertShaderId, sizeof(infoLog), nullptr, infoLog);
		std::cout << "Vertex shader failed to compile:\n" << infoLog;
		return 1;
	}

	// Fragment shader
	std::string fragmentShaderSource = ParseShader("assets/shaders/SimpleFrag.glsl");

	uint32_t fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	const char* rawFragSrc = fragmentShaderSource.c_str();

	glShaderSource(fragShaderId, 1, &rawFragSrc, nullptr);
	glCompileShader(fragShaderId);

	int fragShaderCompileSuccess;
	glGetShaderiv(fragShaderId, GL_COMPILE_STATUS, &fragShaderCompileSuccess);
	if (!fragShaderCompileSuccess)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragShaderId, sizeof(infoLog), nullptr, infoLog);
		std::cout << "Fragment shader failed to compile:\n" << infoLog;
		return 1;
	}

	uint32_t shaderProgramId = glCreateProgram();
	glAttachShader(shaderProgramId, vertShaderId);
	glAttachShader(shaderProgramId, fragShaderId);
	glLinkProgram(shaderProgramId);

	int shaderLinkSuccess;
	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &shaderLinkSuccess);
	if (!shaderLinkSuccess)
	{
		char infoLog[512];
		glGetProgramInfoLog(shaderProgramId, sizeof(infoLog), nullptr, infoLog);
		std::cout << "Shader program failed to link:\n" << infoLog;
		return 1;
	}
	glUseProgram(shaderProgramId);

	glDeleteShader(vertShaderId);
	glDeleteShader(fragShaderId);

	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgramId);
		glBindVertexArray(vertexArrayId);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}