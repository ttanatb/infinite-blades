
// Constant Buffer
// - Allows us to define a buffer of individual variables 
//    which will (eventually) hold data from our C++ code
// - All non-pipeline variables that get their values from 
//    our C++ code must be defined inside a Constant Buffer
// - The name of the cbuffer itself is unimportant
cbuffer perFrameData : register(b0)
{
	matrix view;
	matrix projection;
};

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 position		: POSITION;     // XYZ position
	float2 uv			: TEXCOORD0;
	float4 color		: TEXCOORD1;
	float size			: SIZE;
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float2 uv			: TEXCOORD0;
	float4 color		: TEXCOORD1;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel output;

	//Since we use billboarding, everything wants to be in 2D space, so we don't need world position
	matrix viewProj = mul(view, projection);

	output.position = mul(float4(input.position, 1.0f), viewProj);

	//Billboarding calculations
	//Use the UV to offset the position
	float2 offset = input.uv * 2 - 1;
	offset *= input.size;
	offset.y *= -1;
	output.position.xy += offset;


	output.uv = input.uv;
	output.color = input.color;

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}