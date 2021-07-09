struct GeometryInput
{
	float3 position;
};

struct GeometryOutput
{
	float4 position;
	float3 normal;
};

[maxvertexcount(3)]
void main(triangle GeometryInput input[3], inout TriangleStream<GeometryOutput> stream)
{
	const float3 first = input[0].position;
	const float3 second = input[1].position;
	const float3 third = input[2].position;
	const float3 a = (second - first).xyz;
	const float3 b = (third - first).xyz;
	const float3 normal = normalize(cross(b, a));

	for (int i = 0; i < 3; ++i)
	{
		GeometryOutput output;
		output.position = float4(input[i].position, 1);
		output.normal = normal;
		stream.Append(output);
	}
	stream.RestartStrip();
}
