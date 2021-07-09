#include "common.hlsl"

[[vk::push_constant]]
cbuffer pushConstants {
PerObjectData data;
};

[[vk::binding(0, 0)]]
cbuffer cameraData {
	Matrices matrices;
};

struct VertexInput
{
	float3 position : POSITION;
};

struct VertexOutput
{
	float4 position : POSITION;
};

VertexOutput main(VertexInput input)
{
	const float4x4 mvp = matrices.vp * data.model;
	VertexOutput output;
	output.position = mvp * float4(input.position, 1);
	return output;
}
