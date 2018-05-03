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
};

// Struct representing a single vertex worth of data
struct VertexShaderInput
{
	float3 position		: POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 tangent		: TANGENT;
	matrix instanceWorld	: WORLD_PER_INSTANCE;
};

// Out of the vertex shader (and eventually input to the PS)
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 worldPos		: WORLD_POS;
	float3 tangent		: TANGENT;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// --------------------------------------------------------
VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel output;

	// Calculate position
	matrix worldViewProj = mul(mul(input.instanceWorld, view), projection); // New world matrix!
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	// Get the normal to the pixel shader (New world matrix)
	output.normal = mul(input.normal, (float3x3)input.instanceWorld); // ASSUMING UNIFORM SCALE HERE!!!  If not, use inverse transpose of world matrix
	output.tangent = mul(input.tangent, (float3x3)input.instanceWorld); // Needed for normal mapping

																		// Get world position of vertex
	output.worldPos = mul(float4(input.position, 1.0f), input.instanceWorld).xyz;  // New world matrix!

	//uv goes through
	output.uv = input.uv;
	return output;
}