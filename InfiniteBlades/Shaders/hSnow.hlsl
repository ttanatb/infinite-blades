// Input control point
struct VS_CONTROL_POINT_OUTPUT
{
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 tangent		: TANGENT;
	float3 worldPos		: WORLD_POS;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 worldPos		: WORLD_POS;
	float3 tangent		: TANGENT;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]		: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor		: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
												  // TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	//variables used for calculations later
	float3	snowAngle = float3(0, 1.0f, 0);
	float	minDotProduct = 0.f;
	int		additionalTessalation = 3 - 1;

	//how much it should tessalate
	int tessalationFactor = 1;

	//output from vertex shader
	VS_CONTROL_POINT_OUTPUT vs = ip[0];

	//calculate the dot product between normal and angle of snow
	float dotProduct = dot(vs.normal, snowAngle);
	bool mask = dotProduct >= minDotProduct;
	tessalationFactor += mask * additionalTessalation;

	HS_CONSTANT_DATA_OUTPUT Output;
	// Insert code to compute Output here
	Output.EdgeTessFactor[0] =
		Output.EdgeTessFactor[1] =
		Output.EdgeTessFactor[2] =
		Output.InsideTessFactor = tessalationFactor;

	return Output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[patchconstantfunc("CalcHSPatchConstants")]
[outputcontrolpoints(3)]
[maxtessfactor(7.0)]

HS_CONTROL_POINT_OUTPUT main(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONTROL_POINT_OUTPUT Output;

	// Insert code to compute Output here
	Output.position = ip[i].position;
	Output.normal	= ip[i].normal;
	Output.uv		= ip[i].uv;
	Output.worldPos = ip[i].worldPos;
	Output.tangent	= ip[i].tangent;
	return Output;
}
