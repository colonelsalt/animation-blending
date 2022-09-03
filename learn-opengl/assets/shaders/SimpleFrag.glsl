#version 330 core

out vec4 color;

uniform vec3 u_ObjectColor;
uniform vec3 u_LightColor;

//in vec2 v_TexCoord;

//uniform sampler2D u_Texture1;
//uniform sampler2D u_Texture2;

void main()
{
	color = vec4(u_ObjectColor * u_LightColor, 1.0);
	//color = mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, v_TexCoord), 0.2);
}