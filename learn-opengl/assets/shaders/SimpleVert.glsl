#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;
layout (location = 2) in vec2 a_TexCoord;

out vec3 v_Color;
out vec2 v_TexCoord;

uniform mat4 u_Transform;

void main()
{
	gl_Position = u_Transform * vec4(a_Position, 1.0);
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
}
