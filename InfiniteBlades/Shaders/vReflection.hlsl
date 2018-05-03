//per-frame buffer
cbuffer perFrameData : register(b0)
{
	matrix view;
	matrix projection;
};

//per-object buffer
cbuffer perObjData : register(b1)
{
	matrix world;
	matrix reflectionMatrix;
};

// Struct representing a single vertex worth of data
struct VertexShaderInput
{
	float3 position		: POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 tangent		: TANGENT;
};

// Out of the vertex shader (and eventually input to the PS)
struct VertexToPixel
{
	float4 position		 : SV_POSITION;
	float2 uv			 : TEXCOORD;
	float3 normal		 : NORMAL;
	float3 tangent		 : TANGENT;
	float3 worldPos		 : WORLD_POS;
	float4 reflectionPos : TEXCOORD1;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// --------------------------------------------------------
VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel output;
	matrix reflectProjectWorld;

	// Calculate position
	matrix worldViewProj = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), worldViewProj);
	output.worldPos = mul(float4(input.position, 1.0f), world).xyz;

	//transfer normal to world space and normalize it
	output.normal = normalize(mul(input.normal, (float3x3)world));

	// Make sure the tangent is also in WORLD space
	output.tangent = normalize(mul(input.tangent, (float3x3)world));

	//uv goes through
	output.uv = input.uv;
	//calc projected reflection position for the reflection texture 
	reflectProjectWorld = mul(mul(world, reflectionMatrix), projection);
	output.reflectionPos = mul(float4(input.position, 1.0f), reflectProjectWorld);

	return output;
}