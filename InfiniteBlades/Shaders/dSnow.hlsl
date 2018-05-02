struct DS_OUTPUT
{
	float4 vPosition	: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 worldPos		: WORLD_POS;
	float3 tangent		: TANGENT;
	float3 color		: COLOR;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float4 vPosition	: SV_POSITION;	// XYZW position (System Value Position)
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 worldPos		: WORLD_POS;
	float3 tangent		: TANGENT;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]	: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor	: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	Output.vPosition =
		patch[0].vPosition * domain.x +
		patch[1].vPosition * domain.y +
		patch[2].vPosition * domain.z;

	Output.normal =
		patch[0].normal * domain.x +
		patch[1].normal * domain.y +
		patch[2].normal * domain.z;

	Output.worldPos =
		patch[0].worldPos * domain.x +
		patch[1].worldPos * domain.y +
		patch[2].worldPos * domain.z;

	Output.tangent =
		patch[0].tangent * domain.x +
		patch[1].tangent * domain.y +
		patch[2].tangent * domain.z;


	float3 snowAngle = float3(0, 1.0f, 0);
	float snowAmount = 1 - 0.7f;
	float snowRiseAmount = 0.5f;
	float3 snowColor = float3(.5f, .5f, .6f);
	float snowColorAmp = 10.f;

	float dotProduct = dot(Output.normal, snowAngle);
	bool mask = dotProduct >= snowAmount;

	dotProduct = saturate(dotProduct - snowAmount);
	Output.vPosition += mask * float4(Output.normal, 0) * snowRiseAmount * dotProduct; 
	Output.color = float3(0, 0, 0) +
		mask * snowColor * dotProduct * snowColorAmp;

	Output.uv =
		patch[0].uv * domain.x +
		patch[1].uv * domain.y +
		patch[2].uv * domain.z;

	return Output;
}
