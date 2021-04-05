struct VS_IN
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 UVCoords : UV;
	float3 Tangent : TANGENT;
};

struct VS_OUT
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 UVCoords : UV;
	float3 Tangent : TANGENT;
	float3 FragPos : POSITION;
};

cbuffer ViewProjectionBuf : register(b0)
{
	matrix ViewProj;
};

cbuffer ModelMatBuf : register(b1)
{
	matrix ModelView;
};

VS_OUT VSMain(VS_IN input) // main is the default function name
{
	VS_OUT output;
	
	float3 fragPos = mul(ModelView, float4(input.Position, 1.0f)).xyz;
	
	output.Position = mul(ViewProj, float4(fragPos, 1.0f));
	output.FragPos = fragPos;
	output.Normal = input.Normal;
	output.UVCoords = input.UVCoords;
	output.Tangent = mul(ModelView, float4(input.Tangent, 1.0f)).xyz;
	
    return output;
}