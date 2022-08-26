#version 330 core

out vec4 color;

in vec3 v_Color;

void main()
{
	color = vec4(v_Color, 1.0);
}