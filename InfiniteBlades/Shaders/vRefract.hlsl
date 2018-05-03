// Constant Buffer for external
cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

// Struct representing a vertex
struct VertexShaderInput
{
	float3 position		: POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
};

// Output variable for vertex shader to pixel shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPos		: POSITION;
	noperspective float2 screenUV		: TEXCOORD1;
};

// Entry for vert shader
VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	output position
	matrix worldViewProj = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	// world position of this vertex
	output.worldPos = mul(float4(input.position, 1.0f), world).xyz;

	//normal is in WORLD space
	output.normal = mul(input.normal, (float3x3)world);
	output.normal = normalize(output.normal); // Make sure it's length is 1

	// tangent is in WORLD space + unit vector
	output.tangent = normalize(mul(input.tangent, (float3x3)world));

	// Pass thru the uv
	output.uv = input.uv;

	// Get the screen-space UV
	output.screenUV = (output.position.xy / output.position.w);
	output.screenUV.x = output.screenUV.x * 0.5f + 0.5f;
	output.screenUV.y = -output.screenUV.y * 0.5f + 0.5f;

	return output;
}