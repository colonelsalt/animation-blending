#version 330 core

out vec4 color;

in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec3 u_ObjectColor;
uniform vec3 u_LightColor;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

void main()
{
	vec3 ambientColor = 0.1 * u_LightColor;

	vec3 lightDir = normalize(u_LightPos - v_FragPos);
	vec3 normal = normalize(v_Normal);
	float diffuseIntensity = max(dot(normal, lightDir), 0.0);
	vec3 diffuseColor = diffuseIntensity * u_LightColor;

	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	vec3 reflectionDir = reflect(-lightDir, normal);
	float specularIntensity = pow(max(dot(viewDir, reflectionDir), 0.0), 32);
	vec3 specularColor = 0.5 * specularIntensity * u_LightColor;

	color = vec4((ambientColor + diffuseColor + specularColor) * u_ObjectColor, 1.0);
}