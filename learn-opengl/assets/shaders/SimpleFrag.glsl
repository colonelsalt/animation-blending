#version 330 core

struct Material {
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
};

struct Light {
	vec3 Position;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

out vec4 color;

in vec3 v_Normal;
in vec3 v_FragPos;

uniform Material u_Material;
uniform Light u_Light;

uniform vec3 u_ViewPos;

void main()
{
	vec3 ambientColor = u_Material.Ambient * u_Light.Ambient;

	vec3 lightDir = normalize(u_Light.Position - v_FragPos);
	vec3 normal = normalize(v_Normal);
	float diffuseIntensity = max(dot(normal, lightDir), 0.0);
	vec3 diffuseColor = u_Material.Diffuse * diffuseIntensity * u_Light.Diffuse;

	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	vec3 reflectionDir = reflect(-lightDir, normal);
	float specularIntensity = pow(max(dot(viewDir, reflectionDir), 0.0), u_Material.Shininess);
	vec3 specularColor = u_Material.Specular * specularIntensity * u_Light.Specular;

	color = vec4(ambientColor + diffuseColor + specularColor, 1.0);
}