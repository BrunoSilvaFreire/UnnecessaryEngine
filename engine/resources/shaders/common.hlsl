struct PerObjectData
{
	float4x4 model;
};

struct Matrices
{
	float4x4 vp;
};

struct Lighting
{
	float3 color;
	float intensity;
};

struct PointLight
{
	float3 position;
	Lighting lighting;
};

#ifndef MAX_NUM_LIGHTS
#define MAX_NUM_LIGHTS 4
#endif

struct ObjectLightingData
{
	uint nLights;
	uint pointLightsIndices[MAX_NUM_LIGHTS];
};

struct SceneLightingData
{
	PointLight pointLights[MAX_NUM_LIGHTS];
};
