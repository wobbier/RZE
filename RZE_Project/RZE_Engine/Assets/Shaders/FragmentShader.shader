#version 330

in vec3 Normal;
in vec3 FragPos;

out vec4 OutFragmentColor;

uniform vec3 ULightPosition;
uniform vec3 UViewPosition;
uniform vec3 ULightColor;
uniform float ULightStrength;

uniform vec4 UFragColor;

void main()
{
	vec3 ambient = ULightStrength * ULightColor;
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(ULightPosition - FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * ULightColor;
	
	float specularStrength = 0.75f;
    vec3 viewDir = normalize(UViewPosition - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * ULightColor;  
	
	vec3 result = (ambient + diffuse + specular) * UFragColor.xyz;
	OutFragmentColor = vec4(result, 1.0f);
}