#pragma pack_matrix(column_major)

struct PerObjectData
{
	column_major float4x4 model;
};

struct Matrices
{
	column_major float4x4 vp;
};

struct Lighting
{
	float3 color;
	float intensity;
};

struct PointLight
{
	Lighting lighting;
	float3 position;
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
    uint nLights;
	PointLight pointLights[32];
};
