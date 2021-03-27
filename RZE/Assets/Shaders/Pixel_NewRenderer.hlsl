struct PS_IN
{
	float4 Position : SV_POSITION;
	float3 FragPos : POSITION;
	float3 Normal : NORMAL;
	float2 UVCoords : UV;
};

Texture2D diffuse : register(t0);
SamplerState diffSampler : register(s0);

float4 PSMain(PS_IN input) : SV_TARGET
{
	float LightStrength_Temp = 10.0f;
	float3 LightPos_Temp = float3(0.0f, 5.0f, -20.0f);
	float3 LightColour_Temp = float3(1.0f, 1.0f, 1.0f);
	float3 Ambient_Temp = float3(0.15f, 0.15f, 0.15f);
	//float3 ObjectColour_Temp = float3(0.5f, 0.5f, 0.5f);

	float3 lightDir = normalize(LightPos_Temp - input.FragPos);
	
	float3 shadingResult;
	{
		float4 diffSample = diffuse.Sample(diffSampler, input.UVCoords);
		
		float diffuse = max(0.0f, dot(lightDir, input.Normal));
		float3 diffuseResult = diffuse * diffSample.rgb * LightColour_Temp;
		
		shadingResult = (Ambient_Temp + diffuseResult);
	}
	
	
    return float4(shadingResult, 1.0f);
}