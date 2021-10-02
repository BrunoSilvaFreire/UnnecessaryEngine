#include "common.hlsl"
#pragma pack_matrix(column_major)

[[vk::push_constant]]
cbuffer pushConstants {
    PerObjectData data;
};

[[vk::binding(0, 1)]]
cbuffer cameraData {
	Matrices matrices;
};

struct VertexInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
};

VertexOutput main(VertexInput input)
{
	const float4x4 mvp = matrices.vp * data.model;
	VertexOutput output;
	output.position = mvp * float4(input.position, 1);
	output.normal = input.normal;
	return output;
}
