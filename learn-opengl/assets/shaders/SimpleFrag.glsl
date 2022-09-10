#version 330 core

out vec4 color;

in vec2 v_TexCoords;

uniform sampler2D u_Texture;

const float near = 0.1;
const float far = 100.0;

float LineariseDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
	float depth = LineariseDepth(gl_FragCoord.z) / far;
	color = vec4(vec3(depth), 1.0);
	//color = texture(u_Texture, v_TexCoords);
}