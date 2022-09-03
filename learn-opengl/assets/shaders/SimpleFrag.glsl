#version 330 core

struct Material {
	sampler2D Diffuse;
	sampler2D Specular;
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
in vec2 v_TexCoords;

uniform Material u_Material;
uniform Light u_Light;

uniform vec3 u_ViewPos;

void main()
{
	vec3 ambientColor = u_Light.Ambient * texture(u_Material.Diffuse, v_TexCoords).xyz;

	vec3 lightDir = normalize(u_Light.Position - v_FragPos);
	vec3 normal = normalize(v_Normal);
	float diffuseIntensity = max(dot(normal, lightDir), 0.0);
	vec3 diffuseColor = texture(u_Material.Diffuse, v_TexCoords).xyz * diffuseIntensity * u_Light.Diffuse;

	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	vec3 reflectionDir = reflect(-lightDir, normal);
	float specularIntensity = pow(max(dot(viewDir, reflectionDir), 0.0), u_Material.Shininess);
	vec3 specularColor = texture(u_Material.Specular, v_TexCoords).xyz * specularIntensity * u_Light.Specular;

	color = vec4(ambientColor + diffuseColor + specularColor, 1.0);
}