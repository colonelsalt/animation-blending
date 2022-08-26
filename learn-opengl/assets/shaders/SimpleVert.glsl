#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;

out vec3 v_Color;

void main()
{
	gl_Position = vec4(a_Position, 1.0);
	v_Color = a_Color;
}
