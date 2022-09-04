#pragma once

struct DirLight {
	vec3 Direction;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

struct Material
{
	sampler2D TextureDiffuse1;
	sampler2D TextureDiffuse2;
	sampler2D TextureDiffuse3;
	sampler2D TextureSpecular1;
	sampler2D TextureSpecular2;
};
uniform Material u_Material;
uniform DirLight u_DirLight;
uniform vec3 u_ViewPos;

in vec3 v_Normal;
in vec3 v_FragPos;
in vec2 v_TexCoords;

out vec4 color;

vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 ambientColor = light.Ambient * texture(u_Material.TextureDiffuse1, v_TexCoords).xyz;

	vec3 lightDir = normalize(-light.Direction);
	float diffuseIntensity = max(dot(normal, lightDir), 0.0);
	vec3 diffuseColor = texture(u_Material.TextureDiffuse1, v_TexCoords).xyz * diffuseIntensity * light.Diffuse;

	vec3 reflectionDir = reflect(-lightDir, normal);
	float specularIntensity = pow(max(dot(viewDir, reflectionDir), 0.0), 32.0);
	vec3 specularColor = texture(u_Material.TextureSpecular1, v_TexCoords).xyz * specularIntensity * light.Specular;

	return ambientColor + diffuseColor + specularColor;
}

void main()
{
	vec3 normal = normalize(v_Normal);
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	color = vec4(CalculateDirLight(u_DirLight, normal, viewDir), 1.0); 
}