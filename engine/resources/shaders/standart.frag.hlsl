#include "common.hlsl"

struct FragmentInput
{
	float4 position : POSITION;
	float3 normal : NORMAL;
};

struct FragmentOutput
{
	float3 color : COLOR;
};

[[vk::binding(0, 2)]]
cbuffer objectLighting
{
   ObjectLightingData objectLighting;
}

[[vk::binding(0, 0)]]
tbuffer sceneLighting
{
	SceneLightingData sceneLighting;
}

float3 diffuse(float3 lightDir, float3 normal, Lighting light)
{
	float diff = max(dot(normal, lightDir), 0.0F);
	return (light.color.xyz * diff * light.intensity);
};

FragmentOutput main(FragmentInput input)
{
	float3 color = float3(0, 0, 0);
	for (uint i = 0; i < objectLighting.nLights; ++i)
	{
		uint lightIndex = objectLighting.pointLightsIndices[i];
		PointLight light = sceneLighting.pointLights[lightIndex];
		float3 lightDir = normalize(input.position - light.position);
		color += diffuse(lightDir, input.normal, light.lighting);
	}
	FragmentOutput output;
	output.color = color;

	return output;
}
