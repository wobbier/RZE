
struct PS_IN
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Color : UV;
	float3 Tangent : TANGENT;
	float3 FragPos : POSITION;
};

Texture2D textures[3] : register(t0);

SamplerState s1 : register(s0);

struct LIGHT_INPUT_DESC
{
	float4 Position;
	float4 Color;
	matrix LightSpaceMat;
	float1 Strength;
	uint PlaceHolder;
	float2 Padding;
};

struct CAMERA_INPUT_DESC
{
	float3 Position;
};

struct MATERIAL_DATA
{
	float Shininess;
};

cbuffer LightConstantBuffer : register(b0, space1)
{
	LIGHT_INPUT_DESC point_lights[2];
};

cbuffer CameraRootConstant : register(b0, space2)
{
	CAMERA_INPUT_DESC cameraDesc;
};

cbuffer MaterialData : register(b0, space3)
{
	MATERIAL_DATA materialData;
};

cbuffer GlobalConstants : register(b0, space4)
{
	uint PointLightCount;
	uint DirectionalLightCount;
};

float CalculateBlinnPhong(float3 viewDir, float3 lightDir, float3 normal)
{
	float specular = 0.0f;
	
	float3 halfDir = normalize(lightDir + viewDir);
	float specAngle = max(0.0f, dot(halfDir, reflect(-lightDir, normal)));
	specular = pow(specAngle, materialData.Shininess);

	//float3 R = reflect(-lightDir, normal);
	//float3 RdotV = max(0.0f, dot(R, viewDir)); 
	//specular = pow(RdotV, materialData.Shininess);
	
	return specular;
}

float3 CalculateBumpNormal(float3 inNormal, float3 inTangent, float3 inBumpMapSample)
{
	float3 tangent = inTangent;
	tangent = normalize(tangent - dot(tangent, inNormal) * inNormal);
	
	float3 biTangent = cross(tangent, inNormal);
	float3 bumpMapNormal = (2.0f * inBumpMapSample) - 1.0f;
	
	float3x3 TBN = float3x3(tangent, biTangent, inNormal);
	float3 newNormal = normalize(mul(bumpMapNormal, TBN));
	
	return newNormal;
}

float CalcAttenuation(float dist, float falloffStart, float falloffEnd)
{
	return saturate((falloffEnd - dist) / (falloffEnd - falloffStart));
}

float CalculatePointLight(float3 pixelPos, float3 lightPos, float3 normal, float3 toEye)
{
	float tempFalloffStart = 8.0f;
	float tempFalloffEnd = 24.0f;
	
	float3 lightVec = lightPos - pixelPos;
	float distance = length(lightVec);
	
	if (distance > tempFalloffEnd)
	{
		return 0.0f;
	}
	
	lightVec = normalize(lightVec);
	
	float ndotl = max(dot(lightVec, normal), 0.0f);
	float3 lightStrength = point_lights[0].Strength * ndotl;
	
	float att = CalcAttenuation(distance, tempFalloffStart, tempFalloffEnd);
	lightStrength *= att;
	
	return lightStrength;
}

float4 PSMain(PS_IN input) : SV_TARGET
{
	float ambientCoeff = 0.25f;
	
	float3 normal = normalize(input.Normal);
	float3 tangent = normalize(input.Tangent);
	
	float4 diffSample = textures[0].Sample(s1, input.Color);
	float4 specularSample = textures[1].Sample(s1, input.Color);
	float4 bumpSample = textures[2].Sample(s1, input.Color);
	
	normal = CalculateBumpNormal(normal, tangent, bumpSample.rgb);
	
	float3 lightAccum;
	for (uint pointLightIndex = 0; pointLightIndex < PointLightCount; ++pointLightIndex)
	{
		LIGHT_INPUT_DESC light = point_lights[pointLightIndex];

		float3 lightDir = normalize(light.Position - input.FragPos);
		float3 viewDir = normalize(cameraDesc.Position - input.FragPos);
		
		float diff = max(0.0f, saturate(dot(normal, lightDir)));
		float specular = CalculateBlinnPhong(viewDir, lightDir, normal);
		
		float lightStrength = CalculatePointLight(input.FragPos, light.Position, normal, viewDir);
		
		float3 ambientResult = ambientCoeff * diffSample.rgb;
		float3 diffuseResult = light.Color.rgb * lightStrength;
		float3 specularResult = specular * lightStrength * light.Color.rgb;
		
		float3 result = (diffSample.rgb * (ambientResult + diffuseResult)) + (specularResult * specularSample.rgb);
		lightAccum += result;
	}
	
    return float4(lightAccum, 1.0f);
}