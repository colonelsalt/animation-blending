#version 330 core

struct Material {
	sampler2D Diffuse;
	sampler2D Specular;
	float Shininess;
};

struct SpotLight {
	vec3 Position;
	vec3 Direction;
	float InnerCutOff;
	float OuterCutOff;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

struct DirLight {
	vec3 Direction;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

struct PointLight {
	vec3 Position;

	float Constant;
	float Linear;
	float Quadratic;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

#define NUM_POINT_LIGHTS 4
uniform PointLight u_PointLights[NUM_POINT_LIGHTS];

out vec4 color;

in vec3 v_Normal;
in vec3 v_FragPos;
in vec2 v_TexCoords;

uniform Material u_Material;

uniform DirLight u_DirLight;
uniform SpotLight u_SpotLight;

uniform vec3 u_ViewPos;

vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec3 normal = normalize(v_Normal);
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);

	vec3 outColor = CalculateDirLight(u_DirLight, normal, viewDir);
	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
		outColor += CalculatePointLight(u_PointLights[i], normal, v_FragPos, viewDir);
	outColor += CalculateSpotLight(u_SpotLight, normal, v_FragPos, viewDir);

	color = vec4(outColor, 1.0);		
}

vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 ambientColor = light.Ambient * texture(u_Material.Diffuse, v_TexCoords).xyz;

	vec3 lightDir = normalize(-light.Direction);
	float diffuseIntensity = max(dot(normal, lightDir), 0.0);
	vec3 diffuseColor = texture(u_Material.Diffuse, v_TexCoords).xyz * diffuseIntensity * light.Diffuse;

	vec3 reflectionDir = reflect(-lightDir, normal);
	float specularIntensity = pow(max(dot(viewDir, reflectionDir), 0.0), u_Material.Shininess);
	vec3 specularColor = texture(u_Material.Specular, v_TexCoords).xyz * specularIntensity * light.Specular;

	return ambientColor + diffuseColor + specularColor;
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 ambientColor = light.Ambient * texture(u_Material.Diffuse, v_TexCoords).xyz;

	vec3 lightDir = normalize(light.Position - fragPos);
	float diffuseIntensity = max(dot(normal, lightDir), 0.0);
	vec3 diffuseColor = texture(u_Material.Diffuse, v_TexCoords).xyz * diffuseIntensity * light.Diffuse;

	float lightDist = length(fragPos - light.Position);
	float attenuation = 1.0 / (light.Constant + light.Linear * lightDist + light.Quadratic * lightDist * lightDist);

	vec3 reflectionDir = reflect(-lightDir, normal);
	float specularIntensity = pow(max(dot(viewDir, reflectionDir), 0.0), u_Material.Shininess);
	vec3 specularColor = texture(u_Material.Specular, v_TexCoords).xyz * specularIntensity * light.Specular;

	return attenuation * (ambientColor + diffuseColor + specularColor);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.Position - fragPos);

	float theta = dot(lightDir, normalize(-light.Direction));
	float epsilon = light.InnerCutOff - light.OuterCutOff;
	float intensity = clamp((theta - light.OuterCutOff) / epsilon, 0.0, 1.0);

	vec3 ambientColor = light.Ambient * texture(u_Material.Diffuse, v_TexCoords).xyz;

	float diffuseIntensity = max(dot(normal, lightDir), 0.0);
	vec3 diffuseColor = texture(u_Material.Diffuse, v_TexCoords).xyz * diffuseIntensity * light.Diffuse;

	vec3 reflectionDir = reflect(-lightDir, normal);
	float specularIntensity = pow(max(dot(viewDir, reflectionDir), 0.0), u_Material.Shininess);
	vec3 specularColor = texture(u_Material.Specular, v_TexCoords).xyz * specularIntensity * light.Specular;
	
	diffuseColor *= intensity;
	specularColor *= intensity;

	return ambientColor + diffuseColor + specularColor;
}
